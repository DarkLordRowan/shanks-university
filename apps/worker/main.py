import pathlib

from shanks_consumer.export import ExportTrialResults
from shanks_consumer.params import get_series_params_from_json, get_accel_params_from_json
from shanks_consumer.trial import ComplexTrial

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
