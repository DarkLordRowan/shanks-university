import pathlib

from src.params import (
    AccelParamModule,
    SeriesParamModule,
    get_series_params_from_csv,
    get_series_params_from_json,
    get_accel_params_from_json,
)
from src.trial import ComplexTrial
from dataclasses import asdict
import json


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
    print(
        json.dumps(
            [asdict(result) for result in st.execute()],
            indent=4,
            sort_keys=True,
        )
    )
