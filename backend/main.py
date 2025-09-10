from typing import Dict, Optional
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field

from pyshanks_wrapper import ShanksEngine, SeriesResult, AccelResult

app = FastAPI(title="Series Computation API", version="0.1.0", root_path="/api")
engine = ShanksEngine()

class SeriesComputeRequest(BaseModel):
    series: str = Field(..., description="ключ ряда (см. /series)")
    x: float
    n: int = Field(..., ge=1)

class SeriesComputeResponse(BaseModel):
    sum: float
    term: Optional[float] = None
    trace: Optional[list[float]] = None

class AccelComputeRequest(BaseModel):
    accelerator: str = Field(..., description="ключ ускорителя (см. /accelerations)")
    series: str
    x: float
    n: int = Field(..., ge=1)
    order: int = Field(..., ge=1)

class AccelComputeResponse(BaseModel):
    accelerated: float
    difference: Optional[float] = None

@app.get("/healthz")
def healthz():
    return {"status": "ok"}

@app.get("/series", response_model=Dict[str, str])
def list_series():
    return engine.list_series()

@app.get("/accelerations", response_model=Dict[str, str])
def list_accelerations():
    return engine.list_accels()

@app.post("/series/compute", response_model=SeriesComputeResponse)
def compute_series(req: SeriesComputeRequest):
    try:
        res: SeriesResult = engine.series_compute(req.series, req.x, req.n)
        return SeriesComputeResponse(**res.__dict__)
    except KeyError as e:
        raise HTTPException(status_code=404, detail=str(e))
    except Exception:
        raise HTTPException(status_code=500, detail="Computation error")

@app.post("/acceleration/compute", response_model=AccelComputeResponse)
def compute_acceleration(req: AccelComputeRequest):
    try:
        res: AccelResult = engine.accelerate(req.accelerator, req.series, req.x, req.n, req.order)
        return AccelComputeResponse(**res.__dict__)
    except KeyError as e:
        raise HTTPException(status_code=404, detail=str(e))
    except Exception:
        raise HTTPException(status_code=500, detail="Acceleration error")

# диагностика конкретного класса в .pyd
@app.get("/inspect/{class_name}")
def inspect_class(class_name: str):
    try:
        return engine.inspect_class(class_name)
    except KeyError as e:
        raise HTTPException(status_code=404, detail=str(e))
