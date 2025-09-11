import pathlib

from src.params import (
    AccelParamModule,
    SeriesParamModule,
    get_series_params_from_csv,
    get_series_params_from_json,
    get_accel_params_from_json,
    all_accel,
    demo_all_synthetic_series,
)
from src.trial import Trial, ComplexTrial
from dataclasses import asdict
import json


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
    print(*st.execute(), sep="\n")

    st = Trial(
        SeriesParamModule(pyshanks.CosSeries, [9]),
        AccelParamModule(
            pyshanks.ShanksAlgorithm, range(10, 30), [4, 5]
        ),
    )
    print(*st.execute(), sep="\n")  # ? 18

    print("-- All series --")

    a = ComplexTrial(demo_all_synthetic_series([0.5]), [AccelParamModule(
        pyshanks.ShanksAlgorithm, range(10, 30), [4, 5]
    )])

    print(
        json.dumps(
            [asdict(result) for result in a.execute()],
            indent=4,
            sort_keys=True,
        )
    )

    print("--All accels --")

    b = ComplexTrial([SeriesParamModule(pyshanks.CosSeries, [9])], all_accel([30], 4))

    print(
        json.dumps(
            [asdict(result) for result in b.execute()],
            indent=4,
            sort_keys=True,
        )
    )
