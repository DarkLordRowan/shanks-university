"""Run the Shanks client API with ``python -m client``."""

import uvicorn


def main() -> None:
    """Start the development ASGI server."""
    uvicorn.run(
        "shanks.client:get_app",
        host="0.0.0.0",
        port=8000,
        reload=True,
        log_level="info",
    )


if __name__ == "__main__":
    main()
