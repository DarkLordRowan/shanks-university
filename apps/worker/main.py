import io

from fastapi import FastAPI, Body
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse, StreamingResponse
from src import export, params, trial

app = FastAPI(title="Shanks Worker Service", version="1.0.0", root_path="/api")

origins = [
    "http://localhost:5173",
    "http://127.0.0.1:5173",
    "http://localhost:3000",
    "http://127.0.0.1:3000",
    "https://series-accelerator.mirea-kmbo.ru",
    "http://series-accelerator.mirea-kmbo.ru",
]
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=False,
    allow_methods=["GET", "POST", "OPTIONS"],
    allow_headers=["*"],
    expose_headers=["Content-Disposition"],
)


def _compute_results(payload: dict):
    st = trial.ComplexTrial(
        [*params.load_series_params_from_data(payload)],
        [*params.load_accel_params_from_data(payload)],
    )
    return st.execute()


@app.post("/process/json")
async def process_json(payload: dict = Body(...)):
    results = _compute_results(payload)
    exporter = export.ExportTrialResults(results)
    return JSONResponse(content=exporter.as_dict())


@app.post("/process/csv")
async def process_csv(payload: dict = Body(...)):
    results = _compute_results(payload)
    exporter = export.ExportTrialResults(results)
    content = exporter.to_csv_bytes()
    return StreamingResponse(
        io.BytesIO(content),
        media_type="text/csv",
        headers={"Content-Disposition": 'attachment; filename="results.csv"'}
    )
