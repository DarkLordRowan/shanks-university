import io
import os as _os
import json as _json
import gzip as _gzip
from datetime import datetime as _dt
from typing import Dict as _Dict, Any as _Any

from fastapi import FastAPI as _FastAPI, Body as _Body, Header as _Header, HTTPException as _HTTPException
from fastapi.middleware.cors import CORSMiddleware as _CORSMiddleware
from motor.motor_asyncio import AsyncIOMotorClient as _AsyncIOMotorClient
from motor.motor_asyncio import AsyncIOMotorGridFSBucket as _AsyncIOMotorGridFSBucket
import httpx as _httpx
from bson import Binary as _Binary
import logging as _logging
import asyncio
from concurrent.futures import ProcessPoolExecutor as _ProcessPoolExecutor

try:
    import resource as _resource
except Exception:
    _resource = None

from src import export, params, trial, events
from starlette.responses import JSONResponse, StreamingResponse

_MONGODB_URI = _os.getenv("MONGODB_URI", "mongodb://app_user:apppass@127.0.0.1:27017/shanks?authSource=admin")
_MONGODB_DB = _os.getenv("MONGODB_DB", "shanks")
_GATEWAY_URL = _os.getenv("GATEWAY_URL", "http://127.0.0.1:8002")
_GATEWAY_TOKEN = _os.getenv("GATEWAY_TOKEN", "super-secret-gateway-token")
_WORKER_TOKEN = _os.getenv("WORKER_TOKEN", "super-secret-worker-token")

# Параметры управления нагрузкой
_WORKER_MAX_JOBS = int(_os.getenv("WORKER_MAX_JOBS", "2"))
_WORKER_MAX_PROCS = int(_os.getenv("WORKER_MAX_PROCS", "2"))
_WORKER_NICE = int(_os.getenv("WORKER_NICE", "10"))
_WORKER_CHILD_MEM_MB = int(_os.getenv("WORKER_CHILD_MEM_MB", "0"))

_worker_app = _FastAPI(title="Shanks Worker Service", version="1.0.0", root_path="/api")

_origins = (
    _os.getenv("CORS_ORIGINS", "").split(",") if _os.getenv("CORS_ORIGINS") else [
        "http://localhost:5173",
        "http://127.0.0.1:5173",
        "http://localhost:3000",
        "http://127.0.0.1:3000",
    ]
)

_worker_app.add_middleware(
    _CORSMiddleware,
    allow_origins=_origins,
    allow_credentials=False,
    allow_methods=["GET", "POST", "OPTIONS"],
    allow_headers=["*"],
    expose_headers=["Content-Disposition"],
)

_mongo_client = _AsyncIOMotorClient(_MONGODB_URI)
_db = _mongo_client[_MONGODB_DB]
_jobs = _db["jobs"]
_documents = _db["documents"]
_fs = _AsyncIOMotorGridFSBucket(_db, bucket_name="artifacts")

# ----- CPU-тяжёлые функции -----

def _compute_results(_payload: dict):
    st = trial.ComplexTrial(
        [*params.load_series_params_from_data(_payload)],
        [*params.load_accel_params_from_data(_payload)],
    )
    return st.execute()

def _compute_results_json(_payload: dict):
    results = _compute_results(_payload)
    return export.ExportTrialResults(results).as_dict()

def _compute_results_csv_bytes(_payload: dict):
    results = _compute_results(_payload)
    return export.ExportTrialResults(results).to_csv_bytes()

def _compute_and_export(_payload: dict):
    results = _compute_results(_payload)

    results_exporter = export.ExportTrialResults(results)
    results_json = results_exporter.as_dict()
    results_csv_bytes = results_exporter.to_csv_bytes()

    scanner = events.TrialEventScanner(results)
    eventse = scanner.execute()
    events_exporter = export.ExportTrialEvents(eventse)
    events_json = events_exporter.as_dict()
    events_csv_bytes = events_exporter.to_csv_bytes()

    return results_json, results_csv_bytes, events_json, events_csv_bytes

# ----- вспомогательные функции хранения -----

def _to_gz_bytes(obj: _Any) -> bytes:
    raw = _json.dumps(obj, ensure_ascii=False, separators=(",", ":")).encode("utf-8")
    return _gzip.compress(raw, mtime=0)

async def _store_bytes(_name: str, _data: bytes, _content_type: str, _encoding: str | None = None):
    _bio = io.BytesIO(_data)
    _file_id = await _fs.upload_from_stream(_name, _bio, metadata={"content_type": _content_type, "encoding": _encoding})
    return {"storage": "gridfs", "file_id": _file_id, "filename": _name, "content_type": _content_type, "encoding": _encoding, "size": len(_data)}

# ----- настройка процессного пула -----

def _child_initializer():
    try:
        if _os.name == "posix":
            try:
                _os.nice(_WORKER_NICE)
            except Exception:
                pass
        if _WORKER_CHILD_MEM_MB > 0 and _resource is not None:
            _bytes_lim = _WORKER_CHILD_MEM_MB * 1024 * 1024
            try:
                _resource.setrlimit(_resource.RLIMIT_AS, (_bytes_lim, _bytes_lim))
            except Exception:
                pass
    except Exception:
        pass

_EXECUTOR = _ProcessPoolExecutor(
    max_workers=_WORKER_MAX_PROCS,
    initializer=_child_initializer,
)

_CONCURRENCY_SEM = asyncio.Semaphore(_WORKER_MAX_JOBS)

# ----- основной конвейер -----

async def _process_job(_uuid: str, payload: dict):
    async with _CONCURRENCY_SEM:
        try:
            await _jobs.update_one({"uuid": _uuid}, {"$set": {"status": "processing"}})

            loop = asyncio.get_running_loop()
            results_json, results_csv_bytes, events_json, events_csv_bytes = await loop.run_in_executor(
                _EXECUTOR, _compute_and_export, payload
            )
            print("req")

            now = _dt.utcnow()
            await _documents.delete_many({"uuid": _uuid})

            results_json_gz = _to_gz_bytes(results_json)
            events_json_gz = _to_gz_bytes(events_json)
            results_csv = results_csv_bytes
            events_csv = events_csv_bytes

            rj_meta = await _store_bytes(f"{_uuid}__results.json.gz", results_json_gz, "application/json", "gzip")
            ej_meta = await _store_bytes(f"{_uuid}__events.json.gz",  events_json_gz,  "application/json", "gzip")
            rc_meta = await _store_bytes(f"{_uuid}__results.csv",     results_csv,     "text/csv", None)
            ec_meta = await _store_bytes(f"{_uuid}__events.csv",      events_csv,      "text/csv", None)

            docs = [
                {"uuid": _uuid, "kind": "results", "format": "json", **rj_meta, "created_at": now},
                {"uuid": _uuid, "kind": "results", "format": "csv",  **rc_meta, "created_at": now},
                {"uuid": _uuid, "kind": "events",  "format": "json", **ej_meta, "created_at": now},
                {"uuid": _uuid, "kind": "events",  "format": "csv",  **ec_meta, "created_at": now},
            ]
            await _documents.insert_many(docs)

            await _jobs.update_one({"uuid": _uuid}, {"$set": {"status": "ready", "completed_at": now, "error": None}})

            timeout = _httpx.Timeout(connect=2.0, read=5.0, write=5.0, pool=None)
            async with _httpx.AsyncClient(timeout=timeout) as client:
                r = await client.post(
                    f"{_GATEWAY_URL}/internal/job-complete",
                    headers={"Authorization": f"Bearer {_GATEWAY_TOKEN}"},
                    json={"uuid": _uuid},
                )
                r.raise_for_status()

        except Exception as e:
            _logging.exception("Job %s failed", _uuid)
            await _jobs.update_one(
                {"uuid": _uuid},
                {"$set": {"status": "failed", "completed_at": _dt.utcnow(), "error": f"{type(e).__name__}: {e}"}}
            )

# ----- API -----

@_worker_app.post("/jobs", status_code=202)
async def create_job(body: _Dict[str, _Any] = _Body(...), authorization: str | None = _Header(default=None)):
    if authorization != f"Bearer {_WORKER_TOKEN}":
        raise _HTTPException(status_code=401, detail="unauthorized")

    _uuid = body.get("uuid")
    payload = body.get("payload")
    if not _uuid or payload is None:
        raise _HTTPException(status_code=400, detail="uuid and payload are required")

    await _jobs.update_one(
        {"uuid": _uuid},
        {"$setOnInsert": {"created_at": _dt.utcnow()},
         "$set": {"status": "queued", "error": None, "completed_at": None}},
        upsert=True,
    )

    asyncio.create_task(_process_job(_uuid, payload))

    return {"ok": True, "uuid": _uuid, "status": "queued"}

@_worker_app.post("/process/json")
async def legacy_process_json(payload: dict = _Body(...)):
    loop = asyncio.get_running_loop()
    results_json = await loop.run_in_executor(_EXECUTOR, _compute_results_json, payload)
    return JSONResponse(content=results_json)

@_worker_app.post("/process/csv2")
async def legacy_process_csv(payload: dict = _Body(...)):
    loop = asyncio.get_running_loop()
    content = await loop.run_in_executor(_EXECUTOR, _compute_results_csv_bytes, payload)
    return StreamingResponse(
        io.BytesIO(content),
        media_type="text/csv",
        headers={"Content-Disposition": 'attachment; filename="results.csv"'}
    )

@_worker_app.get("/health")
async def _health():
    return {"status": "ok"}

app = _worker_app

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=8000)
