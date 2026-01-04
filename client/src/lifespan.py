from contextlib import asynccontextmanager
from typing import AsyncGenerator

from fastapi import FastAPI
from motor.motor_asyncio import AsyncIOMotorClient

from src.settings import settings


async def _setup_db(app: FastAPI) -> None:
    client = AsyncIOMotorClient(settings.mongo_url)
    db = client[settings.database]
    collection = db[settings.collection]

    app.state.db_client = client
    app.state.db_access = db
    app.state.db_collection = collection


@asynccontextmanager
async def lifespan_setup(app: FastAPI) -> AsyncGenerator[None, None]:

    await _setup_db(app)

    yield

    app.state.db_client.close()
