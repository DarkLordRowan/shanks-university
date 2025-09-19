import io
import os as _os
from datetime import datetime as _dt
from typing import Dict as _Dict, Any as _Any

from fastapi import FastAPI as _FastAPI, Body as _Body, Header as _Header, HTTPException as _HTTPException
from fastapi.middleware.cors import CORSMiddleware as _CORSMiddleware
from motor.motor_asyncio import AsyncIOMotorClient as _AsyncIOMotorClient
import httpx as _httpx
from bson import Binary as _Binary

from src import export, params, trial, events
from starlette.responses import JSONResponse, StreamingResponse

_MONGODB_URI = _os.getenv("MONGODB_URI", "mongodb://localhost:27017")
_MONGODB_DB = _os.getenv("MONGODB_DB", "shanks")
_GATEWAY_URL = _os.getenv("GATEWAY_URL", "http://localhost:8000")
_GATEWAY_TOKEN = _os.getenv("GATEWAY_TOKEN", "super-secret-gateway-token")
_WORKER_TOKEN = _os.getenv("WORKER_TOKEN", "super-secret-worker-token")

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


def _compute_results(payload: dict):
    st = trial.ComplexTrial(
        [*params.load_series_params_from_data(payload)],
        [*params.load_accel_params_from_data(payload)],
    )
    return st.execute()


@_worker_app.post("/jobs")
async def create_job(body: _Dict[str, _Any] = _Body(...), authorization: str | None = _Header(default=None)):
    if authorization != f"Bearer {_WORKER_TOKEN}":
        raise _HTTPException(status_code=401, detail="unauthorized")

    _uuid = body.get("uuid")
    payload = body.get("payload")
    if not _uuid or payload is None:
        raise _HTTPException(status_code=400, detail="uuid and payload are required")

    await _jobs.update_one(
        {"uuid": _uuid},
        {"$set": {"status": "processing"}},
        upsert=True,
    )

    try:
        results = _compute_results(payload)

        results_exporter = export.ExportTrialResults(results)
        results_json = results_exporter.as_dict()
        results_csv_bytes = results_exporter.to_csv_bytes()

        scanner = events.TrialEventScanner(results)
        eventse = scanner.execute()
        events_exporter = export.ExportTrialEvents(eventse)
        events_json = events_exporter.as_dict()
        events_csv_bytes = events_exporter.to_csv_bytes()

        # Сохраняем документы в MongoDB
        now = _dt.utcnow()
        docs_to_insert = [
            {"uuid": _uuid, "kind": "results", "format": "json", "content": results_json, "created_at": now},
            {"uuid": _uuid, "kind": "results", "format": "csv", "content": _Binary(results_csv_bytes), "created_at": now},
            {"uuid": _uuid, "kind": "events",  "format": "json", "content": events_json,  "created_at": now},
            {"uuid": _uuid, "kind": "events",  "format": "csv", "content": _Binary(events_csv_bytes),  "created_at": now},
        ]
        await _documents.delete_many({"uuid": _uuid})
        await _documents.insert_many(docs_to_insert)

        await _jobs.update_one({"uuid": _uuid}, {"$set": {"status": "ready", "completed_at": now, "error": None}})

        async with _httpx.AsyncClient(timeout=30) as client:
            r = await client.post(
                f"{_GATEWAY_URL}/internal/job-complete",
                headers={"Authorization": f"Bearer {_GATEWAY_TOKEN}"},
                json={"uuid": _uuid},
            )
            r.raise_for_status()

        return {"ok": True}

    except Exception as e:
        await _jobs.update_one(
            {"uuid": _uuid},
            {"$set": {"status": "failed", "completed_at": _dt.utcnow(), "error": str(e)}}
        )
        raise _HTTPException(status_code=500, detail=f"processing failed: {e}")


@_worker_app.post("/process/json")
async def legacy_process_json(payload: dict = _Body(...)):
    results = _compute_results(payload)
    exporter = export.ExportTrialResults(results)
    return JSONResponse(content=exporter.as_dict())


@_worker_app.post("/process/csv")
async def legacy_process_csv(payload: dict = _Body(...)):
    results = _compute_results(payload)
    exporter = export.ExportTrialResults(results)
    content = exporter.to_csv_bytes()
    return StreamingResponse(
        io.BytesIO(content),
        media_type="text/csv",
        headers={"Content-Disposition": 'attachment; filename="results.csv"'}
    )


app = _worker_app
