from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
import series_module
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI(
    title="Series Computation API",
    version="0.1.0",
    root_path="/api"
)

# Разрешить CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:5173"],  # адрес фронтенда
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Поддерживаемые ряды и ускорители
AVAILABLE_SERIES = {
    "exp": series_module.ExpSeries,
    # можно добавить sin, cos и т.д.
}

AVAILABLE_ACCEL = {
    "ford-sidi-2": series_module.FordSidiTwo,
    # добавить другие алгоритмы
}

# Pydantic-модели
class SeriesComputeRequest(BaseModel):
    series: str = Field(..., description="Key of the series, e.g. 'exp'")
    x: float = Field(..., description="Argument x")
    n: int = Field(..., description="Number of terms n")

class SeriesComputeResponse(BaseModel):
    sum: float
    term: float


class AccelComputeRequest(BaseModel):
    series: str
    accelerator: str
    x: float
    n: int
    order: int

class AccelComputeResponse(BaseModel):
    accelerated: float
    difference: float


@app.get("/series", response_model=dict)
def list_series():
    return {key: cls.__name__ for key, cls in AVAILABLE_SERIES.items()}


@app.get("/accelerations", response_model=dict)
def list_accelerations():
    return {key: cls.__name__ for key, cls in AVAILABLE_ACCEL.items()}


@app.post("/series/compute", response_model=SeriesComputeResponse)
def compute_series(req: SeriesComputeRequest):
    if req.series not in AVAILABLE_SERIES:
        raise HTTPException(status_code=404, detail="Series not found")
    # Создаём объект
    series_cls = AVAILABLE_SERIES[req.series]
    obj = series_cls(req.x)
    # Вычисления
    try:
        s = obj.S_n(req.n)
        a = obj.term(req.n)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
    return {"sum": s, "term": a}


@app.post("/acceleration/compute", response_model=AccelComputeResponse)
def compute_acceleration(req: AccelComputeRequest):
    if req.series not in AVAILABLE_SERIES:
        raise HTTPException(status_code=404, detail="Series not found")
    if req.accelerator not in AVAILABLE_ACCEL:
        raise HTTPException(status_code=404, detail="Accelerator not found")
    # создаём серию и ускоритель
    series_obj = AVAILABLE_SERIES[req.series](req.x)
    accel_cls = AVAILABLE_ACCEL[req.accelerator]
    accel_obj = accel_cls(series_obj)
    try:
        t = accel_obj.accelerate(req.n, req.order)
        s = series_obj.S_n(req.n)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
    return {"accelerated": t, "difference": t - s}
