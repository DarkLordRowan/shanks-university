import io

from fastapi import FastAPI, Body
from fastapi.responses import JSONResponse
from fastapi.responses import StreamingResponse
from src import export, params, trial

app = FastAPI(title="Shanks Worker Service", version="1.0.0", root_path="/api")


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
