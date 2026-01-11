"""
Author: Sobolev Y. A.
Description: Main application entry point and UI implementation using PyQt6."
"""

import argparse
import sys
import time
from typing import Any, Dict, List, Optional, Set, Tuple

import numpy as np
import pyqtgraph as pg  # type: ignore
from PyQt6.QtCore import QPoint, QRect, QSize, Qt, QThread, pyqtSignal  # type: ignore
from PyQt6.QtGui import QBrush, QColor, QIcon, QPainter, QPen, QPixmap  # type: ignore
from PyQt6.QtWidgets import (  # type: ignore
    QApplication,
    QCheckBox,
    QFrame,
    QGridLayout,
    QGroupBox,
    QHBoxLayout,
    QHeaderView,
    QLabel,
    QLayout,
    QListWidget,
    QListWidgetItem,
    QMainWindow,
    QProgressBar,
    QPushButton,
    QScrollArea,
    QSizePolicy,
    QSplitter,
    QStyle,
    QTableWidget,
    QTableWidgetItem,
    QTextEdit,
    QToolButton,
    QVBoxLayout,
    QWidget,
)

from .data_loader import AccelRecord, DataLoader, Metadata, SeriesRecord
from .symlog import symlog_formatter, vectorized_approx_f64, vectorized_symlog


class CollapsibleCellWidget(QWidget):
    """
    A widget that can expand to show more content, used in the table for long lists.
    Keeps the table looking tidy until the user wants the details.

    This is necessary because inserting thousands of lines of text into a single
    table cell would make the UI sluggish or unusable.
    """

    def __init__(
        self,
        title: str,
        content_lines: List[str],
        parent_table: QTableWidget,
        row_idx: int,
    ):
        """
        Initializes the collapsible widget.

        Inputs:
            title (str): The text to display on the button.
            content_lines (List[str]): The lines of text to show when expanded.
            parent_table (QTableWidget): Reference to the parent table for resizing.
            row_idx (int): The row index this widget belongs to.
        """
        super().__init__()
        self.parent_table = parent_table
        self.row_idx = row_idx

        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(2, 2, 2, 2)
        self.layout.setSpacing(2)

        # Toggle Button
        # We use a tool button because it can have an icon and text.
        self.toggle_btn = QToolButton()
        self.toggle_btn.setText(f"> {title}")
        self.toggle_btn.setToolButtonStyle(Qt.ToolButtonStyle.ToolButtonTextBesideIcon)
        self.toggle_btn.setStyleSheet(
            "QToolButton { border: none; text-align: left; font-weight: bold; }"
        )
        self.toggle_btn.setCheckable(True)
        self.toggle_btn.setChecked(False)
        self.toggle_btn.clicked.connect(self.on_toggle)
        self.toggle_btn.setSizePolicy(
            QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed
        )
        self.layout.addWidget(self.toggle_btn)

        # Content Area
        # Hidden by default to save space.
        self.content_area = QTextEdit()
        self.content_area.setReadOnly(True)
        self.content_area.setText("\n".join(content_lines))
        self.content_area.setMaximumHeight(150)  # Cap height to avoid massive expansion
        self.content_area.setVisible(False)
        self.layout.addWidget(self.content_area)

    def on_toggle(self, checked: bool):
        """
        Handles the expansion/collapse logic.

        Inputs:
            checked (bool): The new state of the toggle button.
        """
        arrow = "v" if checked else ">"
        text = self.toggle_btn.text()[1:]  # Keep original title
        self.toggle_btn.setText(f"{arrow}{text}")
        self.content_area.setVisible(checked)

        # Trigger resize of the row so the content fits.
        self.parent_table.resizeRowToContents(self.row_idx)


def extract_filter_options(
    data: List[Tuple[SeriesRecord, List[AccelRecord]]],
) -> Dict[str, Any]:
    """
    Analyzes loaded data and returns available filter options.
    Only includes options with >1 unique values.

    This is used for the "Quick Filters" which operate in-memory on the data
    we have already downloaded/loaded, as opposed to the "Load Filters" which
    query the disk.

    Inputs:
        data (List[Tuple[SeriesRecord, List[AccelRecord]]]): The loaded dataset.

    Outputs:
        Dict[str, Any]: A dictionary of filter options.
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

        # Series Arguments
        for k, v in series.arguments.items():
            if k not in series_params:
                series_params[k] = set()
            series_params[k].add(v)

        for accel in accels:
            base_accel.add(accel.accel_info.name)
            m_values.add(accel.accel_info.m_value)
            if accel.accel_info.noise_str:
                noise_options.add(accel.accel_info.noise_str)

            # Acceleration Arguments
            for k, v in accel.accel_info.additional_args.items():
                if k not in accel_params:
                    accel_params[k] = set()
                accel_params[k].add(v)

    # Clean up params that only have 1 option (useless to filter on).
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


def filter_data_items(
    data: List[Tuple[SeriesRecord, List[AccelRecord]]], filters: Dict[str, Any]
) -> List[Tuple[SeriesRecord, List[AccelRecord]]]:
    """
    # Structure of filters:
    # {
    #     'precisions': set(),
    #     'base_series': set(),
    #     'base_accel': set(),
    #     'm_values': set(),
    #     'noise_options': set(),
    #     'series_params': {'param_name': set(), ...},
    #     'accel_params': {'param_name': set(), ...}
    # }
    # Empty set implies "All" (no filtering).

    # Inputs:
    #     data (List[Tuple[SeriesRecord, List[AccelRecord]]]): The dataset to filter.
    #     filters (Dict[str, Any]): Active filters.

    # Outputs:
    #     List[Tuple[SeriesRecord, List[AccelRecord]]]: Filtered dataset.
    # """
    # Quick check if any filter is active to avoid iterating if not needed.
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

    for series, accels in data:
        # --- Series Filtering ---
        # If the series itself doesn't match the criteria, we drop it entirely.
        if filters["precisions"] and series.precision not in filters["precisions"]:
            continue
        if filters["base_series"] and series.name not in filters["base_series"]:
            continue

        sp_fail = False
        for param, allowed in filters["series_params"].items():
            if allowed:
                val = series.arguments.get(param)
                if val is None or val not in allowed:
                    sp_fail = True
                    break
        if sp_fail:
            continue

        # --- Accel Filtering ---
        # We need to filter the accels list, but also decide if the series should be kept.
        # If accel filters are present, the series is kept only if at least one accel matches.

        # Check if we have any accel-related filters
        has_accel_filters = (
            bool(filters["base_accel"])
            or bool(filters["m_values"])
            or bool(filters.get("noise_options"))
            or any(filters["accel_params"].values())
        )

        if not has_accel_filters:
            # No accel filters, keep all accels
            filtered_result.append((series, accels))
            continue

        valid_accels = []
        for accel in accels:
            if (
                filters["base_accel"]
                and accel.accel_info.name not in filters["base_accel"]
            ):
                continue
            if (
                filters["m_values"]
                and accel.accel_info.m_value not in filters["m_values"]
            ):
                continue
            if (
                filters.get("noise_options")
                and accel.accel_info.noise_str not in filters["noise_options"]
            ):
                continue

            ap_fail = False
            for param, allowed in filters["accel_params"].items():
                if allowed:
                    val = accel.accel_info.additional_args.get(param)
                    if val is None or val not in allowed:
                        ap_fail = True
                        break
            if ap_fail:
                continue

            valid_accels.append(accel)

        # Only add the series if it has at least one matching acceleration
        if valid_accels:
            filtered_result.append((series, valid_accels))

    return filtered_result


class FlowLayout(QLayout):
    """
    A custom layout that wraps items like words in a paragraph.
    Great for lists of checkboxes that don't fit on one line.
    Standard Qt layouts don't support this "flow" behavior natively.
    """

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)
        self._items: List[Any] = []

    def __del__(self) -> None:
        item = self.takeAt(0)
        while item:
            item = self.takeAt(0)

    def addItem(self, item: Any) -> None:
        self._items.append(item)

    def count(self) -> int:
        return len(self._items)

    def itemAt(self, index: int) -> Optional[Any]:
        if 0 <= index < len(self._items):
            return self._items[index]
        return None

    def takeAt(self, index: int) -> Optional[Any]:
        if 0 <= index < len(self._items):
            return self._items.pop(index)
        return None

    def expandingDirections(self) -> Qt.Orientation:
        return Qt.Orientation(0)

    def hasHeightForWidth(self) -> bool:
        return True

    def heightForWidth(self, width: int) -> int:
        return self._do_layout(QRect(0, 0, width, 0), True)

    def setGeometry(self, rect: QRect) -> None:
        super().setGeometry(rect)
        self._do_layout(rect, False)

    def sizeHint(self) -> QSize:
        return self.minimumSize()

    def minimumSize(self) -> QSize:
        size = QSize()
        for item in self._items:
            size = size.expandedTo(item.minimumSize())
        margins = self.contentsMargins()
        size += QSize(
            margins.left() + margins.right(), margins.top() + margins.bottom()
        )
        return size

    def _do_layout(self, rect: QRect, test_only: bool) -> int:
        """
        Calculates the layout for the given rectangle.
        Iterates through items, placing them next to each other until they hit the right edge,
        then moves to the next line.
        """
        margins = self.contentsMargins()
        x = rect.x() + margins.left()
        y = rect.y() + margins.top()
        line_height = 0
        spacing = self.spacing()

        for item in self._items:
            style = item.widget().style()
            layout_spacing_x = style.layoutSpacing(
                QSizePolicy.ControlType.CheckBox,
                QSizePolicy.ControlType.CheckBox,
                Qt.Orientation.Horizontal,
            )
            layout_spacing_y = style.layoutSpacing(
                QSizePolicy.ControlType.CheckBox,
                QSizePolicy.ControlType.CheckBox,
                Qt.Orientation.Vertical,
            )
            space_x = spacing if spacing != -1 else layout_spacing_x
            space_y = spacing if spacing != -1 else layout_spacing_y

            next_x = x + item.sizeHint().width() + space_x
            if next_x - space_x > rect.right() - margins.right() and line_height > 0:
                x = rect.x() + margins.left()
                y = y + line_height + space_y
                next_x = x + item.sizeHint().width() + space_x
                line_height = 0

            if not test_only:
                item.setGeometry(QRect(QPoint(x, y), item.sizeHint()))

            x = next_x
            line_height = max(line_height, item.sizeHint().height())

        return y + line_height - rect.y() + margins.bottom()


class DataLoadThread(QThread):
    """
    Background thread for loading data without freezing the UI.
    Polars operations can take seconds, which would make the window unresponsive.
    """

    finished = pyqtSignal(list)
    error = pyqtSignal(str)

    def __init__(self, loader: DataLoader, filters: Dict[str, Any]):
        super().__init__()
        self.loader = loader
        self.filters = filters

    def run(self):
        """
        Executes the data loading process.
        """
        try:
            data = self.loader.filter_data(self.filters)
            self.finished.emit(data)
        except Exception as e:
            self.error.emit(str(e))


class SymLogAxisItem(pg.AxisItem):
    """
    Custom axis item for SymLog scale.
    Delegates tick formatting to our symlog_formatter.
    Standard logarithmic axes crash or behave poorly with negative numbers.
    """

    def tickStrings(self, values, scale, spacing):
        """
        Generates string labels for ticks.

        Inputs:
            values (List[float]): Tick values.
            scale (float): Scale factor.
            spacing (float): Spacing.

        Outputs:
            List[str]: Formatted strings.
        """
        return [symlog_formatter(v) for v in values]


class PlotWidget(pg.PlotWidget):
    """
    Enhanced PlotWidget with SymLog support and standard configuration.
    """

    def __init__(self, title, symlog=False, **kwargs):
        """
        Initializes the plot widget.

        Inputs:
            title (str): Plot title.
            symlog (bool): Whether to use symmetric log scale.
        """
        if symlog:
            # Swap the default axis item with our custom one BEFORE initialization
            kwargs["axisItems"] = {"left": SymLogAxisItem(orientation="left")}
        super().__init__(**kwargs)
        self.setTitle(title)
        self.showGrid(x=True, y=True)
        self.symlog = symlog
        if symlog:
            self.getAxis("left").setLabel("Value (Symlog)")
        else:
            self.getAxis("left").setLabel("Value")
        self.getAxis("bottom").setLabel("Iteration n")


class DashboardApp(QMainWindow):
    """
    The main window of the application.
    Orchestrates data loading, filtering, and visualization.
    """

    def __init__(self, data_dir: str):
        """
        Initializes the main window.

        Inputs:
            data_dir (str): Path to the data directory.
        """
        super().__init__()
        self.setWindowTitle("vizr")
        self.resize(1400, 800)

        self.loader = DataLoader(data_dir)

        # Load Filters (for fetching from disk)
        self.load_filters: Dict[str, Any] = {
            "precisions": set(),
            "base_series": set(),
            "base_accel": set(),
            "m_values": set(),
            "noise_options": set(),
            "series_params": {},
            "accel_params": {},
        }

        # Quick Filters (for filtering in-memory data)
        self.quick_filters: Dict[str, Any] = {
            "precisions": set(),
            "base_series": set(),
            "base_accel": set(),
            "m_values": set(),
            "noise_options": set(),
            "series_params": {},
            "accel_params": {},
        }

        self.data: List[Tuple[SeriesRecord, List[AccelRecord]]] = []
        self.filtered_data: List[Tuple[SeriesRecord, List[AccelRecord]]] = []
        self.symlog = True

        # Visualization Flags
        self.show_partial_sums = True
        self.show_limits = True
        self.show_real = True
        self.show_imaginary = True
        self.force_show_imaginary = False

        self.init_ui()

    def init_ui(self):
        """
        Sets up the entire UI hierarchy.
        """
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)

        # Scroll area for plots and filters.
        # This is important because on smaller screens the content might overflow.
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_container = QWidget()
        self.plots_layout = QVBoxLayout(self.scroll_container)

        # Load Filters
        self.setup_load_filters_ui(self.plots_layout)

        # Quick Filters Container (Dynamic)
        # This group box is populated after data is loaded.
        self.quick_filters_group = QGroupBox("Quick Filters (In-Memory)")
        self.quick_filters_group.hide()  # Hidden until data is loaded
        self.quick_filters_layout = QVBoxLayout(self.quick_filters_group)
        self.plots_layout.addWidget(self.quick_filters_group)

        # Visualization Options
        vis_options_group = QGroupBox("Visualization Options")
        vis_options_layout = QHBoxLayout(vis_options_group)

        self.symlog_check = QCheckBox("Symlog")
        self.symlog_check.setChecked(self.symlog)
        self.symlog_check.stateChanged.connect(self.on_symlog_changed)
        vis_options_layout.addWidget(self.symlog_check)

        self.partial_sums_check = QCheckBox("Partial Sums")
        self.partial_sums_check.setChecked(self.show_partial_sums)
        self.partial_sums_check.stateChanged.connect(self.on_visual_option_changed)
        vis_options_layout.addWidget(self.partial_sums_check)

        self.limits_check = QCheckBox("Limits")
        self.limits_check.setChecked(self.show_limits)
        self.limits_check.stateChanged.connect(self.on_visual_option_changed)
        vis_options_layout.addWidget(self.limits_check)

        self.real_check = QCheckBox("Real")
        self.real_check.setChecked(self.show_real)
        self.real_check.stateChanged.connect(self.on_visual_option_changed)
        vis_options_layout.addWidget(self.real_check)

        self.imaginary_check = QCheckBox("Imaginary")
        self.imaginary_check.setChecked(self.show_imaginary)
        self.imaginary_check.stateChanged.connect(self.on_visual_option_changed)
        vis_options_layout.addWidget(self.imaginary_check)

        self.force_imaginary_check = QCheckBox("Force Imaginary")
        self.force_imaginary_check.setChecked(self.force_show_imaginary)
        self.force_imaginary_check.stateChanged.connect(self.on_visual_option_changed)
        vis_options_layout.addWidget(self.force_imaginary_check)

        self.plots_layout.addWidget(vis_options_group)

        self.setup_plots()

        self.scroll_area.setWidget(self.scroll_container)
        main_layout.addWidget(self.scroll_area, 1)

        # Right sidebar for Legend
        self.legend_list = QListWidget()
        self.legend_list.setFixedWidth(300)
        self.legend_list.setWordWrap(True)
        main_layout.addWidget(self.legend_list)

    def setup_load_filters_ui(self, layout):
        """
        Creates the UI section for initial data loading filters.
        These filters determine what is fetched from Parquet in the first place.

        Inputs:
            layout (QLayout): The layout to add widgets to.
        """
        metadata = self.loader.metadata

        filter_header = QLabel("Load Filters")
        font = filter_header.font()
        font.setBold(True)
        font.setPointSize(12)
        filter_header.setFont(font)
        layout.addWidget(filter_header)

        # Precisions
        layout.addWidget(
            self.create_filter_group(
                "Precisions", metadata.precisions, "precisions", self.load_filters
            )
        )

        # Series
        layout.addWidget(
            self.create_filter_group(
                "Series Names", metadata.series_names, "base_series", self.load_filters
            )
        )

        # Accel
        layout.addWidget(
            self.create_filter_group(
                "Accel Names", metadata.accel_names, "base_accel", self.load_filters
            )
        )

        # M Values
        layout.addWidget(
            self.create_filter_group(
                "M Values",
                [str(m) for m in metadata.m_values],
                "m_values",
                self.load_filters,
                is_int=True,
            )
        )

        # Noise
        if metadata.noise_options:
            layout.addWidget(
                self.create_filter_group(
                    "Noise Options",
                    metadata.noise_options,
                    "noise_options",
                    self.load_filters,
                )
            )

        # Series Params
        for param, values in metadata.series_param_info.items():
            layout.addWidget(
                self.create_filter_group(
                    f"Series: {param}",
                    values,
                    ("series_params", param),
                    self.load_filters,
                )
            )

        # Accel Params
        for param, values in metadata.accel_param_info.items():
            layout.addWidget(
                self.create_filter_group(
                    f"Accel: {param}",
                    values,
                    ("accel_params", param),
                    self.load_filters,
                )
            )

        # Refresh row
        refresh_layout = QHBoxLayout()
        self.refresh_btn = QPushButton("Reload data")
        self.refresh_btn.clicked.connect(self.refresh_data)
        refresh_layout.addWidget(self.refresh_btn)

        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 0)
        self.progress_bar.hide()
        refresh_layout.addWidget(self.progress_bar)
        layout.addLayout(refresh_layout)

        # Divider
        line = QFrame()
        line.setFrameShape(QFrame.Shape.HLine)
        line.setFrameShadow(QFrame.Shadow.Sunken)
        layout.addWidget(line)

    def create_filter_group(
        self,
        title,
        items,
        filter_key,
        target_dict,
        is_int=False,
        on_change=None,
    ):
        """
        Creates a group box containing checkboxes for filtering options.

        Inputs:
            title (str): Group title.
            items (List[str]): List of items to create checkboxes for.
            filter_key (str/tuple): Key in target_dict to update.
            target_dict (Dict): Dictionary storing filter state.
            is_int (bool): Convert values to int before storing.
            on_change (Callable): Callback when selection changes.

        Outputs:
            QGroupBox: The created widget group.
        """
        group = QGroupBox(title)
        layout = QVBoxLayout()
        layout.setContentsMargins(5, 5, 5, 5)

        flow = FlowLayout()
        layout.addLayout(flow)

        all_btn = QPushButton("All")
        none_btn = QPushButton("None")

        # Make buttons compact
        all_btn.setFixedSize(40, 20)
        none_btn.setFixedSize(50, 20)
        font = all_btn.font()
        font.setPointSize(8)
        all_btn.setFont(font)
        none_btn.setFont(font)

        flow.addWidget(all_btn)
        flow.addWidget(none_btn)

        checkboxes = []
        for item in items:
            cb = QCheckBox(item)
            checkboxes.append(cb)
            flow.addWidget(cb)

            # Determine if currently checked (state recovery)
            val = int(item) if is_int else item

            is_checked = False
            # Handle nested keys for params
            if isinstance(filter_key, tuple):
                p_key, s_key = filter_key
                if s_key in target_dict[p_key] and val in target_dict[p_key][s_key]:
                    is_checked = True
            else:
                if val in target_dict[filter_key]:
                    is_checked = True

            if is_checked:
                cb.setChecked(True)

            def make_toggle(item_val=item, cb_obj=cb):
                def toggle(state):
                    val = int(item_val) if is_int else item_val
                    if isinstance(filter_key, tuple):
                        parent_key, sub_key = filter_key
                        if parent_key not in target_dict:
                            target_dict[parent_key] = {}
                        if sub_key not in target_dict[parent_key]:
                            target_dict[parent_key][sub_key] = set()
                        target = target_dict[parent_key][sub_key]
                    else:
                        target = target_dict[filter_key]

                    if state == Qt.CheckState.Checked.value:
                        target.add(val)
                    else:
                        target.discard(val)

                    if on_change:
                        on_change()

                return toggle

            cb.stateChanged.connect(make_toggle())

        def select_all():
            for cb in checkboxes:
                cb.setChecked(True)

        def select_none():
            for cb in checkboxes:
                cb.setChecked(False)

        all_btn.clicked.connect(select_all)
        none_btn.clicked.connect(select_none)

        group.setLayout(layout)
        return group

    def setup_quick_filters_ui(self):
        """
        Creates the UI section for in-memory filtering (fast).
        Updates based on the currently loaded data.
        """
        # Clear existing layout because options change when new data is loaded.
        while self.quick_filters_layout.count():
            item = self.quick_filters_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
            elif item.layout():
                # Recursively delete layout items?
                # FlowLayout clean up handles its items
                pass

        # Calculate available options based on CURRENT data.
        options = extract_filter_options(self.data)

        if (
            not any(options.values())
            and not options.get("series_params")
            and not options.get("accel_params")
        ):
            self.quick_filters_group.hide()
            return

        self.quick_filters_group.show()

        # Helper to trigger update
        def on_change():
            self.apply_quick_filters_and_update()

        # Precisions
        if options["precisions"]:
            self.quick_filters_layout.addWidget(
                self.create_filter_group(
                    "Precisions",
                    options["precisions"],
                    "precisions",
                    self.quick_filters,
                    on_change=on_change,
                )
            )

        # Series
        if options["base_series"]:
            self.quick_filters_layout.addWidget(
                self.create_filter_group(
                    "Series Names",
                    options["base_series"],
                    "base_series",
                    self.quick_filters,
                    on_change=on_change,
                )
            )

        # Accel
        if options["base_accel"]:
            self.quick_filters_layout.addWidget(
                self.create_filter_group(
                    "Accel Names",
                    options["base_accel"],
                    "base_accel",
                    self.quick_filters,
                    on_change=on_change,
                )
            )

        # M Values
        if options["m_values"]:
            self.quick_filters_layout.addWidget(
                self.create_filter_group(
                    "M Values",
                    [str(m) for m in options["m_values"]],
                    "m_values",
                    self.quick_filters,
                    is_int=True,
                    on_change=on_change,
                )
            )

        # Noise
        if options["noise_options"]:
            self.quick_filters_layout.addWidget(
                self.create_filter_group(
                    "Noise Options",
                    options["noise_options"],
                    "noise_options",
                    self.quick_filters,
                    on_change=on_change,
                )
            )

        # Series Params

        # Accel Params
        if options["accel_params"]:
            for param, values in options["accel_params"].items():
                self.quick_filters_layout.addWidget(
                    self.create_filter_group(
                        f"Accel: {param}",
                        values,
                        ("accel_params", param),
                        self.quick_filters,
                        on_change=on_change,
                    )
                )

    def setup_plots(self):
        """
        Initializes the three main plots and the data table.
        """
        self.convergence_plot = PlotWidget("Convergence")
        self.convergence_plot.setMinimumHeight(400)
        self.setup_hover(self.convergence_plot)
        self.plots_layout.addWidget(self.convergence_plot)

        self.error_plot = PlotWidget("Error Convergence", symlog=self.symlog)
        self.error_plot.setMinimumHeight(400)
        self.setup_hover(self.error_plot)
        self.plots_layout.addWidget(self.error_plot)

        self.performance_plot = PlotWidget("Performance", symlog=self.symlog)
        self.performance_plot.setMinimumHeight(400)
        self.setup_hover(self.performance_plot)
        self.plots_layout.addWidget(self.performance_plot)

        # Table
        self.table = QTableWidget()
        self.table.setColumnCount(14)
        self.table.setHorizontalHeaderLabels(
            [
                "Series ID",
                "Name",
                "Precision",
                "Limit",
                "Series Params",
                "Accel Name",
                "M",
                "Accel Params",
                "Noise",
                "Series Values",
                "Accel Values",
                "Deviations",
                "Errors",
                "Events",
            ]
        )
        self.table.horizontalHeader().setSectionResizeMode(
            QHeaderView.ResizeMode.Interactive
        )
        self.table.horizontalHeader().setStretchLastSection(True)
        self.table.setMinimumHeight(400)
        self.plots_layout.addWidget(self.table)

    def setup_hover(self, plot_widget):
        """
        Sets up the mouse hover functionality for a plot widget.
        Displays tooltips when hovering over curves.

        This is tricky because we often have many overlapping curves or points.

        Inputs:
            plot_widget (PlotWidget): The widget to attach hover logic to.
        """
        # Create a text item for the tooltip
        tooltip = pg.TextItem(
            text="",
            color=(255, 255, 255),
            anchor=(0, 1),
            border="w",
            fill=(0, 0, 0, 200),
        )
        tooltip.hide()

        # Store reference in the widget for easy access
        plot_widget.custom_tooltip = tooltip

        def mouse_moved(evt):
            pos = evt[0]
            if plot_widget.sceneBoundingRect().contains(pos):
                mouse_point = plot_widget.plotItem.vb.mapSceneToView(pos)
                self.update_tooltip(plot_widget, mouse_point, pos)

        # Rate limit is important to avoid lagging on mouse move.
        plot_widget.proxy = pg.SignalProxy(
            plot_widget.scene().sigMouseMoved, rateLimit=60, slot=mouse_moved
        )

    def dist_to_segment(self, p, v, w):
        """
        Calculates the minimum distance from point p to the line segment vw.
        All inputs are QPointF-like objects (screen coordinates).

        Inputs:
            p, v, w (QPointF): Points.

        Outputs:
            float: Distance.
        """
        # p, v, w are QPointF (screen coords)
        # Return distance from p to segment vw

        l2 = (v.x() - w.x()) ** 2 + (v.y() - w.y()) ** 2
        if l2 == 0:
            return np.hypot(p.x() - v.x(), p.y() - v.y())

        # t = ((p - v) . (w - v)) / l2
        t = ((p.x() - v.x()) * (w.x() - v.x()) + (p.y() - v.y()) * (w.y() - v.y())) / l2
        t = max(0, min(1, t))

        # Projection point
        proj_x = v.x() + t * (w.x() - v.x())
        proj_y = v.y() + t * (w.y() - v.y())

        return np.hypot(p.x() - proj_x, p.y() - proj_y)

    def update_tooltip(self, plot_widget, mouse_point, scene_pos):
        """
        Updates the tooltip position and text based on mouse location.
        Finds the closest curve or point to the mouse cursor.

        Inputs:
            plot_widget (PlotWidget): The plot widget.
            mouse_point (QPointF): Mouse position in plot coordinates.
            scene_pos (QPointF): Mouse position in scene coordinates.
        """
        mouse_x = mouse_point.x()

        closest_curve = None
        closest_name = None
        min_dist = 20.0  # pixels

        vb = plot_widget.plotItem.vb
        mouse_screen_pt = vb.mapViewToDevice(mouse_point)
        if mouse_screen_pt is None:
            return

        for item in plot_widget.plotItem.listDataItems():
            if not item.isVisible():
                continue

            x_data = item.xData
            y_data = item.yData

            if x_data is None or len(x_data) == 0:
                continue

            candidate_dist = float("inf")
            candidate_name = item.opts.get("name", "Unknown")

            # Helper to check a segment
            def check_segment(xs, ys, seg_name):
                # We need to find indices surrounding mouse_x
                # idx is where mouse_x should be inserted to maintain order
                idx = np.searchsorted(xs, mouse_x)

                local_min = float("inf")

                # We check the segment [idx-1, idx]
                # Ensure indices are valid within this slice
                i_left = idx - 1
                i_right = idx

                if i_left >= 0 and i_right < len(xs):
                    # Valid segment
                    p1 = vb.mapViewToDevice(pg.Point(xs[i_left], ys[i_left]))
                    p2 = vb.mapViewToDevice(pg.Point(xs[i_right], ys[i_right]))
                    if p1 and p2:
                        d = self.dist_to_segment(mouse_screen_pt, p1, p2)
                        if d < local_min:
                            local_min = d

                # Also check point distance for idx (right side) if we are off the end?
                # or just standard robustness if searchsorted puts us exactly on point
                if i_right < len(xs):
                    p = vb.mapViewToDevice(pg.Point(xs[i_right], ys[i_right]))
                    if p:
                        d = np.hypot(
                            p.x() - mouse_screen_pt.x(), p.y() - mouse_screen_pt.y()
                        )
                        if d < local_min:
                            local_min = d

                if i_left >= 0:
                    p = vb.mapViewToDevice(pg.Point(xs[i_left], ys[i_left]))
                    if p:
                        d = np.hypot(
                            p.x() - mouse_screen_pt.x(), p.y() - mouse_screen_pt.y()
                        )
                        if d < local_min:
                            local_min = d

                return local_min

            if hasattr(item, "point_names"):
                # Scatter plot with named points (Performance Plot)
                view_range_x = vb.viewRange()[0]
                view_width = view_range_x[1] - view_range_x[0]

                # Filter indices within reasonable X range (broad phase)
                x_margin = view_width * 0.1
                mask = (x_data >= mouse_x - x_margin) & (x_data <= mouse_x + x_margin)
                indices = np.where(mask)[0]

                for i in indices:
                    pt = vb.mapViewToDevice(pg.Point(x_data[i], y_data[i]))
                    if pt:
                        d = np.hypot(
                            pt.x() - mouse_screen_pt.x(), pt.y() - mouse_screen_pt.y()
                        )
                        if d < candidate_dist:
                            candidate_dist = d
                            candidate_name = item.point_names[i]

            elif hasattr(item, "segments"):
                # Batched segments.
                # This is an optimization where multiple curves are drawn as one continuous line with gaps (NaNs).
                # We need to check which segment we are closest to.
                for start, end, info_name in item.segments:
                    seg_x = x_data[start:end]
                    seg_y = y_data[start:end]

                    # Optimization: Skip segments clearly out of X range
                    if len(seg_x) > 0:
                        if mouse_x < seg_x[0] - 1 or mouse_x > seg_x[-1] + 1:
                            continue

                    d = check_segment(seg_x, seg_y, info_name)
                    if d < candidate_dist:
                        candidate_dist = d
                        candidate_name = info_name
            else:
                # Standard curve
                d = check_segment(x_data, y_data, candidate_name)
                candidate_dist = d

            if candidate_dist < min_dist:
                min_dist = candidate_dist
                closest_curve = item
                closest_name = candidate_name

        tooltip = plot_widget.custom_tooltip
        if closest_curve:
            tooltip.setText(closest_name)
            tooltip.setPos(mouse_point.x(), mouse_point.y())
            tooltip.show()
        else:
            tooltip.hide()

    def on_visual_option_changed(self, state):
        """
        Callback for visualization checkboxes.
        """
        self.show_partial_sums = self.partial_sums_check.isChecked()
        self.show_limits = self.limits_check.isChecked()
        self.show_real = self.real_check.isChecked()
        self.show_imaginary = self.imaginary_check.isChecked()
        self.force_show_imaginary = self.force_imaginary_check.isChecked()
        self.update_plots()

    def on_symlog_changed(self, state):
        """
        Callback for symlog checkbox.
        """
        self.symlog = state == Qt.CheckState.Checked.value
        # Update error_plot and performance_plot axis
        for plot in [self.error_plot, self.performance_plot]:
            if self.symlog:
                plot.getPlotItem().setAxisItems(
                    {"left": SymLogAxisItem(orientation="left")}
                )
                plot.getAxis("left").setLabel("Value (Symlog)")
            else:
                plot.getPlotItem().setAxisItems(
                    {"left": pg.AxisItem(orientation="left")}
                )
                plot.getAxis("left").setLabel("Value")
        self.update_plots()

    def add_legend_item(self, text, color, is_dashed=False):
        """
        Adds an item to the custom legend widget.

        Inputs:
            text (str): Label text.
            color (QColor): Icon color.
            is_dashed (bool): Whether to draw dashed line icon.
        """
        pixmap = QPixmap(16, 16)
        pixmap.fill(color)

        if is_dashed:
            painter = QPainter(pixmap)
            pen = QPen(Qt.GlobalColor.white)
            pen.setWidth(2)
            pen.setStyle(Qt.PenStyle.DashLine)
            painter.setPen(pen)
            painter.drawRect(0, 0, 15, 15)
            painter.end()

        item = QListWidgetItem(QIcon(pixmap), text)
        self.legend_list.addItem(item)

    def refresh_data(self):
        """
        Triggers the data loading process in a background thread.
        """
        self.refresh_btn.setEnabled(False)
        self.progress_bar.show()

        self.load_thread = DataLoadThread(self.loader, self.load_filters)
        self.load_thread.finished.connect(self.on_data_loaded)
        self.load_thread.error.connect(self.on_load_error)
        self.load_thread.start()

    def on_data_loaded(self, data):
        """
        Callback when data loading completes successfully.

        Inputs:
            data (List): Loaded data.
        """
        self.data = data
        self.refresh_btn.setEnabled(True)
        self.progress_bar.hide()

        # Setup Quick Filters for new data
        self.setup_quick_filters_ui()

        # Apply initial filtering (using default empty filters = Show All)
        self.apply_quick_filters_and_update()

    def apply_quick_filters_and_update(self):
        """
        Applies in-memory filters and updates the visualization.
        """
        print("Applying Quick Filters...")
        t0 = time.time()
        self.filtered_data = filter_data_items(self.data, self.quick_filters)
        print(
            f"Quick Filters applied in {time.time() - t0:.3f}s. Items: {len(self.filtered_data)}/{len(self.data)}"
        )
        self.update_plots()

    def on_load_error(self, err_msg):
        """
        Callback for loading errors.
        """
        print(f"Error loading data: {err_msg}")
        self.refresh_btn.setEnabled(True)
        self.progress_bar.hide()

    def format_args(self, args: Dict[str, str]) -> str:
        """
        Formats argument dictionary to string.
        """
        if not args:
            return ""
        items = [f"{k}={v}" for k, v in args.items()]
        return "{" + ", ".join(items) + "}"

    def update_plots(self):
        """
        Redraws all plots and populates the data table.
        """
        import time

        t_start = time.time()
        print("Start updating plots...")

        self.convergence_plot.clear()
        self.error_plot.clear()
        self.performance_plot.clear()

        # Re-add tooltips
        if hasattr(self.convergence_plot, "custom_tooltip"):
            self.convergence_plot.addItem(self.convergence_plot.custom_tooltip)
            self.convergence_plot.custom_tooltip.hide()
        if hasattr(self.error_plot, "custom_tooltip"):
            self.error_plot.addItem(self.error_plot.custom_tooltip)
            self.error_plot.custom_tooltip.hide()
        if hasattr(self.performance_plot, "custom_tooltip"):
            self.performance_plot.addItem(self.performance_plot.custom_tooltip)
            self.performance_plot.custom_tooltip.hide()

        # Clear Legend
        t_legend_clear_start = time.time()
        self.legend_list.clear()
        t_legend_clear = time.time() - t_legend_clear_start

        self.table.setRowCount(0)

        # We limit the number of rows to avoid freezing the UI if the user selects "All".
        row_limit = 100
        rows_added = 0

        total_accels = 0
        t_plotting = 0
        t_table = 0
        t_legend_add = 0

        # Performance points collection.
        # We collect all points first and then plot them in one go to be faster.
        perf_x = []
        perf_y = []
        perf_names = []
        perf_brushes = []

        # Colors matching Rust implementation
        COLOR_PARTIAL_REAL = QColor(128, 128, 128)
        COLOR_PARTIAL_IMAG = QColor(255, 192, 203)
        COLOR_LIMIT_REAL = QColor(255, 0, 0)
        COLOR_LIMIT_IMAG = QColor(255, 100, 100)
        COLOR_ACCEL_IMAG = QColor(255, 165, 0)

        # Use filtered_data instead of data
        n_series = len(self.filtered_data)

        for i, (series, accels) in enumerate(self.filtered_data):
            # Generate color based on index for Accel Real lines.
            # This ensures distinct colors for different series.
            series_color = pg.intColor(i, n_series)

            if i % 5 == 0:
                print(f"Processed {i} series...")

            series_params = self.format_args(series.arguments)
            series_legend_name = f"{series.name} {series_params}"

            # --- Convergence Plotting ---
            n_vals = series.n

            # 1. Partial Sums
            if self.show_partial_sums:
                if self.show_real:
                    real_vals = vectorized_approx_f64(
                        series.val_real_m, series.val_real_e
                    )
                    self.convergence_plot.plot(
                        n_vals,
                        real_vals,
                        pen=pg.mkPen(COLOR_PARTIAL_REAL, width=2),
                        name=f"{series_legend_name} (Partial)",
                    )

                    if self.symlog:
                        series_dev = vectorized_symlog(series.dev_m, series.dev_e)
                    else:
                        series_dev = vectorized_approx_f64(series.dev_m, series.dev_e)

                    self.error_plot.plot(
                        n_vals,
                        series_dev,
                        pen=pg.mkPen(QColor(255, 0, 0), width=3),
                        name=f"{series_legend_name} (Partial Error)",
                    )

                if self.show_imaginary:
                    is_zero_imag = np.all(series.val_imag_m == 0.0)  # Simplified check
                    if self.force_show_imaginary or not is_zero_imag:
                        imag_vals = vectorized_approx_f64(
                            series.val_imag_m, series.val_imag_e
                        )
                        self.convergence_plot.plot(
                            n_vals,
                            imag_vals,
                            pen=pg.mkPen(COLOR_PARTIAL_IMAG, width=2),
                            name=f"{series_legend_name} (Partial Imag)",
                        )

            # 2. Limits
            if self.show_limits:
                limit = series.series_limit

                if self.show_real:
                    real_limit = limit.real.approx_f64()
                    # Plot horizontal line representing the true limit.
                    if len(n_vals) > 0:
                        min_x, max_x = n_vals[0], n_vals[-1]
                        self.convergence_plot.plot(
                            [min_x, max_x],
                            [real_limit, real_limit],
                            pen=pg.mkPen(COLOR_LIMIT_REAL, width=3),
                            name=f"{series_legend_name} (Limit)",
                        )

                if self.show_imaginary:
                    imag_limit = limit.imag.approx_f64()
                    is_zero_imag = imag_limit == 0.0
                    if self.force_show_imaginary or not is_zero_imag:
                        if len(n_vals) > 0:
                            min_x, max_x = n_vals[0], n_vals[-1]
                            self.convergence_plot.plot(
                                [min_x, max_x],
                                [imag_limit, imag_limit],
                                pen=pg.mkPen(COLOR_LIMIT_IMAG, width=2),
                                name=f"{series_legend_name} (Limit Imag)",
                            )

            # 3. Accelerations
            t0 = time.time()

            # Collect accel curves for batch plotting (Real Only).
            # Creating individual PlotCurveItems is expensive, so we combine data
            # into one large array with NaN gaps to separate lines.
            series_accel_n = []
            series_accel_real = []
            series_accel_dev = []  # For Error Plot

            accel_segments = []  # For Tooltips (Real)
            current_offset = 0

            # For Imaginary (batching separate due to different color)
            series_accel_n_imag = []
            series_accel_imag = []
            accel_segments_imag = []
            current_offset_imag = 0

            for accel in accels:
                total_accels += 1

                accel_params = self.format_args(accel.accel_info.additional_args)
                noise_str = f" [Noise: {accel.accel_info.noise_str}]" if accel.accel_info.noise_str != "None" else ""

                accel_legend_name = f"{accel.accel_info.name} (m={accel.accel_info.m_value}){accel_params}{noise_str}"

                if self.show_real:
                    self.add_legend_item(
                        f"  -> {accel_legend_name}", series_color, is_dashed=True
                    )

                mask = accel.valid_mask
                if not np.any(mask):
                    continue

                min_len = min(len(n_vals), len(mask))

                current_n = n_vals[:min_len]
                current_mask = mask[:min_len]
                accel_n = current_n[current_mask]

                # --- Real Accel ---
                if self.show_real:
                    accel_real = vectorized_approx_f64(
                        accel.val_real_m[:min_len][current_mask],
                        accel.val_real_e[:min_len][current_mask],
                    )

                    # Error Plot Data (Deviation)
                    # We always need real deviation values for statistics
                    accel_dev_real = vectorized_approx_f64(
                        accel.dev_m[:min_len][current_mask],
                        accel.dev_e[:min_len][current_mask],
                    )

                    if self.symlog:
                        accel_dev = vectorized_symlog(
                            accel.dev_m[:min_len][current_mask],
                            accel.dev_e[:min_len][current_mask],
                        )
                    else:
                        accel_dev = accel_dev_real

                    seg_len = len(accel_n)
                    # Track where this segment lives in the batched array for tooltip mapping.
                    accel_segments.append(
                        (current_offset, current_offset + seg_len, accel_legend_name)
                    )
                    current_offset += seg_len + 1  # +1 for the NaN separator

                    series_accel_n.append(accel_n.astype(float))
                    series_accel_n.append(np.array([np.nan]))

                    series_accel_real.append(accel_real)
                    series_accel_real.append(np.array([np.nan]))

                    series_accel_dev.append(accel_dev)
                    series_accel_dev.append(np.array([np.nan]))

                    # Min error for performance plot
                    # Find the best result this algorithm achieved.
                    valid_dev_indices = np.where(~np.isnan(accel_dev))[0]
                    if len(valid_dev_indices) > 0:
                        min_idx_local = np.argmin(accel_dev[valid_dev_indices])
                        min_idx = valid_dev_indices[min_idx_local]
                        min_error = accel_dev[min_idx]
                        min_error_n = accel_n[min_idx]

                        perf_x.append(min_error_n)
                        perf_y.append(min_error)
                        perf_names.append(accel_legend_name)
                        perf_brushes.append(pg.mkBrush(series_color))

                        # Table Row
                        if rows_added < row_limit:
                            # Statistics on REAL deviations (not symlog)
                            valid_real_indices = np.where(~np.isnan(accel_dev_real))[0]
                            if len(valid_real_indices) > 0:
                                valid_vals = accel_dev_real[valid_real_indices]
                                mean_err = np.mean(valid_vals)
                                min_err = np.min(valid_vals)
                                dev_summary = (
                                    f"Mean: {mean_err:.2e}, Min: {min_err:.2e}"
                                )
                            else:
                                dev_summary = "No valid data"

                            row_idx = self.table.rowCount()
                            self.table.insertRow(row_idx)

                            # Standard Text Items
                            self.table.setItem(
                                row_idx, 0, QTableWidgetItem(str(series.series_id))
                            )
                            self.table.setItem(
                                row_idx, 1, QTableWidgetItem(series.name)
                            )
                            self.table.setItem(
                                row_idx, 2, QTableWidgetItem(series.precision)
                            )
                            self.table.setItem(
                                row_idx,
                                3,
                                QTableWidgetItem(series.series_limit.format()),
                            )
                            self.table.setItem(
                                row_idx, 4, QTableWidgetItem(series_params)
                            )
                            self.table.setItem(
                                row_idx, 5, QTableWidgetItem(accel.accel_info.name)
                            )
                            self.table.setItem(
                                row_idx,
                                6,
                                QTableWidgetItem(str(accel.accel_info.m_value)),
                            )
                            self.table.setItem(
                                row_idx,
                                7,
                                QTableWidgetItem(
                                    self.format_args(accel.accel_info.additional_args)
                                ),
                            )
                            
                            # 8. Noise
                            self.table.setItem(row_idx, 8, QTableWidgetItem(accel.accel_info.noise_str))

                            # Collapsible Widgets for Lists

                            # 9. Series Values

                            def format_sci(m, e):
                                return f"{m * (10.0**e):.6e}"

                            s_lines = []
                            for i in range(len(series.n)):
                                val_str = format_sci(
                                    series.val_real_m[i], series.val_real_e[i]
                                )
                                if series.val_imag_m[i] != 0:
                                    val_str += f" + {format_sci(series.val_imag_m[i], series.val_imag_e[i])}j"
                                s_lines.append(f"n={series.n[i]}: {val_str}")

                            if s_lines:
                                self.table.setCellWidget(
                                    row_idx,
                                    9,
                                    CollapsibleCellWidget(
                                        f"{len(s_lines)} values",
                                        s_lines,
                                        self.table,
                                        row_idx,
                                    ),
                                )
                            else:
                                self.table.setItem(
                                    row_idx, 9, QTableWidgetItem("(empty)")
                                )

                            # 10. Accel Values
                            a_lines = []
                            for i in range(len(accel_n)):
                                val_str = f"{accel_real[i]:.6e}"
                                a_lines.append(f"n={int(accel_n[i])}: {val_str}")

                            if a_lines:
                                self.table.setCellWidget(
                                    row_idx,
                                    10,
                                    CollapsibleCellWidget(
                                        f"{len(a_lines)} values",
                                        a_lines,
                                        self.table,
                                        row_idx,
                                    ),
                                )
                            else:
                                self.table.setItem(
                                    row_idx, 10, QTableWidgetItem("(empty)")
                                )

                            # 11. Deviations
                            d_lines = []
                            for i in range(len(accel_n)):
                                # accel_dev_real is aligned with accel_n
                                d_lines.append(
                                    f"n={int(accel_n[i])}: {accel_dev_real[i]:.6e}"
                                )

                            if d_lines:
                                self.table.setCellWidget(
                                    row_idx,
                                    11,
                                    CollapsibleCellWidget(
                                        dev_summary, d_lines, self.table, row_idx
                                    ),
                                )
                            else:
                                self.table.setItem(
                                    row_idx, 11, QTableWidgetItem("No data")
                                )

                            # 12. Errors
                            err_lines = [f"n={e.n}: {e.message}" for e in accel.errors]
                            if err_lines:
                                self.table.setCellWidget(
                                    row_idx,
                                    12,
                                    CollapsibleCellWidget(
                                        f"{len(err_lines)} errors",
                                        err_lines,
                                        self.table,
                                        row_idx,
                                    ),
                                )
                            else:
                                self.table.setItem(row_idx, 12, QTableWidgetItem(""))

                            # 13. Events
                            evt_lines = [
                                f"n={e.n}: {e.name} - {e.description}"
                                for e in accel.events
                            ]
                            if evt_lines:
                                self.table.setCellWidget(
                                    row_idx,
                                    13,
                                    CollapsibleCellWidget(
                                        f"{len(evt_lines)} events",
                                        evt_lines,
                                        self.table,
                                        row_idx,
                                    ),
                                )
                            else:
                                self.table.setItem(row_idx, 13, QTableWidgetItem(""))

                            rows_added += 1

                # --- Imag Accel ---
                if self.show_imaginary:
                    # Check zero
                    # Accessing numpy arrays is cheap
                    v_im_m = accel.val_imag_m[:min_len][current_mask]
                    is_zero_imag = np.all(v_im_m == 0.0)

                    if self.force_show_imaginary or not is_zero_imag:
                        accel_imag = vectorized_approx_f64(
                            v_im_m, accel.val_imag_e[:min_len][current_mask]
                        )

                        seg_len = len(accel_n)
                        accel_segments_imag.append(
                            (
                                current_offset_imag,
                                current_offset_imag + seg_len,
                                f"{accel_legend_name} (Imag)",
                            )
                        )
                        current_offset_imag += seg_len + 1

                        series_accel_n_imag.append(accel_n.astype(float))
                        series_accel_n_imag.append(np.array([np.nan]))

                        series_accel_imag.append(accel_imag)
                        series_accel_imag.append(np.array([np.nan]))

            t_plotting += time.time() - t0

            # Batch Plotting Real
            if series_accel_n:
                all_n = np.concatenate(series_accel_n)
                all_real = np.concatenate(series_accel_real)
                all_dev = np.concatenate(series_accel_dev)

                # Series Color for Accel Real
                pen = pg.mkPen(
                    series_color, width=1, style=Qt.PenStyle.DashLine, cosmetic=True
                )

                curve_conv = self.convergence_plot.plot(
                    all_n,
                    all_real,
                    connect="finite",
                    pen=pen,
                    name=f"{series.name} (accels)",
                )
                curve_conv.segments = accel_segments

                curve_err = self.error_plot.plot(
                    all_n,
                    all_dev,
                    connect="finite",
                    pen=pen,
                    name=f"{series.name} (accels)",
                )
                curve_err.segments = accel_segments

            # Batch Plotting Imag
            if series_accel_n_imag:
                all_n_imag = np.concatenate(series_accel_n_imag)
                all_imag = np.concatenate(series_accel_imag)

                # Orange for Accel Imag
                pen = pg.mkPen(
                    COLOR_ACCEL_IMAG, width=1, style=Qt.PenStyle.DashLine, cosmetic=True
                )

                curve_conv_imag = self.convergence_plot.plot(
                    all_n_imag,
                    all_imag,
                    connect="finite",
                    pen=pen,
                    name=f"{series.name} (accels imag)",
                )
                curve_conv_imag.segments = accel_segments_imag

        # Batch plot performance scatter
        t0 = time.time()
        if perf_x:
            perf_curve = self.performance_plot.plot(
                perf_x,
                perf_y,
                pen=None,
                symbol="o",
                symbolBrush=perf_brushes,
                symbolSize=5,
            )
            perf_curve.point_names = perf_names

        t_plotting += time.time() - t0

        # Probably need to remove some metrics?
        print(
            f"Finished updating plots structure in {time.time() - t_start:.3f}s. Rendering starting..."
        )
        print(f"  - Plotting setup time: {t_plotting:.3f}s")
        print(f"  - Table setup time: {t_table:.3f}s")
        print(f"  - Legend clear time: {t_legend_clear:.3f}s")
        print(f"  - Legend add time: {t_legend_add:.3f}s")
        print(f"  - Total accels processed: {total_accels}")


def main():
    """
    Entry point for the application.
    Parses arguments and launches the PyQt application.
    """
    parser = argparse.ArgumentParser(description="Vizr")
    parser.add_argument(
        "data_dir", help="Path to the directory containing parquet files"
    )
    args = parser.parse_args()

    app = QApplication(sys.argv)
    window = DashboardApp(args.data_dir)
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
