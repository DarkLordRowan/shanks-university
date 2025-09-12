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
                pyshanks.BinSeries,
                a=[1, 2, 3, 4, 5],
                b=[1, 1, 1, 1, 1],
            ),
            *get_series_params_from_csv(pathlib.Path("example_series.csv")),
            *get_series_params_from_json(pathlib.Path("example.json")),
        ],
        [
            AccelParamModule(
                pyshanks.LevinAlgorithm,
                [10, 15, 20],
                [4, 5],
                variant=[pyshanks.RemainderType(1)],
                useRecFormulas=[True, False],
            ),
            AccelParamModule(pyshanks.ShanksAlgorithm, [10, 15, 20], [4, 5]),
            *get_accel_params_from_json(pathlib.Path("example.json")),
        ],
    )
    print(
        json.dumps(
            [asdict(result) for result in st.execute()],
            indent=4,
            sort_keys=True,
        )
    )
