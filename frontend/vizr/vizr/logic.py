"""
Author: Sobolev Y. A.
Description: Pure functional logic for filtering and data preparation.
Separates the 'business logic' from the Qt UI.
"""

from dataclasses import dataclass, field
from typing import Any, Callable, Dict, List, Optional, Set, Tuple

import numpy as np
from PyQt6.QtCore import Qt
from PyQt6.QtGui import (
    QColor,  # needed for color generation logic, though it's UI-related
)

# we import the data types
from .data_loader import AccelRecord, FilteredMethod, SeriesRecord
from .symlog import vectorized_approx_f64, vectorized_symlog

# --- data structures for visualization ---


@dataclass
class CurveData:
    """Data required to draw a single curve."""

    x: np.ndarray
    y: np.ndarray
    pen: Any  # pg.mkPen result or config dict
    name: str
    segments: Optional[List[Tuple[int, int, str]]] = None


@dataclass
class ScatterData:
    """Data for scatter plots."""

    x: List[float]
    y: List[float]
    brushes: List[Any]
    names: List[str]


@dataclass
class TableRowData:
    """Data for a single row in the results table."""

    series_id: str
    name: str
    precision: str
    limit_str: str
    series_params: str
    accel_name: str
    m_value: str
    accel_params: str
    noise_str: str

    # content for collapsible widgets
    series_values: List[str]
    accel_values: List[str]
    deviations: List[str]
    errors: List[str]
    events: List[str]

    # summary for deviation
    dev_summary: str


@dataclass
class LegendItemData:
    """Data for a custom legend item."""

    text: str
    color: QColor
    is_dashed: bool = False


@dataclass
class VisualizationData:
    """
    The complete set of data ready to be rendered by the UI.
    This object is created by a pure function.
    """

    convergence_curves: List[CurveData] = field(default_factory=list)
    error_curves: List[CurveData] = field(default_factory=list)
    profiling_curves: List[CurveData] = field(default_factory=list)
    performance_data: Optional[ScatterData] = None
    table_rows: List[TableRowData] = field(default_factory=list)
    legend_items: List[LegendItemData] = field(default_factory=list)


# --- filtering logic ---


def _match_params(actual: Dict[str, str], filters: Dict[str, Set[str]]) -> bool:
    """Checks if actual parameters match the filter requirements."""
    for param, allowed in filters.items():
        if allowed:
            val = actual.get(param)
            if val is None or val not in allowed:
                return False
    return True


def filter_dataset(
    data: List[Tuple[SeriesRecord, List[AccelRecord]]], filters: Dict[str, Any]
) -> List[Tuple[SeriesRecord, List[AccelRecord]]]:
    """
    Pure function to filter the dataset.

    Inputs:
        data: The full dataset.
        filters: Dictionary of active filters.

    Outputs:
        Filtered dataset.
    """
    # quick exit if no filters (assuming empty sets mean "All")
    has_filters = (
        bool(filters["precisions"])
        or bool(filters["base_series"])
        or bool(filters["base_accel"])
        or bool(filters["m_values"])
        or bool(filters.get("noise_options"))
        or any(filters["series_params"].values())
        or any(filters["accel_params"].values())
    )

    if not has_filters:
        return data

    filtered_result = []

    # extract filter sets for faster lookup
    f_precisions = filters["precisions"]
    f_base_series = filters["base_series"]
    f_series_params = filters["series_params"]

    f_base_accel = filters["base_accel"]
    f_m_values = filters["m_values"]
    f_noise = filters.get("noise_options")
    f_accel_params = filters["accel_params"]

    has_accel_filters = (
        bool(f_base_accel)
        or bool(f_m_values)
        or bool(f_noise)
        or any(f_accel_params.values())
    )

    for series, accels in data:
        # series level checks
        if f_precisions and series.precision not in f_precisions:
            continue
        if f_base_series and series.name not in f_base_series:
            continue
        if not _match_params(series.arguments, f_series_params):
            continue

        if not has_accel_filters:
            filtered_result.append((series, accels))
            continue

        # accel level checks
        valid_accels = []
        for accel in accels:
            if f_base_accel and accel.accel_info.name not in f_base_accel:
                continue
            if f_m_values and accel.accel_info.m_value not in f_m_values:
                continue
            if f_noise and accel.accel_info.noise_str not in f_noise:
                continue
            if not _match_params(accel.accel_info.additional_args, f_accel_params):
                continue

            valid_accels.append(accel)

        if valid_accels:
            filtered_result.append((series, valid_accels))

    return filtered_result


# --- visualization preparation logic ---


@dataclass
class ViewOptions:
    show_partial_sums: bool = True
    show_limits: bool = True
    show_real: bool = True
    show_imaginary: bool = True
    force_show_imaginary: bool = False
    show_filters: bool = True
    symlog: bool = False
    show_prof_add: bool = False
    show_prof_mul: bool = False
    show_prof_div: bool = False
    show_prof_special: bool = False


def format_args(args: Dict[str, str]) -> str:
    if not args:
        return ""
    items = [f"{k}={v}" for k, v in args.items()]
    return "{" + ", ".join(items) + "}"


class LabelOptimizer:
    """
    Analyzes visible data to determine which metadata varies.
    Allows creating labels that only show 'interesting' differences.
    """

    def __init__(self, data: List[Tuple[SeriesRecord, List[AccelRecord]]]):
        self.series_names: Set[str] = set()
        self.precisions: Set[str] = set()
        self.series_args: Dict[str, Set[str]] = {}

        self.accel_names: Set[str] = set()
        self.m_values: Set[int] = set()
        self.accel_args: Dict[str, Set[str]] = {}
        self.noise_options: Set[str] = set()

        for series, accels in data:
            self.series_names.add(series.name)
            self.precisions.add(series.precision)
            for k, v in series.arguments.items():
                if k not in self.series_args:
                    self.series_args[k] = set()
                self.series_args[k].add(v)

            for accel in accels:
                self.accel_names.add(accel.accel_info.name)
                self.m_values.add(accel.accel_info.m_value)
                self.noise_options.add(accel.accel_info.noise_str)
                for k, v in accel.accel_info.additional_args.items():
                    if k not in self.accel_args:
                        self.accel_args[k] = set()
                    self.accel_args[k].add(v)

    def get_series_label(self, series: SeriesRecord) -> str:
        parts = []
        if len(self.series_names) > 1:
            parts.append(series.name)
        if len(self.precisions) > 1:
            parts.append(series.precision)

        arg_parts = []
        for k, v in series.arguments.items():
            if k in self.series_args and len(self.series_args[k]) > 1:
                arg_parts.append(f"{k}={v}")
        if arg_parts:
            parts.append("{" + ", ".join(arg_parts) + "}")

        return " ".join(parts) if parts else series.name

    def get_accel_label(self, accel: AccelRecord) -> str:
        parts = []
        if len(self.accel_names) > 1:
            parts.append(accel.accel_info.name)
        if len(self.m_values) > 1:
            parts.append(f"m={accel.accel_info.m_value}")
        if len(self.noise_options) > 1:
            if accel.accel_info.noise_str != "None":
                parts.append(f"[Noise: {accel.accel_info.noise_str}]")

        arg_parts = []
        for k, v in accel.accel_info.additional_args.items():
            if k in self.accel_args and len(self.accel_args[k]) > 1:
                arg_parts.append(f"{k}={v}")
        if arg_parts:
            parts.append("{" + ", ".join(arg_parts) + "}")

        # fallback if everything is the same but we need a label
        if not parts:
            return f"{accel.accel_info.name}(m={accel.accel_info.m_value})"

        return " ".join(parts)


def prepare_viz_data(
    data: List[Tuple[SeriesRecord, List[AccelRecord]]],
    options: ViewOptions,
    max_table_rows: int = 100,
) -> VisualizationData:
    """
    Transforms filtered data into visualization primitives.
    This function contains NO PyQt widget calls, only data manipulation.
    """
    import pyqtgraph as pg  # we use pg.mkPen/mkBrush, which are data factories essentially.

    viz_data = VisualizationData()
    optimizer = LabelOptimizer(data)

    # colors (constants)
    COLOR_PARTIAL_REAL = QColor(128, 128, 128)
    COLOR_PARTIAL_IMAG = QColor(255, 192, 203)
    COLOR_LIMIT_REAL = QColor(255, 0, 0)
    COLOR_LIMIT_IMAG = QColor(255, 100, 100)
    COLOR_ACCEL_IMAG = QColor(255, 165, 0)
    COLOR_FILTER_REAL = QColor(0, 128, 0)
    COLOR_FILTER_IMAG = QColor(0, 100, 0)

    # Profiling colors
    COLOR_PROF = {
        "add": QColor(0, 255, 0),       # Green
        "mul": QColor(0, 0, 255),       # Blue
        "div": QColor(255, 0, 0),       # Red
        "special": QColor(255, 165, 0), # Orange
    }

    n_series = len(data)

    perf_x = []
    perf_y = []
    perf_names = []
    perf_brushes = []

    rows_added = 0

    for i, (series, accels) in enumerate(data):
        series_color = pg.intColor(i, n_series)
        series_params_str = format_args(series.arguments)

        # optimized label for this series
        series_label = optimizer.get_series_label(series)

        n_vals = series.n

        # add series to legend
        viz_data.legend_items.append(
            LegendItemData(text=series_label, color=series_color)
        )

        # --- 1. partial sums ---
        if options.show_partial_sums:
            if options.show_real:
                real_vals = vectorized_approx_f64(series.val_real_m, series.val_real_e)
                viz_data.convergence_curves.append(
                    CurveData(
                        x=n_vals,
                        y=real_vals,
                        pen=pg.mkPen(COLOR_PARTIAL_REAL, width=2),
                        name=f"{series_label} (Partial)",
                    )
                )

                # error
                if options.symlog:
                    series_dev = vectorized_symlog(series.dev_m, series.dev_e)
                else:
                    series_dev = vectorized_approx_f64(series.dev_m, series.dev_e)

                viz_data.error_curves.append(
                    CurveData(
                        x=n_vals,
                        y=series_dev,
                        pen=pg.mkPen(QColor(255, 0, 0), width=3),
                        name=f"{series_label} (Partial Error)",
                    )
                )

            if options.show_imaginary:
                is_zero_imag = np.all(series.val_imag_m == 0.0)
                if options.force_show_imaginary or not is_zero_imag:
                    imag_vals = vectorized_approx_f64(
                        series.val_imag_m, series.val_imag_e
                    )
                    viz_data.convergence_curves.append(
                        CurveData(
                            x=n_vals,
                            y=imag_vals,
                            pen=pg.mkPen(COLOR_PARTIAL_IMAG, width=2),
                            name=f"{series_label} (Partial Imag)",
                        )
                    )

        # --- 2. limits ---
        if options.show_limits and len(n_vals) > 0:
            limit = series.series_limit
            min_x, max_x = float(n_vals[0]), float(n_vals[-1])

            if options.show_real:
                real_limit = limit.real.approx_f64()
                viz_data.convergence_curves.append(
                    CurveData(
                        x=np.array([min_x, max_x]),
                        y=np.array([real_limit, real_limit]),
                        pen=pg.mkPen(COLOR_LIMIT_REAL, width=3),
                        name=f"{series_label} (Limit)",
                    )
                )

            if options.show_imaginary:
                imag_limit = limit.imag.approx_f64()
                is_zero_imag = imag_limit == 0.0
                if options.force_show_imaginary or not is_zero_imag:
                    viz_data.convergence_curves.append(
                        CurveData(
                            x=np.array([min_x, max_x]),
                            y=np.array([imag_limit, imag_limit]),
                            pen=pg.mkPen(COLOR_LIMIT_IMAG, width=2),
                            name=f"{series_label} (Limit Imag)",
                        )
                    )

        # --- 3. accelerations (batched) ---

        # we accumulate segments for batch plotting
        batch_real_n = []
        batch_real_y = []
        batch_real_err = []
        batch_real_segs = []
        curr_off_real = 0

        batch_imag_n = []
        batch_imag_y = []
        batch_imag_segs = []
        curr_off_imag = 0

        # filters
        batch_filt_n = []
        batch_filt_y = []
        batch_filt_segs = []
        curr_off_filt = 0

        batch_filt_imag_n = []
        batch_filt_imag_y = []
        batch_filt_imag_segs = []
        curr_off_filt_imag = 0

        batch_filt_err_n = []
        batch_filt_err_y = []

        # profiling batches
        batch_prof_n = {cat: [] for cat in COLOR_PROF}
        batch_prof_y = {cat: [] for cat in COLOR_PROF}
        batch_prof_segs = {cat: [] for cat in COLOR_PROF}
        curr_off_prof = {cat: 0 for cat in COLOR_PROF}

        for accel in accels:
            accel_params_str = format_args(accel.accel_info.additional_args)

            # optimized label for this accel
            accel_label_diff = optimizer.get_accel_label(accel)
            # full tooltip label
            accel_tooltip = f"{series_label} -> {accel_label_diff}"


            if options.show_real:
                viz_data.legend_items.append(
                    LegendItemData(
                        text=f"  -> {accel_label_diff}",
                        color=series_color,
                        is_dashed=True,
                    )
                )

            mask = accel.valid_mask
            if not np.any(mask):
                continue

            min_len = min(len(n_vals), len(mask))
            current_n = n_vals[:min_len].astype(float)
            current_mask = mask[:min_len]

            # Profiling curves (accumulate batches)
            for cat, color in COLOR_PROF.items():
                if cat in accel.profiling:
                    # Check if this category should be shown
                    if cat == "add" and not options.show_prof_add: continue
                    if cat == "mul" and not options.show_prof_mul: continue
                    if cat == "div" and not options.show_prof_div: continue
                    if cat == "special" and not options.show_prof_special: continue

                    prof_vals = accel.profiling[cat][:min_len].astype(float)
                    # Use the same mask logic as real/imag
                    prof_vals[~current_mask] = np.nan

                    seg_len = len(current_n)
                    batch_prof_segs[cat].append(
                        (curr_off_prof[cat], curr_off_prof[cat] + seg_len, f"{accel_tooltip} ({cat})")
                    )
                    curr_off_prof[cat] += seg_len + 1

                    batch_prof_n[cat].append(current_n)
                    batch_prof_n[cat].append(np.array([np.nan]))
                    batch_prof_y[cat].append(prof_vals)
                    batch_prof_y[cat].append(np.array([np.nan]))

            # --- real accel ---
            if options.show_real:
                r_m = accel.val_real_m[:min_len]
                r_e = accel.val_real_e[:min_len]
                accel_real = vectorized_approx_f64(r_m, r_e)
                accel_real[~current_mask] = np.nan

                d_m = accel.dev_m[:min_len]
                d_e = accel.dev_e[:min_len]
                accel_dev_real = vectorized_approx_f64(
                    d_m, d_e
                )  # keep raw for table/perf

                if options.symlog:
                    accel_dev = vectorized_symlog(d_m, d_e)
                else:
                    accel_dev = accel_dev_real.copy()
                accel_dev[~current_mask] = np.nan

                # append to batch
                seg_len = len(current_n)
                batch_real_segs.append(
                    (curr_off_real, curr_off_real + seg_len, accel_tooltip)
                )
                curr_off_real += seg_len + 1  # +1 for NaN

                batch_real_n.append(current_n)
                batch_real_n.append(np.array([np.nan]))

                batch_real_y.append(accel_real)
                batch_real_y.append(np.array([np.nan]))

                batch_real_err.append(accel_dev)
                batch_real_err.append(np.array([np.nan]))

                # performance point
                valid_dev_indices = np.where(~np.isnan(accel_dev))[0]
                if len(valid_dev_indices) > 0:
                    # find min error index
                    local_min_idx = np.argmin(accel_dev[valid_dev_indices])
                    min_idx = valid_dev_indices[local_min_idx]

                    perf_x.append(current_n[min_idx])
                    perf_y.append(accel_dev[min_idx])
                    perf_names.append(accel_tooltip)
                    perf_brushes.append(pg.mkBrush(series_color))

                # table data
                if rows_added < max_table_rows:
                    # stats
                    valid_real_indices = np.where(
                        ~np.isnan(accel_dev_real) & current_mask
                    )[0]
                    if len(valid_real_indices) > 0:
                        v = accel_dev_real[valid_real_indices]
                        dev_summary = f"Mean: {np.mean(v):.2e}, Min: {np.min(v):.2e}"
                    else:
                        dev_summary = "No valid data"

                    # collapsible content generators
                    def make_val_list(vals, ns):
                        return [
                            f"n={int(n)}: {v:.6e}"
                            for v, n in zip(vals, ns)
                            if not np.isnan(v)
                        ]

                    viz_data.table_rows.append(
                        TableRowData(
                            series_id=str(series.series_id),
                            name=series.name,
                            precision=series.precision,
                            limit_str=series.series_limit.format(),
                            series_params=series_params_str,
                            accel_name=accel.accel_info.name,
                            m_value=str(accel.accel_info.m_value),
                            accel_params=accel_params_str,
                            noise_str=accel.accel_info.noise_str,
                            series_values=[
                                f"n={series.n[k]}: {series.val_real_m[k] * (10.0 ** series.val_real_e[k]):.6e}"
                                + (
                                    f" + {series.val_imag_m[k] * (10.0 ** series.val_imag_e[k]):.6e}j"
                                    if series.val_imag_m[k] != 0
                                    else ""
                                )
                                for k in range(len(series.n))
                            ],
                            accel_values=make_val_list(accel_real, current_n),
                            deviations=make_val_list(
                                accel_dev_real, current_n
                            ),  # use real deviation for text
                            errors=[f"n={e.n}: {e.message}" for e in accel.errors],
                            events=[
                                f"n={e.n}: {e.name} - {e.description}"
                                for e in accel.events
                            ],
                            dev_summary=dev_summary,
                        )
                    )
                    rows_added += 1

            # --- imag accel ---
            if options.show_imaginary:
                v_im_m = accel.val_imag_m[:min_len]
                v_im_e = accel.val_imag_e[:min_len]
                is_zero_imag = np.all(v_im_m == 0.0)

                if options.force_show_imaginary or not is_zero_imag:
                    accel_imag = vectorized_approx_f64(v_im_m, v_im_e)
                    accel_imag[~current_mask] = np.nan

                    seg_len = len(current_n)
                    batch_imag_segs.append(
                        (
                            curr_off_imag,
                            curr_off_imag + seg_len,
                            f"{accel_tooltip} (Imag)",
                        )
                    )
                    curr_off_imag += seg_len + 1

                    batch_imag_n.append(current_n)
                    batch_imag_n.append(np.array([np.nan]))

                    batch_imag_y.append(accel_imag)
                    batch_imag_y.append(np.array([np.nan]))

            # --- filters ---
            if options.show_filters and accel.filtered:
                for method in accel.filtered.methods:
                    f_len = len(method.val_real_m)
                    f_n = np.arange(
                        accel.filtered.start_n,
                        accel.filtered.start_n + f_len,
                        dtype=float,
                    )
                    filter_tooltip = f"{accel_tooltip} [Filter: {method.name}]"

                    if options.show_real:
                        f_real = vectorized_approx_f64(
                            method.val_real_m, method.val_real_e
                        )

                        batch_filt_segs.append(
                            (curr_off_filt, curr_off_filt + f_len, filter_tooltip)
                        )
                        curr_off_filt += f_len + 1

                        batch_filt_n.append(f_n)
                        batch_filt_n.append(np.array([np.nan]))
                        batch_filt_y.append(f_real)
                        batch_filt_y.append(np.array([np.nan]))

                        # filter error
                        lim_r = series.series_limit.real.approx_f64()
                        lim_i = series.series_limit.imag.approx_f64()
                        f_imag_vals = vectorized_approx_f64(
                            method.val_imag_m, method.val_imag_e
                        )

                        f_diff_r = f_real - lim_r
                        f_diff_i = f_imag_vals - lim_i
                        f_dev_linear = np.sqrt(f_diff_r**2 + f_diff_i**2)

                        if options.symlog:
                            # simple approx for symlog of float array since we don't have separate m/e for deviation here easily
                            # we can re-use vectorized_symlog by faking exponent=0
                            f_dev = vectorized_symlog(
                                f_dev_linear,
                                np.zeros_like(f_dev_linear, dtype=np.int32),
                            )
                        else:
                            f_dev = f_dev_linear

                        batch_filt_err_n.append(f_n)
                        batch_filt_err_n.append(np.array([np.nan]))
                        batch_filt_err_y.append(f_dev)
                        batch_filt_err_y.append(np.array([np.nan]))

                    if options.show_imaginary:
                        is_zero_imag = np.all(method.val_imag_m == 0.0)
                        if options.force_show_imaginary or not is_zero_imag:
                            f_imag = vectorized_approx_f64(
                                method.val_imag_m, method.val_imag_e
                            )

                            batch_filt_imag_segs.append(
                                (
                                    curr_off_filt_imag,
                                    curr_off_filt_imag + f_len,
                                    f"{filter_tooltip} (Imag)",
                                )
                            )
                            curr_off_filt_imag += f_len + 1

                            batch_filt_imag_n.append(f_n)
                            batch_filt_imag_n.append(np.array([np.nan]))
                            batch_filt_imag_y.append(f_imag)
                            batch_filt_imag_y.append(np.array([np.nan]))

        # --- finalize batches for this series ---

        # real accel
        if batch_real_n:
            all_n = np.concatenate(batch_real_n)

            # convergence
            viz_data.convergence_curves.append(
                CurveData(
                    x=all_n,
                    y=np.concatenate(batch_real_y),
                    pen=pg.mkPen(
                        series_color, width=1, style=Qt.PenStyle.DashLine, cosmetic=True
                    ),
                    name=f"{series_label} (accels)",
                    segments=batch_real_segs,
                )
            )

            # error
            viz_data.error_curves.append(
                CurveData(
                    x=all_n,
                    y=np.concatenate(batch_real_err),
                    pen=pg.mkPen(
                        series_color, width=1, style=Qt.PenStyle.DashLine, cosmetic=True
                    ),
                    name=f"{series_label} (accels)",
                    segments=batch_real_segs,
                )
            )

        # imag accel
        if batch_imag_n:
            viz_data.convergence_curves.append(
                CurveData(
                    x=np.concatenate(batch_imag_n),
                    y=np.concatenate(batch_imag_y),
                    pen=pg.mkPen(
                        COLOR_ACCEL_IMAG,
                        width=1,
                        style=Qt.PenStyle.DashLine,
                        cosmetic=True,
                    ),
                    name=f"{series_label} (accels imag)",
                    segments=batch_imag_segs,
                )
            )

        # filters real
        if batch_filt_n:
            viz_data.convergence_curves.append(
                CurveData(
                    x=np.concatenate(batch_filt_n),
                    y=np.concatenate(batch_filt_y),
                    pen=pg.mkPen(
                        COLOR_FILTER_REAL,
                        width=2,
                        style=Qt.PenStyle.DotLine,
                        cosmetic=True,
                    ),
                    name=f"{series_label} (filters)",
                    segments=batch_filt_segs,
                )
            )

        # filters error
        if batch_filt_err_n:
            viz_data.error_curves.append(
                CurveData(
                    x=np.concatenate(batch_filt_err_n),
                    y=np.concatenate(batch_filt_err_y),
                    pen=pg.mkPen(
                        COLOR_FILTER_REAL,
                        width=2,
                        style=Qt.PenStyle.DotLine,
                        cosmetic=True,
                    ),
                    name=f"{series_label} (filters error)",
                    segments=batch_filt_segs,  # re-use real segments for tooltip
                )
            )

        # filters imag
        if batch_filt_imag_n:
            viz_data.convergence_curves.append(
                CurveData(
                    x=np.concatenate(batch_filt_imag_n),
                    y=np.concatenate(batch_filt_imag_y),
                    pen=pg.mkPen(
                        COLOR_FILTER_IMAG,
                        width=2,
                        style=Qt.PenStyle.DotLine,
                        cosmetic=True,
                    ),
                    name=f"{series_label} (filters imag)",
                    segments=batch_filt_imag_segs,
                )
            )

        # finalize profiling batches for this series
        for cat, color in COLOR_PROF.items():
            if batch_prof_n[cat]:
                viz_data.profiling_curves.append(
                    CurveData(
                        x=np.concatenate(batch_prof_n[cat]),
                        y=np.concatenate(batch_prof_y[cat]),
                        pen=pg.mkPen(color, width=1, style=Qt.PenStyle.SolidLine),
                        name=f"{series_label} ({cat})",
                        segments=batch_prof_segs[cat],
                    )
                )

    # wrap up performance data
    if perf_x:
        viz_data.performance_data = ScatterData(
            x=perf_x, y=perf_y, brushes=perf_brushes, names=perf_names
        )

    return viz_data


def extract_filter_options(
    data: List[Tuple[SeriesRecord, List[AccelRecord]]],
) -> Dict[str, Any]:
    """
    Analyzes loaded data and returns available filter options.
    Only includes options with >1 unique values.
    """
    precisions: Set[str] = set()
    base_series: Set[str] = set()
    base_accel: Set[str] = set()
    m_values: Set[int] = set()
    noise_options: Set[str] = set()
    series_params: Dict[str, Set[str]] = {}
    accel_params: Dict[str, Set[str]] = {}

    for series, accels in data:
        precisions.add(series.precision)
        base_series.add(series.name)

        # series arguments
        for k, v in series.arguments.items():
            if k not in series_params:
                series_params[k] = set()
            series_params[k].add(v)

        for accel in accels:
            base_accel.add(accel.accel_info.name)
            m_values.add(accel.accel_info.m_value)
            if accel.accel_info.noise_str:
                noise_options.add(accel.accel_info.noise_str)

            # acceleration arguments
            for k, v in accel.accel_info.additional_args.items():
                if k not in accel_params:
                    accel_params[k] = set()
                accel_params[k].add(v)

    # clean up params that only have 1 option (useless to filter on).
    def clean_params(params: Dict[str, Set[str]]) -> Dict[str, List[str]]:
        return {k: sorted(list(v)) for k, v in params.items() if len(v) > 1}

    return {
        "precisions": sorted(list(precisions)) if len(precisions) > 1 else [],
        "base_series": sorted(list(base_series)) if len(base_series) > 1 else [],
        "base_accel": sorted(list(base_accel)) if len(base_accel) > 1 else [],
        "m_values": sorted(list(m_values)) if len(m_values) > 1 else [],
        "noise_options": sorted(list(noise_options)) if len(noise_options) > 1 else [],
        "series_params": clean_params(series_params),
        "accel_params": clean_params(accel_params),
    }
