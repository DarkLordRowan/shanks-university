import pathlib

from src.export import ExportTrialResults
from src.params import get_series_params_from_json, get_accel_params_from_json
from src.trial import ComplexTrial

def proc(json_file):
    st = ComplexTrial(
        [
            *get_series_params_from_json(json_file),
        ],
        [
            *get_accel_params_from_json(json_file),
        ],
    )
    results = st.execute()
    exporter = ExportTrialResults(results)
    return exporter.to_json()


def main():
    jsonfile = pathlib.Path("example.json")
    jsonNe = proc(jsonfile)
    print(jsonNe)


if __name__ == "__main__":
    main()
