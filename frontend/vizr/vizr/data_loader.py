"""
Author: Sobolev Y. A.
Description: Data loading and processing module for visualization, handling Parquet files and Polars dataframes.
"""

import glob
import os
from dataclasses import dataclass, field
from typing import Any, Dict, List, Optional, Set, Tuple

import numpy as np
import polars as pl

from .symlog import Scientific


@dataclass
class ComplexNumber:
    """
    Represents a complex number with arbitrary precision parts.
    We separate the mantissa and exponent to avoid floating point overflow/underflow
    during intermediate calculations.
    """

    real: Scientific
    imag: Scientific

    def format(self) -> str:
        """
        Formats the complex number into a pretty string.

        Outputs:
            str: "real + imagj" or just "real" if imag is zero.
        """
        real_str = self.real.format()
        # only show the imaginary part if it's significant (non-zero mantissa).
        if abs(self.imag.mantissa) > 0.0:
            imag_str = self.imag.format()
            return f"{real_str} + {imag_str}j"
        return real_str

    @classmethod
    def from_polars_struct(cls, s: Optional[Dict[str, Any]]) -> "ComplexNumber":
        """
        Constructs a ComplexNumber from a dictionary struct, typically from Polars.
        The Parquet schema stores these as structs with fields '0' (mantissa) and '1' (exponent).

        Inputs:
            s (Dict[str, Any]): A dictionary with 'real' and 'imag' keys, each having '0' (float) and '1' (int) for mantissa/exponent.

        Outputs:
            ComplexNumber: The constructed complex number.
        """
        if s is None:
            return cls(Scientific(0.0, 0), Scientific(0.0, 0))

        r_data = s.get("real")
        if r_data:
            real = Scientific(float(r_data.get("0", 0.0)), int(r_data.get("1", 0)))
        else:
            real = Scientific(0.0, 0)

        i_data = s.get("imag")
        if i_data:
            imag = Scientific(float(i_data.get("0", 0.0)), int(i_data.get("1", 0)))
        else:
            imag = Scientific(0.0, 0)

        return cls(real, imag)


@dataclass
class ErrorInfo:
    """
    Holds information about errors encountered during calculation.
    Typically used to mark points where an acceleration algorithm failed (e.g., division by zero).
    """

    n: int
    message: str


@dataclass
class EventInfo:
    """
    Represents significant events during the series processing.
    For example, when an algorithm switches strategies or detects convergence.
    """

    n: int
    name: str
    description: str


@dataclass
class AccelInfo:
    """
    Metadata about the acceleration method used.
    """

    name: str
    m_value: int
    additional_args: Dict[str, str]
    noise_str: str = ""
    noise_info: Optional[Dict[str, Any]] = None


@dataclass
class FilteredMethod:
    """
    Data from a filter applied to a divergent segment.
    """

    name: str
    val_real_m: np.ndarray
    val_real_e: np.ndarray
    val_imag_m: np.ndarray
    val_imag_e: np.ndarray


@dataclass
class FilteredData:
    """
    Container for filtered segments.
    """

    start_n: int
    methods: List[FilteredMethod]


@dataclass
class AccelRecord:
    """
    Record containing all the data for a specific acceleration run.
    Stores arrays of values (real/imag parts, deviations) aligned by index.
    """

    accel_info: AccelInfo
    # arrays for computed values (dense)
    # storing these as separate arrays is more efficient for plotting than a list of objects.
    val_real_m: np.ndarray
    val_real_e: np.ndarray
    val_imag_m: np.ndarray
    val_imag_e: np.ndarray
    dev_m: np.ndarray
    dev_e: np.ndarray
    # mask for valid entries (since some might be None/Null if the calc failed at step n)
    valid_mask: np.ndarray

    profiling: Dict[str, np.ndarray]

    errors: List[ErrorInfo]
    events: List[EventInfo]

    filtered: Optional[FilteredData] = None


@dataclass
class SeriesRecord:
    """
    Contains the base series data and its computed values.
    This is the "ground truth" or baseline that accelerations are trying to improve upon.
    """

    precision: str
    series_id: int
    name: str
    arguments: Dict[str, str]
    series_limit: ComplexNumber
    # arrays for computed values
    n: np.ndarray
    val_real_m: np.ndarray
    val_real_e: np.ndarray
    val_imag_m: np.ndarray
    val_imag_e: np.ndarray
    dev_m: np.ndarray
    dev_e: np.ndarray


@dataclass
class Metadata:
    """
    Summary of available data in the dataset. Used for populating UI filters.
    We compute this once on startup so the UI knows what dropdown options to show.
    """

    precisions: List[str]
    series_names: List[str]
    accel_names: List[str]
    m_values: List[int]
    noise_options: List[str]
    accel_param_info: Dict[str, List[str]]
    series_param_info: Dict[str, List[str]]


class DataLoader:
    """
    Handles loading and filtering of Parquet data using Polars.
    We use Polars because it's extremely fast at filtering large datasets and supports
    lazy evaluation, which saves memory.
    """

    def __init__(self, data_dir: str):
        """
        Initializes the DataLoader.

        Inputs:
            data_dir (str): The directory containing 'series' and 'accelerations' subdirectories,
                           or a path to a JSON file.
        """
        self.data_dir = data_dir
        if os.path.isfile(data_dir) and data_dir.endswith(".json"):
            self._load_json([data_dir])
        elif os.path.isdir(data_dir):
            if os.path.exists(os.path.join(data_dir, "series")):
                # lazy scan allowing us to build a query plan before executing it.
                # hive_partitioning=True allows us to leverage folder structures like /series_name=Harmonic/
                self.series_df = pl.scan_parquet(
                    os.path.join(data_dir, "series"), hive_partitioning=True
                )
                self.accel_df = pl.scan_parquet(
                    os.path.join(data_dir, "accelerations"), hive_partitioning=True
                )
            else:
                # check for JSON files
                json_files = glob.glob(os.path.join(data_dir, "results*.json"))
                if json_files:
                    self._load_json(json_files)
                else:
                    raise ValueError(f"No compatible data found in {data_dir}")
        else:
            raise ValueError(f"Invalid data path: {data_dir}")

        self.metadata = self._compute_metadata()

    def _parse_dynamic_dict(self, data: Any) -> Dict[str, str]:
        """Parses dynamic arguments which are stored as Structs in Parquet/JSON."""
        if not data:
            return {}
        if isinstance(data, dict):
            return {str(k): str(v) for k, v in data.items() if v is not None and k != "__dummy__"}
        return {}

    def _infer_precision(self, path: str) -> str:
        """Infers precision from filename (e.g., results_FLong.json -> FLong)."""
        filename = os.path.basename(path)
        name_without_ext = os.path.splitext(filename)[0]
        parts = name_without_ext.split("_")
        if len(parts) > 1:
            # assumes format results_PRECISION
            return parts[-1]
        return "f64"  # default

    @staticmethod
    def _format_noise_dict(ni: Dict[str, Any]) -> str:
        """Compresses noise dictionary into a readable string."""
        if not ni:
            return "None"
        method = ni.get("method", "Unknown")
        params = []
        # sort keys to ensure consistent string representation
        for k in sorted(ni.keys()):
            v = ni[k]
            if k == "method" or v is None:
                continue
            params.append(f"{k}={v}")
        if not params:
            return method
        return f"{method}({', '.join(params)})"

    def _load_json(self, json_paths: List[str]):
        """
        Loads data from JSON files and transforms it to match the expected schema.
        We attempt to make the JSON data look like the Parquet-based LazyFrames.
        """
        dfs = []
        for path in json_paths:
            try:
                # we use pl.read_json which is eager, then convert to lazy.
                df = pl.read_json(path, infer_schema_length=None)
                precision = self._infer_precision(path)

                # 1. series dataframe
                # each entry in JSON is (series + accel), so we group by series to get the unique base series.
                series_df = (
                    df.select(
                        [
                            pl.lit(precision).alias("precision"),
                            pl.col("series").struct.field("id").alias("series_id"),
                            pl.col("series").struct.field("name").alias("series_name"),
                            pl.col("series")
                            .struct.field("arguments")
                            .alias("arguments"),
                            # series_limit as a struct of strings (real, imag)
                            pl.col("series").struct.field("lim").alias("series_limit"),
                            # computed as list of struct {n, value: {real, imag}, deviation}
                            pl.col("computed")
                            .list.eval(
                                pl.struct(
                                    [
                                        pl.element().struct.field("n"),
                                        pl.element()
                                        .struct.field("partial_sum")
                                        .alias("value"),
                                        pl.element()
                                        .struct.field("partial_sum_deviation")
                                        .alias("deviation"),
                                        pl.element()
                                        .struct.field("profiling")
                                        .alias("profiling"),
                                    ]
                                )
                            )
                            .alias("computed"),
                        ]
                    )
                    .sort(pl.col("computed").list.len(), descending=True)
                    .unique(subset=["series_id"])
                )

                # 2. accel dataframe
                def extract_events(computed_list):
                    evts = []
                    for step in computed_list:
                        n = step.get("n")
                        for e in step.get("events", []):
                            evts.append(
                                {
                                    "n": n,
                                    "name": e.get("name", ""),
                                    "description": e.get("description", ""),
                                }
                            )
                    return evts

                # handle filtered if present
                # it comes as a struct in JSON

                accel_df = df.select(
                    [
                        pl.col("series").struct.field("id").alias("series_id"),
                        pl.col("accel").struct.field("name").alias("accel_name"),
                        pl.col("accel").struct.field("m_value").alias("m_value"),
                        pl.col("accel")
                        .struct.field("additional_args")
                        .alias("additional_args"),
                        pl.col("noise").alias("noise"),
                        pl.col("noise")
                        .map_elements(
                            self._format_noise_dict,
                            return_dtype=pl.String,
                            skip_nulls=False,
                        )
                        .alias("noise_str"),
                        # computed for accel (aligned with series steps)
                        pl.col("computed")
                        .list.eval(
                            pl.struct(
                                [
                                    pl.element()
                                    .struct.field("accel_value")
                                    .alias("value"),
                                    pl.element()
                                    .struct.field("accel_value_deviation")
                                    .alias("deviation"),
                                    pl.element()
                                    .struct.field("profiling")
                                    .alias("profiling"),
                                ]
                            )
                        )
                        .alias("computed"),
                        # map events from steps
                        pl.col("computed")
                        .map_elements(
                            extract_events,
                            return_dtype=pl.List(
                                pl.Struct(
                                    [
                                        pl.Field("n", pl.Int64),
                                        pl.Field("name", pl.String),
                                        pl.Field("description", pl.String),
                                    ]
                                )
                            ),
                        )
                        .alias("events"),
                        # global error if any
                        pl.col("error")
                        .map_elements(
                            lambda x: [{"n": 0, "message": str(x)}]
                            if x is not None
                            else [],
                            return_dtype=pl.List(
                                pl.Struct(
                                    [
                                        pl.Field("n", pl.Int64),
                                        pl.Field("message", pl.String),
                                    ]
                                )
                            ),
                        )
                        .alias("errors"),
                        pl.col("filtered").alias("filtered"),
                    ]
                )

                dfs.append((series_df, accel_df))

            except Exception as e:
                print(f"Error loading {path}: {e}")

        if not dfs:
            # initialize empty lazy frames if nothing loaded
            self.series_df = pl.DataFrame().lazy()
            self.accel_df = pl.DataFrame().lazy()
            return

        # concat all series DFs and Accel DFs
        all_series = [d[0] for d in dfs]
        all_accels = [d[1] for d in dfs]

        self.series_df = (
            pl.concat(all_series, how="diagonal")
            .sort(pl.col("computed").list.len(), descending=True)
            .unique(subset=["series_id"])
            .lazy()
        )
        self.accel_df = pl.concat(all_accels, how="diagonal").lazy()

    def _compute_metadata(self) -> Metadata:
        """
        Scans the dataset to extract available filtering options.
        We perform distinct value queries on the columns to populate the UI filters.

        Outputs:
            Metadata: Object containing lists of available series, accelerations, etc.
        """
        # we need to collect some data to get unique values.
        # the 'collect()' call triggers the actual file reading.
        precisions = (
            self.series_df.select("precision")
            .unique()
            .collect()
            .get_column("precision")
            .drop_nulls()
            .sort()
            .to_list()
        )
        series_names = (
            self.series_df.select("series_name")
            .unique()
            .collect()
            .get_column("series_name")
            .drop_nulls()
            .sort()
            .to_list()
        )
        accel_names = (
            self.accel_df.select("accel_name")
            .unique()
            .collect()
            .get_column("accel_name")
            .drop_nulls()
            .sort()
            .to_list()
        )
        m_values = (
            self.accel_df.select("m_value")
            .unique()
            .collect()
            .get_column("m_value")
            .drop_nulls()
            .sort()
            .to_list()
        )

        noise_options = []
        if "noise_str" in self.accel_df.collect_schema():
            noise_options = (
                self.accel_df.select("noise_str")
                .unique()
                .collect()
                .get_column("noise_str")
                .drop_nulls()
                .sort()
                .to_list()
            )
        elif "noise" in self.accel_df.collect_schema():
            # if we have 'noise' struct but not 'noise_str' (e.g. from Parquet)
            noise_options = (
                self.accel_df.select(
                    pl.col("noise")
                    .map_elements(
                        self._format_noise_dict,
                        return_dtype=pl.String,
                        skip_nulls=False,
                    )
                    .alias("noise_str")
                )
                .unique()
                .collect()
                .get_column("noise_str")
                .drop_nulls()
                .sort()
                .to_list()
            )

        # for nested parameters (like arguments in a JSON struct), we need a helper.
        series_param_info = self._get_unique_param_info(self.series_df, "arguments")
        accel_param_info = self._get_unique_param_info(self.accel_df, "additional_args")

        return Metadata(
            precisions=precisions,
            series_names=series_names,
            accel_names=accel_names,
            m_values=m_values,
            noise_options=noise_options,
            accel_param_info=accel_param_info,
            series_param_info=series_param_info,
        )

    def _get_unique_param_info(
        self, df: pl.LazyFrame, column: str
    ) -> Dict[str, List[str]]:
        """
        Helper to extract unique values from nested struct fields.
        Used because 'arguments' and 'additional_args' are stored as Structs in Parquet.

        Inputs:
            df (pl.LazyFrame): The LazyFrame to query.
            column (str): The name of the struct column.

        Outputs:
            Dict[str, List[str]]: A dictionary mapping field names to lists of unique values (as strings).
        """
        res = {}
        # get the schema of the struct column to know what fields exist inside it.
        schema = df.collect_schema()[column]
        if isinstance(schema, pl.Struct):
            for field in schema.fields:
                field_name = field.name
                # query unique values for this specific field inside the struct.
                unique_vals = (
                    df.select(pl.col(column).struct.field(field_name))
                    .unique()
                    .collect()
                    .get_column(field_name)
                    .drop_nulls()
                    .sort()
                    .to_list()
                )
                if unique_vals:
                    res[field_name] = [str(v) for v in unique_vals]
        return res

    def _parse_sci_expr(
        self, col_expr: pl.Expr, default_val: Optional[float] = None
    ) -> Tuple[pl.Expr, pl.Expr]:
        """
        Creates a Polars expression to parse scientific notation strings.
        This handles cases where numbers are stored as strings (e.g. "1.23e-100").

        Inputs:
            col_expr (pl.Expr): The string column expression to parse.
            default_val (Optional[float]): Value to use if mantissa is null/missing.

        Outputs:
            Tuple[pl.Expr, pl.Expr]: A tuple of (mantissa_expr, exponent_expr).
        """
        # regex to split mantissa and exponent (if present)
        # matches: start, capture mantissa (no e/E), optional group (e/E, capture exponent), end
        # we use extract with group index.

        # mantissa is everything up to 'e' or 'E' or end
        m_str = col_expr.str.extract(r"^([^eE]+)", 1)
        # exponent is everything after 'e' or 'E'
        e_str = col_expr.str.extract(r"[eE](.+)$", 1)

        m = m_str.cast(pl.Float64)
        if default_val is not None:
            m = m.fill_null(default_val)

        e = e_str.cast(pl.Int32).fill_null(0)
        return m, e

    def _parse_filtered(self, raw_filtered: Any) -> Optional[FilteredData]:
        """
        Parses the filtered dictionary/struct from Arrow/Parquet/JSON.
        """
        if not raw_filtered:
            return None

        start_n = raw_filtered.get("start_n")
        if start_n is None:
            return None

        methods_raw = raw_filtered.get("methods")
        if not methods_raw:
            return None

        parsed_methods = []

        if not isinstance(methods_raw, dict):
            return None

        for m_name, m_data in methods_raw.items():
            if m_name == "__dummy__" or not isinstance(m_data, dict):
                continue
            if not isinstance(m_data, dict):
                continue
            values = m_data.get("values", [])
            if not values:
                continue

            # arrays to store components
            v_r_m = np.zeros(len(values), dtype=np.float64)
            v_r_e = np.zeros(len(values), dtype=np.int32)
            v_i_m = np.zeros(len(values), dtype=np.float64)
            v_i_e = np.zeros(len(values), dtype=np.int32)

            for i, val in enumerate(values):
                # val is {real: ..., imag: ...}
                r = self._parse_scientific(val.get("real"))
                im = self._parse_scientific(val.get("imag"))

                v_r_m[i] = r.mantissa
                v_r_e[i] = r.exponent
                v_i_m[i] = im.mantissa
                v_i_e[i] = im.exponent

            parsed_methods.append(
                FilteredMethod(
                    name=m_name,
                    val_real_m=v_r_m,
                    val_real_e=v_r_e,
                    val_imag_m=v_i_m,
                    val_imag_e=v_i_e,
                )
            )

        if not parsed_methods:
            return None

        return FilteredData(start_n=start_n, methods=parsed_methods)

    def filter_data(
        self, filters: Dict[str, Any]
    ) -> List[Tuple[SeriesRecord, List[AccelRecord]]]:
        """
        Filters the dataset based on provided criteria and returns structured records.
        This is the main workhorse that executes the query and materializes the data for the UI.

        Inputs:
            filters (Dict[str, Any]): Dictionary of filter keys and allowed values.

        Outputs:
            List[Tuple[SeriesRecord, List[AccelRecord]]]: A list of tuples, each containing a SeriesRecord and its associated AccelRecords.
        """
        import time

        t0 = time.time()

        # start building the query for the base series table.
        s_df = self.series_df
        if filters.get("precisions"):
            s_df = s_df.filter(pl.col("precision").is_in(list(filters["precisions"])))
        if filters.get("base_series"):
            s_df = s_df.filter(
                pl.col("series_name").is_in(list(filters["base_series"]))
            )

        for param, values in filters.get("series_params", {}).items():
            if values:
                s_df = s_df.filter(
                    pl.col("arguments").struct.field(param).is_in(list(values))
                )

        # define expression to transform computed list.
        # the data on disk stores 'computed' as a list of structs.
        # we want to flatten these structs into parallel arrays (columns) for faster plotting logic later.

        def transform_computed(df_to_check: pl.LazyFrame, col_name="computed", has_n=True):
            # get the schema of the list element to see if profiling exists
            list_dtype = df_to_check.collect_schema()[col_name]
            inner_fields = []
            if isinstance(list_dtype, pl.List):
                inner_dtype = list_dtype.inner
                if isinstance(inner_dtype, pl.Struct):
                    inner_fields = [f.name for f in inner_dtype.fields]

            root = pl.element()
            val = root.struct.field("value")
            dev_str = root.struct.field("deviation")
            real_str = val.struct.field("real")
            imag_str = val.struct.field("imag")

            # parse the string representations into numbers.
            vr_m, vr_e = self._parse_sci_expr(real_str, default_val=0.0)
            vi_m, vi_e = self._parse_sci_expr(imag_str, default_val=0.0)
            d_m, d_e = self._parse_sci_expr(dev_str, default_val=None)

            struct_fields = {
                "vr_m": vr_m,
                "vr_e": vr_e,
                "vi_m": vi_m,
                "vi_e": vi_e,
                "d_m": d_m,
                "d_e": d_e,
            }
            if "profiling" in inner_fields:
                struct_fields["prof"] = root.struct.field("profiling")
            else:
                struct_fields["prof"] = pl.lit(None)

            if has_n:
                struct_fields["n"] = root.struct.field("n")

            return pl.col(col_name).list.eval(pl.struct(**struct_fields))

        t1 = time.time()

        # execute the query and bring data into memory.
        # use to_dicts() to avoid Arrow serialization issues with nested structs.
        series_collected = s_df.collect()
        series_rows = series_collected.with_columns(
            transform_computed(s_df, "computed", has_n=True).alias("computed_parsed")
        ).to_dicts()

        t2 = time.time()
        print(f"[Timing] Series collect: {t2 - t1:.3f}s (rows: {len(series_rows)})")

        if not series_rows:
            return []

        # get the Series IDs so we can fetch only the relevant accelerations.
        series_ids = [row["series_id"] for row in series_rows]

        # start building the query for the accelerations table.
        a_df = self.accel_df.filter(pl.col("series_id").is_in(series_ids))
        if filters.get("base_accel"):
            a_df = a_df.filter(pl.col("accel_name").is_in(list(filters["base_accel"])))
        if filters.get("m_values"):
            a_df = a_df.filter(pl.col("m_value").is_in(list(filters["m_values"])))
        if filters.get("noise_options"):
            if "noise_str" in self.accel_df.collect_schema():
                a_df = a_df.filter(
                    pl.col("noise_str").is_in(list(filters["noise_options"]))
                )
            elif "noise" in self.accel_df.collect_schema():
                a_df = a_df.filter(
                    pl.col("noise")
                    .map_elements(
                        self._format_noise_dict,
                        return_dtype=pl.String,
                        skip_nulls=False,
                    )
                    .is_in(list(filters["noise_options"]))
                )

        for param, values in filters.get("accel_params", {}).items():
            if values:
                a_df = a_df.filter(
                    pl.col("additional_args").struct.field(param).is_in(list(values))
                )

        t3 = time.time()
        accel_collected = a_df.collect()
        accel_rows = accel_collected.with_columns(
            transform_computed(a_df, "computed", has_n=False).alias("computed_parsed")
        ).to_dicts()

        t4 = time.time()
        print(f"[Timing] Accel collect: {t4 - t3:.3f}s (rows: {len(accel_rows)})")

        # group accelerations by series_id locally.
        # we need to associate each acceleration run with its parent series.
        accels_by_series: Dict[int, List[AccelRecord]] = {}

        # process accel table.
        t_start_proc_accel = time.time()
        for row in accel_rows:
            sid = row["series_id"]
            name = row["accel_name"]
            m_val = row["m_value"]

            # args (dict)
            args = self._parse_dynamic_dict(row.get("additional_args"))

            noise_info = row.get("noise")
            noise_str = row.get("noise_str")
            if noise_str is None:
                noise_str = (
                    self._format_noise_dict(noise_info) if noise_info else "None"
                )

            # computed
            computed_parsed = row.get("computed_parsed") or []
            n_points = len(computed_parsed)

            val_real_m = np.zeros(n_points, dtype=np.float64)
            val_real_e = np.zeros(n_points, dtype=np.int32)
            val_imag_m = np.zeros(n_points, dtype=np.float64)
            val_imag_e = np.zeros(n_points, dtype=np.int32)
            dev_m = np.zeros(n_points, dtype=np.float64)
            dev_e = np.zeros(n_points, dtype=np.int32)
            valid_mask = np.ones(n_points, dtype=bool)

            prof_add = np.zeros(n_points, dtype=np.int64)
            prof_mul = np.zeros(n_points, dtype=np.int64)
            prof_div = np.zeros(n_points, dtype=np.int64)
            prof_special = np.zeros(n_points, dtype=np.int64)

            for i, p in enumerate(computed_parsed):
                if p is None:
                    valid_mask[i] = False
                    continue
                val_real_m[i] = p.get("vr_m", 0.0)
                val_real_e[i] = p.get("vr_e", 0)
                val_imag_m[i] = p.get("vi_m", 0.0)
                val_imag_e[i] = p.get("vi_e", 0)

                dm = p.get("d_m")
                if dm is None:
                    dev_m[i] = 0.0
                    dev_e[i] = 0
                else:
                    dev_m[i] = dm
                    dev_e[i] = p.get("d_e", 0)

                prof = p.get("prof")
                if prof:
                    prof_add[i] = prof.get("add", 0)
                    prof_mul[i] = prof.get("mul", 0)
                    prof_div[i] = prof.get("div", 0)
                    prof_special[i] = prof.get("special", 0)

            # errors & events
            err_list = row.get("errors") or []
            evt_list = row.get("events") or []

            errors = [
                ErrorInfo(n=int(e.get("n", 0)), message=e.get("message", ""))
                for e in err_list
                if e
            ]
            events = [
                EventInfo(
                    n=int(e.get("n", 0)),
                    name=e.get("name", ""),
                    description=e.get("description", ""),
                )
                for e in evt_list
                if e
            ]

            # filtered
            filtered_obj = self._parse_filtered(row.get("filtered"))

            accel_rec = AccelRecord(
                accel_info=AccelInfo(
                    name, m_val, args, noise_str=noise_str, noise_info=noise_info
                ),
                val_real_m=val_real_m,
                val_real_e=val_real_e,
                val_imag_m=val_imag_m,
                val_imag_e=val_imag_e,
                dev_m=dev_m,
                dev_e=dev_e,
                valid_mask=valid_mask,
                profiling={
                    "add": prof_add,
                    "mul": prof_mul,
                    "div": prof_div,
                    "special": prof_special,
                },
                errors=errors,
                events=events,
                filtered=filtered_obj,
            )
            if sid not in accels_by_series:
                accels_by_series[sid] = []
            accels_by_series[sid].append(accel_rec)

        print(
            f"[Timing] Accel processing loop: {time.time() - t_start_proc_accel:.3f}s"
        )

        # process series table.
        t_start_proc_series = time.time()
        result: List[Tuple[SeriesRecord, List[AccelRecord]]] = []

        for row in series_rows:
            sid = row["series_id"]

            # computed
            computed_parsed = row.get("computed_parsed") or []
            n_points = len(computed_parsed)

            n_vals = np.zeros(n_points, dtype=np.int64)
            val_real_m = np.zeros(n_points, dtype=np.float64)
            val_real_e = np.zeros(n_points, dtype=np.int32)
            val_imag_m = np.zeros(n_points, dtype=np.float64)
            val_imag_e = np.zeros(n_points, dtype=np.int32)
            dev_m = np.zeros(n_points, dtype=np.float64)
            dev_e = np.zeros(n_points, dtype=np.int32)

            for i, p in enumerate(computed_parsed):
                if p is None:
                    continue
                n_vals[i] = p.get("n", 0)
                val_real_m[i] = p.get("vr_m", 0.0)
                val_real_e[i] = p.get("vr_e", 0)
                val_imag_m[i] = p.get("vi_m", 0.0)
                val_imag_e[i] = p.get("vi_e", 0)
                dev_m[i] = p.get("d_m", 0.0)
                dev_e[i] = p.get("d_e", 0)

            # limit
            series_limit = self._parse_complex(row.get("series_limit"))

            series_rec = SeriesRecord(
                precision=row["precision"],
                series_id=sid,
                name=row["series_name"],
                arguments=self._parse_dynamic_dict(row.get("arguments")),
                series_limit=series_limit,
                n=n_vals,
                val_real_m=val_real_m,
                val_real_e=val_real_e,
                val_imag_m=val_imag_m,
                val_imag_e=val_imag_e,
                dev_m=dev_m,
                dev_e=dev_e,
            )

            accels = accels_by_series.get(sid, [])
            result.append((series_rec, accels))

        print(
            f"[Timing] Series processing loop: {time.time() - t_start_proc_series:.3f}s"
        )
        return result

    def _parse_scientific(self, s: Any) -> Scientific:
        """
        Parses a scientific notation value (string, dict, or None) into a Scientific object.

        Inputs:
            s (Any): Input value (string, dict with '0'/'1', or None).

        Outputs:
            Scientific: Parsed object.
        """
        if s is None:
            return Scientific(0.0, 0)
        if isinstance(s, str):
            return Scientific.from_str(s)
        if isinstance(s, dict) and "0" in s and "1" in s:
            return Scientific(float(s["0"]), int(s["1"]))
        # fallback for unexpected formats
        return Scientific(0.0, 0)

    def _parse_complex(self, s: Optional[Dict[str, Any]]) -> ComplexNumber:
        """
        Parses a dictionary representing a complex number into a ComplexNumber object.

        Inputs:
            s (Dict[str, Any]): Dictionary with 'real' and 'imag' keys.

        Outputs:
            ComplexNumber: Parsed complex number.
        """
        if s is None:
            return ComplexNumber(Scientific(0.0, 0), Scientific(0.0, 0))
        return ComplexNumber(
            real=self._parse_scientific(s.get("real")),
            imag=self._parse_scientific(s.get("imag")),
        )
