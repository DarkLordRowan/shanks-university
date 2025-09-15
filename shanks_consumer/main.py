import argparse
import pathlib
import sys

from src.params import (
    get_accel_params_from_json,
    get_series_params_from_json,
    get_series_params_from_csv,
)
from src.trial import ComplexTrial
from src.export import ExportTrialResults, ExportTrialEvents
from src.events import TrialEventScanner
from src.plot import save_all_plots

ARB = True

def main():
    parser = argparse.ArgumentParser(description="Complex Trial Analysis Tool")

    parser.add_argument(
        "--series-json",
        type=pathlib.Path,
        default=pathlib.Path("data/example.json"),
    )
    parser.add_argument(
        "--series-csv",
        type=pathlib.Path,
        default=pathlib.Path("data/example_series.csv"),
    )
    parser.add_argument(
        "--accel-json",
        type=pathlib.Path,
        default=pathlib.Path("data/example.json"),
    )

    parser.add_argument(
        "--output-dir", type=pathlib.Path, default=pathlib.Path("output")
    )
    parser.add_argument(
        "--plots-dir", type=pathlib.Path, default=pathlib.Path("plots")
    )
    parser.add_argument("--results-json", type=pathlib.Path, default=None)
    parser.add_argument("--results-csv", type=pathlib.Path, default=None)
    parser.add_argument("--events-json", type=pathlib.Path, default=None)
    parser.add_argument("--events-csv", type=pathlib.Path, default=None)

    parser.add_argument("--no-events", action="store_true")
    parser.add_argument("--no-plots", action="store_true")
    parser.add_argument("--verbose", "-v", action="count", default=0)

    args = parser.parse_args()

    if args.results_json is None:
        args.results_json = args.output_dir / "output.json"
    if args.results_csv is None:
        args.results_csv = args.output_dir / "output.csv"
    if args.events_json is None:
        args.events_json = args.output_dir / "events.json"
    if args.events_csv is None:
        args.events_csv = args.output_dir / "events.csv"

    args.output_dir.mkdir(parents=True, exist_ok=True)
    args.plots_dir.mkdir(parents=True, exist_ok=True)

    if args.verbose >= 1:
        print(f"Series JSON: {args.series_json}")
        print(f"Series CSV: {args.series_csv}")
        print(f"Acceleration JSON: {args.accel_json}")
        print(f"Output directory: {args.output_dir}")
        print(f"Plots directory: {args.plots_dir}")

    try:
        series_params = []
        if args.series_json.exists():
            series_params.extend(get_series_params_from_json(args.series_json, ARB))
        else:
            print(f"Warning: Series JSON file not found: {args.series_json}")

        if args.series_csv.exists():
            series_params.extend(get_series_params_from_csv(args.series_csv, ARB))
        else:
            print(f"Warning: Series CSV file not found: {args.series_csv}")

        accel_params = []
        if args.accel_json.exists():
            accel_params.extend(get_accel_params_from_json(args.accel_json, ARB))
        else:
            print(
                f"Warning: Acceleration JSON file not found: {args.accel_json}"
            )

        if not series_params:
            print("Error: No series parameters found!")
            sys.exit(1)

        if args.verbose >= 1:
            print(f"Loaded {len(series_params)} series parameters")
            print(f"Loaded {len(accel_params)} acceleration parameters")
            print("Executing complex trial...")

        st = ComplexTrial(series_params, accel_params)
        results = st.execute()

        results_exporter = ExportTrialResults(results)
        results_exporter.to_json(args.results_json)
        results_exporter.to_csv(args.results_csv)

        if args.verbose >= 1:
            print(
                f"Results exported to: {args.results_json}, {args.results_csv}"
            )

        if not args.no_plots:
            if args.verbose >= 1:
                print("Saving plots...")
            save_all_plots(results, save_dir=args.plots_dir)
            if args.verbose >= 1:
                print(f"Plots saved to: {args.plots_dir}")
        else:
            if args.verbose >= 1:
                print("Skipping plots as requested")

        if not args.no_events:
            if args.verbose >= 1:
                print("Scanning for events...")

            scanner = TrialEventScanner(results)
            events = scanner.execute()

            events_exporter = ExportTrialEvents(events)
            events_exporter.to_json(args.events_json)
            events_exporter.to_csv(args.events_csv)

            if args.verbose >= 1:
                print(
                    f"Events exported to: {args.events_json}, {args.events_csv}"
                )
        else:
            if args.verbose >= 1:
                print("Skipping event scanning as requested")

        if args.verbose >= 1:
            print("Processing completed successfully!")

    except FileNotFoundError as e:
        print(f"Error: File not found - {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Error during execution: {e}")
        if args.verbose >= 2:
            import traceback

            traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
