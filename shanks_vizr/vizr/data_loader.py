"""
Author: Sobolev Y. A.
Description: Data loading and processing module for visualization, handling Parquet files and Polars dataframes.
"""

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
        # Only show the imaginary part if it's significant (non-zero mantissa).
        if abs(self.imag.mantissa) > 0.0:
            imag_str = self.imag.format()
            return f"{real_str} + {imag_str}j"
        return real_str

    @classmethod
    def from_polars_struct(cls, s: Dict[str, Any]) -> "ComplexNumber":
        """
        Constructs a ComplexNumber from a dictionary struct, typically from Polars.
        The Parquet schema stores these as structs with fields '0' (mantissa) and '1' (exponent).

        Inputs:
            s (Dict[str, Any]): A dictionary with 'real' and 'imag' keys, each having '0' (float) and '1' (int) for mantissa/exponent.

        Outputs:
            ComplexNumber: The constructed complex number.
        """
        real = Scientific(float(s["real"]["0"]), int(s["real"]["1"]))
        imag = Scientific(float(s["imag"]["0"]), int(s["imag"]["1"]))
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


@dataclass
class AccelRecord:
    """
    Record containing all the data for a specific acceleration run.
    Stores arrays of values (real/imag parts, deviations) aligned by index.
    """

    accel_info: AccelInfo
    # Arrays for computed values (dense)
    # Storing these as separate arrays is more efficient for plotting than a list of objects.
    val_real_m: np.ndarray
    val_real_e: np.ndarray
    val_imag_m: np.ndarray
    val_imag_e: np.ndarray
    dev_m: np.ndarray
    dev_e: np.ndarray
    # Mask for valid entries (since some might be None/Null if the calc failed at step n)
    valid_mask: np.ndarray

    errors: List[ErrorInfo]
    events: List[EventInfo]


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
    # Arrays for computed values
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
            data_dir (str): The directory containing 'series' and 'accelerations' subdirectories.
        """
        self.data_dir = data_dir
        # lazy scan allowing us to build a query plan before executing it.
        # hive_partitioning=True allows us to leverage folder structures like /series_name=Harmonic/
        self.series_df = pl.scan_parquet(
            os.path.join(data_dir, "series"), hive_partitioning=True
        )
        self.accel_df = pl.scan_parquet(
            os.path.join(data_dir, "accelerations"), hive_partitioning=True
        )
        self.metadata = self._compute_metadata()

    def _compute_metadata(self) -> Metadata:
        """
        Scans the dataset to extract available filtering options.
        We perform distinct value queries on the columns to populate the UI filters.

        Outputs:
            Metadata: Object containing lists of available series, accelerations, etc.
        """
        # We need to collect some data to get unique values.
        # The 'collect()' call triggers the actual file reading.
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

        # For nested parameters (like arguments in a JSON struct), we need a helper.
        series_param_info = self._get_unique_param_info(self.series_df, "arguments")
        accel_param_info = self._get_unique_param_info(self.accel_df, "additional_args")

        return Metadata(
            precisions=precisions,
            series_names=series_names,
            accel_names=accel_names,
            m_values=m_values,
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
        # Get the schema of the struct column to know what fields exist inside it.
        schema = df.collect_schema()[column]
        if isinstance(schema, pl.Struct):
            for field in schema.fields:
                field_name = field.name
                # Query unique values for this specific field inside the struct.
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
        # Returns (mantissa_expr, exponent_expr)
        # Regex to split mantissa and exponent (if present)
        # Matches: start, capture mantissa (no e/E), optional group (e/E, capture exponent), end
        # We use extract with group index.

        # Mantissa is everything up to 'e' or 'E' or end
        m_str = col_expr.str.extract(r"^([^eE]+)", 1)
        # Exponent is everything after 'e' or 'E'
        e_str = col_expr.str.extract(r"[eE](.+)$", 1)

        m = m_str.cast(pl.Float64)
        if default_val is not None:
            m = m.fill_null(default_val)

        e = e_str.cast(pl.Int32).fill_null(0)
        return m, e

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

        # Start building the query for the base series table.
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

        # Define expression to transform computed list.
        # The data on disk stores 'computed' as a list of structs.
        # We want to flatten these structs into parallel arrays (columns) for faster plotting logic later.

        def transform_computed(col_name="computed", has_n=True):
            root = pl.element()
            val = root.struct.field("value")
            dev_str = root.struct.field("deviation")
            real_str = val.struct.field("real")
            imag_str = val.struct.field("imag")

            # Parse the string representations into numbers.
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
            if has_n:
                struct_fields["n"] = root.struct.field("n")

            return pl.col(col_name).list.eval(pl.struct(**struct_fields))

        # Apply the transformation query.
        s_transformed = s_df.with_columns(
            transform_computed("computed", has_n=True).alias("computed_parsed")
        )

        t1 = time.time()
        print(f"[Timing] Series query construction: {t1 - t0:.3f}s")

        # Execute the query and bring data into memory as an Arrow table.
        # Arrow is efficient for transfer to Numpy.
        series_table = s_transformed.collect().to_arrow()
        t2 = time.time()
        print(
            f"[Timing] Series collect to arrow: {t2 - t1:.3f}s (rows: {series_table.num_rows})"
        )

        if series_table.num_rows == 0:
            return []

        # Get the Series IDs so we can fetch only the relevant accelerations.
        series_ids_col = series_table.column("series_id")
        series_ids = series_ids_col.to_pylist()  # List[int]

        # Start building the query for the accelerations table.
        a_df = self.accel_df.filter(pl.col("series_id").is_in(series_ids))
        if filters.get("base_accel"):
            a_df = a_df.filter(pl.col("accel_name").is_in(list(filters["base_accel"])))
        if filters.get("m_values"):
            a_df = a_df.filter(pl.col("m_value").is_in(list(filters["m_values"])))

        for param, values in filters.get("accel_params", {}).items():
            if values:
                a_df = a_df.filter(
                    pl.col("additional_args").struct.field(param).is_in(list(values))
                )

        # Same transformation for accel computed values (without 'n', as accels usually map to the series 'n' implied).
        # Actually, accels do not always have 'n' explicitly in the same way, or it's implied by index.
        # Correction: The rust code output 'n' for accels too if they are sparse, but here we assume dense or aligned?
        # Re-checking the transform_computed: it uses has_n=False.
        a_transformed = a_df.with_columns(
            transform_computed("computed", has_n=False).alias("computed_parsed")
        )

        t3 = time.time()
        accel_table = a_transformed.collect().to_arrow()
        t4 = time.time()
        print(
            f"[Timing] Accel collect to arrow: {t4 - t3:.3f}s (rows: {accel_table.num_rows})"
        )

        # Group accelerations by series_id locally.
        # We need to associate each acceleration run with its parent series.
        accels_by_series: Dict[int, List[AccelRecord]] = {}

        # Process Accel Table.
        # We iterate through batches to utilize Arrow's speed and avoid heavy Python object creation overhead where possible.
        t_start_proc_accel = time.time()
        if accel_table.num_rows > 0:
            batches = accel_table.to_batches()
            for batch in batches:
                b_sid = batch.column("series_id")
                b_name = batch.column("accel_name")
                b_m = batch.column("m_value")
                b_args = batch.column("additional_args")
                b_err = batch.column("errors")
                b_evt = batch.column("events")

                b_comp = batch.column("computed_parsed")  # ListArray

                # OPTIMIZATION: Extract numpy arrays once per batch.
                # The 'computed_parsed' column is a ListArray. We get its values (the flat array) and offsets.
                # This allows us to slice it using numpy instead of iterating row-by-row in Python.
                offsets = b_comp.offsets.to_numpy()  # int32 array of offsets
                child_struct = b_comp.values  # StructArray

                child_arrays = {}
                for field_name in ["vr_m", "vr_e", "vi_m", "vi_e", "d_m", "d_e"]:
                    child_arrays[field_name] = child_struct.field(field_name).to_numpy(
                        zero_copy_only=False
                    )

                # Validity mask for the child structs (points).
                # This tells us if a specific point is valid or null (e.g. calculation error).
                child_validity = child_struct.is_valid().to_numpy(zero_copy_only=False)

                for i in range(batch.num_rows):
                    sid = b_sid[i].as_py()
                    name = b_name[i].as_py()
                    m_val = b_m[i].as_py()

                    # Args (StructScalar to dict)
                    args_scalar = b_args[i]
                    args = {
                        k: str(v)
                        for k, v in args_scalar.as_py().items()
                        if v is not None
                    }

                    # Computed slice
                    start = offsets[i]
                    end = offsets[i + 1]

                    # valid_mask for this slice
                    valid_mask = child_validity[start:end]

                    # Errors & Events
                    err_list = b_err[i].as_py()
                    evt_list = b_evt[i].as_py()

                    errors = [
                        ErrorInfo(n=int(e.get("n", 0)), message=e.get("message", ""))
                        for e in (err_list or [])
                        if e
                    ]
                    events = [
                        EventInfo(
                            n=int(e.get("n", 0)),
                            name=e.get("name", ""),
                            description=e.get("description", ""),
                        )
                        for e in (evt_list or [])
                        if e
                    ]

                    accel_rec = AccelRecord(
                        accel_info=AccelInfo(name, m_val, args),
                        val_real_m=child_arrays["vr_m"][start:end],
                        val_real_e=child_arrays["vr_e"][start:end],
                        val_imag_m=child_arrays["vi_m"][start:end],
                        val_imag_e=child_arrays["vi_e"][start:end],
                        dev_m=child_arrays["d_m"][start:end],
                        dev_e=child_arrays["d_e"][start:end],
                        valid_mask=valid_mask,
                        errors=errors,
                        events=events,
                    )
                    if sid not in accels_by_series:
                        accels_by_series[sid] = []
                    accels_by_series[sid].append(accel_rec)

        print(
            f"[Timing] Accel processing loop: {time.time() - t_start_proc_accel:.3f}s"
        )

        # Process Series Table.
        # Similar logic to Accel table, but constructing SeriesRecords.
        t_start_proc_series = time.time()
        result: List[Tuple[SeriesRecord, List[AccelRecord]]] = []
        batches = series_table.num_rows > 0 and series_table.to_batches() or []
        for batch in batches:
            b_prec = batch.column("precision")
            b_sid = batch.column("series_id")
            b_name = batch.column("series_name")
            b_args = batch.column("arguments")
            b_limit = batch.column("series_limit")

            b_comp = batch.column("computed_parsed")

            # OPTIMIZATION: Extract numpy arrays once per batch
            offsets = b_comp.offsets.to_numpy()
            child_struct = b_comp.values

            child_arrays = {}
            for field_name in ["vr_m", "vr_e", "vi_m", "vi_e", "d_m", "d_e"]:
                child_arrays[field_name] = child_struct.field(field_name).to_numpy(
                    zero_copy_only=False
                )

            # Series has 'n', unlike Accel which relies on alignment (in this schema version).
            has_n = "n" in child_struct.type.names
            if has_n:
                child_arrays["n"] = child_struct.field("n").to_numpy(
                    zero_copy_only=False
                )
            else:
                child_arrays["n"] = np.array([], dtype=np.int64)

            for i in range(batch.num_rows):
                sid = b_sid[i].as_py()

                # Computed slice
                start = offsets[i]
                end = offsets[i + 1]

                # Limit
                limit_dict = b_limit[i].as_py()
                series_limit = self._parse_complex(limit_dict)

                series_rec = SeriesRecord(
                    precision=b_prec[i].as_py(),
                    series_id=sid,
                    name=b_name[i].as_py(),
                    arguments={
                        k: str(v) for k, v in b_args[i].as_py().items() if v is not None
                    },
                    series_limit=series_limit,
                    n=child_arrays["n"][start:end],
                    val_real_m=child_arrays["vr_m"][start:end],
                    val_real_e=child_arrays["vr_e"][start:end],
                    val_imag_m=child_arrays["vi_m"][start:end],
                    val_imag_e=child_arrays["vi_e"][start:end],
                    dev_m=child_arrays["d_m"][start:end],
                    dev_e=child_arrays["d_e"][start:end],
                )

                accels = accels_by_series.get(sid, [])
                result.append((series_rec, accels))

        print(
            f"[Timing] Series processing loop: {time.time() - t_start_proc_series:.3f}s"
        )
        return result

    def _parse_scientific(self, s: Any) -> Scientific:
        """
        Parses a scientific notation value (string or existing Scientific) into a Scientific object.

        Inputs:
            s (Any): Input value (string, Scientific, or None).

        Outputs:
            Scientific: Parsed object.
        """
        if s is None:
            return Scientific(0.0, 0)
        if isinstance(s, str):
            return Scientific.from_str(s)
        # Fallback for unexpected formats
        return Scientific(0.0, 0)

    def _parse_complex(self, s: Dict[str, Any]) -> ComplexNumber:
        """
        Parses a dictionary representing a complex number into a ComplexNumber object.

        Inputs:
            s (Dict[str, Any]): Dictionary with 'real' and 'imag' keys.

        Outputs:
            ComplexNumber: Parsed complex number.
        """
        return ComplexNumber(
            real=self._parse_scientific(s["real"]),
            imag=self._parse_scientific(s["imag"]),
        )
