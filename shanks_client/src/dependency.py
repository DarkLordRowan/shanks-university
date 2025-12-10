from starlette.requests import Request


async def get_collection(request: Request):
    return request.app.state.db_collection
