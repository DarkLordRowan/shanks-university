from fastapi import FastAPI

from src.lifespan import lifespan_setup
from src.views import api_router


def get_app():
    app = FastAPI(
        title="MongoDB Results Viewer API",
        description="Read-only MongoDB from shanks_consumer",
        version="1.0.0",
        lifespan=lifespan_setup,
        docs_url="/docs",
    )

    app.include_router(api_router)

    return app
