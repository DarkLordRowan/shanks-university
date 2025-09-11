import pathlib

from src.params import (
    AccelParamModule,
    SeriesParamModule,
    get_series_params_from_csv,
    get_series_params_from_json,
    get_accel_params_from_json,
)
from src.trial import ComplexTrial


import pyshanks

if __name__ == "__main__":
    st = ComplexTrial(
        [
            SeriesParamModule(pyshanks.CosSeries, [9]),
            *get_series_params_from_csv(pathlib.Path("example_series.csv")),
            *get_series_params_from_json(pathlib.Path("example.json")),
        ],
        [
            AccelParamModule(pyshanks.ShanksAlgorithm, range(10, 30), [4, 5]),
            *get_accel_params_from_json(pathlib.Path("example.json")),
        ],
    )
    st.execute()
    print(*st.results, sep="\n")
