import pathlib

from src.params import (
    SeriesParamModule,
    get_accel_params_from_json,
    get_series_params_from_json,
)
from src.trial import ComplexTrial
from src.export import ExportTrialResults

import pyshanks

if __name__ == "__main__":
    st = ComplexTrial(
        [
            *get_series_params_from_json(pathlib.Path("data/example.json")),
        ],
        [
            *get_accel_params_from_json(pathlib.Path("data/example.json")),
        ],
    )
    results = st.execute()
    exporter = ExportTrialResults(results)

    exporter.to_json(pathlib.Path("output/output.json"))
    exporter.to_csv(pathlib.Path("output/output.csv"))
    # InteractiveConvergencePlot(results).show()
