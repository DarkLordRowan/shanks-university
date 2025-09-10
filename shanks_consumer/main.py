import pathlib

from src.params import (
    AccelParamModule,
    SeriesParamModule,
    get_trial_params,
)
from src.trial import SimpleTrial, ComplexTrial
from dataclasses import asdict
import json


import pyshanks

if __name__ == "__main__":
    st = SimpleTrial(
        SeriesParamModule(pyshanks.CosSeries, [9]),
        AccelParamModule(
            pyshanks.ShanksTransform, list(range(10, 30)), [4, 5]
        ),
    )
    st.execute()
    print(*st.results, sep="\n")  # ? 18

    tp = get_trial_params(json_location=pathlib.Path("example.json"))

    if tp:
        st = ComplexTrial(tp)
        st.execute()
        print(
            json.dumps(
                [asdict(result) for result in st.results],
                indent=4,
                sort_keys=True,
            )
        )
