import pathlib

from shanks_consumer.src.shanks_consumer.params import (
    AccelParamModule,
    SeriesParamModule,
    get_accel_params_from_json,
    get_series_params_from_json,
)
from shanks_consumer.src.shanks_consumer.trial import ComplexTrial
from shanks_consumer.src.shanks_consumer.export import ExportTrialResult

import pyshanks

if __name__ == "__main__":
    st = ComplexTrial(
        [
            SeriesParamModule(
                pyshanks.CosSeries,
                x=[100],
            ),
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
