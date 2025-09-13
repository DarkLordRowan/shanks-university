import pathlib

from shanks_consumer.src.shanks_consumer.params import (
    AccelParamModule,
    SeriesParamModule,
)
from shanks_consumer.src.shanks_consumer.trial import ComplexTrial
from shanks_consumer.src.shanks_consumer.export import ExportTrialResult

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
