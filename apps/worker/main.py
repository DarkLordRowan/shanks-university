from fastapi import FastAPI, Body
from fastapi.responses import JSONResponse

from shanks_consumer import export, params, trial

app = FastAPI(title="Shanks Worker Service")

def proc(json_data: dict) -> dict:
    """
    Принимает Python-словарь (распарсенный JSON), запускает ComplexTrial
    и возвращает результат как dict.
    """
    st = trial.ComplexTrial(
        [*params.get_series_params_from_json_location(json_data)],
        [*params.get_accel_params_from_json(json_data)],
    )
    results = st.execute()
    exporter = export.ExportTrialResults(results)
    return exporter.to_json()

@app.post("/process")
async def process_json(payload: dict = Body(...)):
    result = proc(payload)
    return JSONResponse(content=result)


if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=8000)
