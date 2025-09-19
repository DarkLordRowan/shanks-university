from datetime import datetime
import json
import os
import uuid as uuidlib
from typing import Dict, Any

import httpx
from fastapi import FastAPI, Body, WebSocket, WebSocketDisconnect, Header, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse, StreamingResponse
from pydantic import BaseModel
from motor.motor_asyncio import AsyncIOMotorClient
from bson import Binary

MONGODB_URI = os.getenv("MONGODB_URI", "mongodb://localhost:27017")
MONGODB_DB = os.getenv("MONGODB_DB", "shanks")
WORKER_URL = os.getenv("WORKER_URL", "http://localhost:8001/api")
WORKER_TOKEN = os.getenv("WORKER_TOKEN", "super-secret-worker-token")
GATEWAY_TOKEN = os.getenv("GATEWAY_TOKEN", "super-secret-gateway-token")

origins = (os.getenv("CORS_ORIGINS", "").split(",") if os.getenv("CORS_ORIGINS") else [
    "http://localhost:5173",
    "http://127.0.0.1:5173",
    "http://localhost:3000",
    "http://127.0.0.1:3000",
])

app = FastAPI(title="Shanks Gateway Service", version="1.0.0")
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=False,
    allow_methods=["GET", "POST", "OPTIONS"],
    allow_headers=["*"],
    expose_headers=["Content-Disposition"],
)

mongo_client = AsyncIOMotorClient(MONGODB_URI)
db = mongo_client[MONGODB_DB]
jobs = db["jobs"]
documents = db["documents"]


class ConnectionManager:
    def __init__(self):
        self.active: Dict[str, set[WebSocket]] = {}

    async def connect(self, uuid: str, websocket: WebSocket):
        await websocket.accept()
        self.active.setdefault(uuid, set()).add(websocket)

    def disconnect(self, uuid: str, websocket: WebSocket):
        if uuid in self.active:
            self.active[uuid].discard(websocket)
            if not self.active[uuid]:
                del self.active[uuid]

    async def send(self, uuid: str, message: Dict[str, Any]):
        if uuid not in self.active:
            return
        dead = []
        for ws in list(self.active[uuid]):
            try:
                await ws.send_json(message)
            except Exception:
                dead.append(ws)
        for ws in dead:
            self.disconnect(uuid, ws)

manager = ConnectionManager()


class ProcessResponse(BaseModel):
    uuid: str


@app.websocket("/ws/{uuid}")
async def ws_status(websocket: WebSocket, uuid: str):
    await manager.connect(uuid, websocket)
    try:
        while True:
            # Пинг-понг/ожидание закрытия
            await websocket.receive_text()
    except WebSocketDisconnect:
        manager.disconnect(uuid, websocket)


@app.post("/process/json", response_model=ProcessResponse)
async def process_json(payload: Dict[str, Any] = Body(...)):
    _uuid = str(uuidlib.uuid4())

    await jobs.insert_one({
        "uuid": _uuid,
        "status": "queued",
        "payload": payload,
        "created_at": datetime.utcnow(),
        "completed_at": None,
        "error": None,
    })

    async with httpx.AsyncClient(timeout=60) as client:
        resp = await client.post(
            f"{WORKER_URL}/jobs",
            headers={"Authorization": f"Bearer {WORKER_TOKEN}"},
            json={"uuid": _uuid, "payload": payload},
        )
        resp.raise_for_status()

    return ProcessResponse(uuid=_uuid)


@app.get("/jobs/{uuid}/status")
async def get_status(uuid: str):
    doc = await jobs.find_one({"uuid": uuid}, {"_id": 0, "payload": 0})
    if not doc:
        raise HTTPException(status_code=404, detail="job not found")
    return doc


@app.get("/jobs/{uuid}/docs/{doc_name}")
async def get_document(uuid: str, doc_name: str):
    try:
        kind, fmt = doc_name.split(".")
    except ValueError:
        raise HTTPException(status_code=400, detail="doc_name must be like 'results.json' or 'events.csv'")

    doc = await documents.find_one({"uuid": uuid, "kind": kind, "format": fmt})
    if not doc:
        raise HTTPException(status_code=404, detail="document not found")

    if fmt == "json":
        return JSONResponse(content=doc.get("content", {}))
    elif fmt == "csv":
        content = bytes(doc.get("content") or b"")
        return StreamingResponse(
            iter([content]),
            media_type="text/csv",
            headers={"Content-Disposition": f'attachment; filename="{kind}.csv"'}
        )
    else:
        raise HTTPException(status_code=400, detail="unsupported format")


@app.post("/internal/job-complete")
async def job_complete(payload: Dict[str, Any] = Body(...), authorization: str | None = Header(default=None)):
    if authorization != f"Bearer {GATEWAY_TOKEN}":
        raise HTTPException(status_code=401, detail="unauthorized")

    _uuid = payload.get("uuid")
    if not _uuid:
        raise HTTPException(status_code=400, detail="uuid is required")

    upd = await jobs.update_one(
        {"uuid": _uuid},
        {"$set": {"status": "ready", "completed_at": datetime.utcnow(), "error": None}}
    )
    if upd.matched_count == 0:
        raise HTTPException(status_code=404, detail="job not found")

    await manager.send(_uuid, {
        "type": "completed",
        "uuid": _uuid,
        "available": ["results.json", "results.csv", "events.json", "events.csv"],
    })
    return {"ok": True}