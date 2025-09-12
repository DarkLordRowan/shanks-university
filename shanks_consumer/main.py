import pathlib

from src.params import (
    AccelParamModule,
    SeriesParamModule,
)
from src.trial import ComplexTrial
from src.export import ExportTrialResult

import pyshanks

if __name__ == "__main__":
    st = ComplexTrial(
        [
            SeriesParamModule(
                pyshanks.CosSeries,
                x=[5],
            ),
        ],
        [
            AccelParamModule(pyshanks.ShanksAlgorithm, range(10, 20), [4]),
        ],
    )
    exporter = ExportTrialResult(st.execute())

    exporter.to_json(pathlib.Path("output.json"))
    exporter.to_csv(pathlib.Path("output.csv"))
