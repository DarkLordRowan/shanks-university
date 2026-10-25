from fastapi import FastAPI

from shanks.client.lifespan import lifespan_setup
from shanks.client.views import api_router


def get_app():
    app = FastAPI(
        title="MongoDB Results Viewer API",
        description="Read-only MongoDB from runner",
        version="1.0.0",
        lifespan=lifespan_setup,
        docs_url="/docs",
    )

    app.include_router(api_router)

    return app
