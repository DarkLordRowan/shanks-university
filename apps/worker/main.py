from fastapi import FastAPI, Body
from fastapi.responses import JSONResponse

from src import export, params, trial

app = FastAPI(title="Shanks Worker Service", version="1.0.0", root_path="/api")

@app.post("/process")
async def process_json(payload: dict = Body(...)):
    result = proc(payload)
    return JSONResponse(content=result)

def proc(json_data: dict) -> dict:
    st = trial.ComplexTrial(
        [*params.load_series_params_from_data(json_data)],
        [*params.load_accel_params_from_data(json_data)],
    )
    results = st.execute()
    exporter = export.ExportTrialResults(results)
    return exporter.as_dict()