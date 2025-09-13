import pathlib

from src.params import (
    AccelParamModule,
    SeriesParamModule,
    AccelParamModule,
)
from src.trial import ComplexTrial
from src.events import TrialEventScanner
from src.export import ExportTrialResults, ExportTrialEvents

from src.plot import InteractiveConvergencePlot

import pyshanks

if __name__ == "__main__":
    st = ComplexTrial(
        [
            SeriesParamModule(
                pyshanks.CosSeries,
                x=[20, 100],
            ),
        ],
        [AccelParamModule(pyshanks.LevinAlgorithm, range(1, 5), range(1, 5))],
    )
    results = st.execute()
    result_exporter = ExportTrialResults(results)

    result_exporter.to_json(pathlib.Path("output/output.json"))
    result_exporter.to_csv(pathlib.Path("output/output.csv"))

    scanner = TrialEventScanner(results)
    events = scanner.execute()

    event_exporter = ExportTrialEvents(events)
    event_exporter.to_json(pathlib.Path("output/events.json"))
    event_exporter.to_csv(pathlib.Path("output/events.csv"))

    # InteractiveConvergencePlot(results).show()
