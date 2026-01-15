"""
Author: Sobolev Y. A.
Description: Main application entry point and UI implementation using PyQt6.
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
from .logic import (
    TableRowData,
    ViewOptions,
    VisualizationData,
    extract_filter_options,
    filter_dataset,
    prepare_viz_data,
)
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

        # toggle button
        # we use a tool button because it can have an icon and text.
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

        # content area
        # hidden by default to save space.
        self.content_area = QTextEdit()
        self.content_area.setReadOnly(True)
        self.content_area.setText("\n".join(content_lines))
        self.content_area.setMaximumHeight(150)  # cap height to avoid massive expansion
        self.content_area.setVisible(False)
        self.layout.addWidget(self.content_area)

    def on_toggle(self, checked: bool):
        """
        Handles the expansion/collapse logic.

        Inputs:
            checked (bool): The new state of the toggle button.
        """
        arrow = "v" if checked else ">"
        text = self.toggle_btn.text()[1:]  # keep original title
        self.toggle_btn.setText(f"{arrow}{text}")
        self.content_area.setVisible(checked)

        # trigger resize of the row so the content fits.
        self.parent_table.resizeRowToContents(self.row_idx)


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
            # swap the default axis item with our custom one BEFORE initialization
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

        # load filters (for fetching from disk)
        self.load_filters: Dict[str, Any] = {
            "precisions": set(),
            "base_series": set(),
            "base_accel": set(),
            "m_values": set(),
            "noise_options": set(),
            "series_params": {},
            "accel_params": {},
        }

        # quick filters (for filtering in-memory data)
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

        # visualization flags
        self.show_partial_sums = True
        self.show_limits = True
        self.show_real = True
        self.show_imaginary = True
        self.force_show_imaginary = False
        self.show_filters = True

        self.init_ui()

    def init_ui(self):
        """
        Sets up the entire UI hierarchy.
        """
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)

        # scroll area for plots and filters.
        # this is important because on smaller screens the content might overflow.
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_container = QWidget()
        self.plots_layout = QVBoxLayout(self.scroll_container)

        # load filters
        self.setup_load_filters_ui(self.plots_layout)

        # quick filters container (dynamic)
        # this group box is populated after data is loaded.
        self.quick_filters_group = QGroupBox("Quick Filters (In-Memory)")
        self.quick_filters_group.hide()  # hidden until data is loaded
        self.quick_filters_layout = QVBoxLayout(self.quick_filters_group)
        self.plots_layout.addWidget(self.quick_filters_group)

        # visualization options
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

        self.filters_check = QCheckBox("Filters")
        self.filters_check.setChecked(self.show_filters)
        self.filters_check.stateChanged.connect(self.on_visual_option_changed)
        vis_options_layout.addWidget(self.filters_check)

        self.plots_layout.addWidget(vis_options_group)

        self.setup_plots()

        self.scroll_area.setWidget(self.scroll_container)
        main_layout.addWidget(self.scroll_area, 1)

        # right sidebar for legend
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

        # precisions
        layout.addWidget(
            self.create_filter_group(
                "Precisions", metadata.precisions, "precisions", self.load_filters
            )
        )

        # series
        layout.addWidget(
            self.create_filter_group(
                "Series Names", metadata.series_names, "base_series", self.load_filters
            )
        )

        # accel
        layout.addWidget(
            self.create_filter_group(
                "Accel Names", metadata.accel_names, "base_accel", self.load_filters
            )
        )

        # m values
        layout.addWidget(
            self.create_filter_group(
                "M Values",
                [str(m) for m in metadata.m_values],
                "m_values",
                self.load_filters,
                is_int=True,
            )
        )

        # noise
        if metadata.noise_options:
            layout.addWidget(
                self.create_filter_group(
                    "Noise Options",
                    metadata.noise_options,
                    "noise_options",
                    self.load_filters,
                )
            )

        # series params
        for param, values in metadata.series_param_info.items():
            layout.addWidget(
                self.create_filter_group(
                    f"Series: {param}",
                    values,
                    ("series_params", param),
                    self.load_filters,
                )
            )

        # accel params
        for param, values in metadata.accel_param_info.items():
            layout.addWidget(
                self.create_filter_group(
                    f"Accel: {param}",
                    values,
                    ("accel_params", param),
                    self.load_filters,
                )
            )

        # refresh row
        refresh_layout = QHBoxLayout()
        self.refresh_btn = QPushButton("Reload data")
        self.refresh_btn.clicked.connect(self.refresh_data)
        refresh_layout.addWidget(self.refresh_btn)

        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 0)
        self.progress_bar.hide()
        refresh_layout.addWidget(self.progress_bar)
        layout.addLayout(refresh_layout)

        # divider
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

        # make buttons compact
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

            # determine if currently checked (state recovery)
            val = int(item) if is_int else item

            is_checked = False
            # handle nested keys for params
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
        # clear existing layout because options change when new data is loaded.
        while self.quick_filters_layout.count():
            item = self.quick_filters_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
            elif item.layout():
                pass

        # calculate available options based on CURRENT data.
        options = extract_filter_options(self.data)

        if (
            not any(options.values())
            and not options.get("series_params")
            and not options.get("accel_params")
        ):
            self.quick_filters_group.hide()
            return

        self.quick_filters_group.show()

        # helper to trigger update
        def on_change():
            self.apply_quick_filters_and_update()

        # precisions
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

        # series
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

        # accel
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

        # m values
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

        # noise
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

        # accel params
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

        # table
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
        # create a text item for the tooltip
        tooltip = pg.TextItem(
            text="",
            color=(255, 255, 255),
            anchor=(0, 1),
            border="w",
            fill=(0, 0, 0, 200),
        )
        tooltip.hide()

        # store reference in the widget for easy access
        plot_widget.custom_tooltip = tooltip

        def mouse_moved(evt):
            pos = evt[0]
            if plot_widget.sceneBoundingRect().contains(pos):
                mouse_point = plot_widget.plotItem.vb.mapSceneToView(pos)
                self.update_tooltip(plot_widget, mouse_point, pos)

        # rate limit is important to avoid lagging on mouse move.
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
        # return distance from p to segment vw

        l2 = (v.x() - w.x()) ** 2 + (v.y() - w.y()) ** 2
        if l2 == 0:
            return np.hypot(p.x() - v.x(), p.y() - v.y())

        # t = ((p - v) . (w - v)) / l2
        t = ((p.x() - v.x()) * (w.x() - v.x()) + (p.y() - v.y()) * (w.y() - v.y())) / l2
        t = max(0, min(1, t))

        # projection point
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
        closest_point = None
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
            candidate_point = None

            # helper to check a segment
            def check_segment(xs, ys, seg_name):
                # we need to find indices surrounding mouse_x
                # idx is where mouse_x should be inserted to maintain order
                idx = np.searchsorted(xs, mouse_x)

                local_min = float("inf")
                local_pt = None

                # we check the segment [idx-1, idx]
                # ensure indices are valid within this slice
                i_left = idx - 1
                i_right = idx

                if i_left >= 0 and i_right < len(xs):
                    # valid segment
                    p1 = vb.mapViewToDevice(pg.Point(xs[i_left], ys[i_left]))
                    p2 = vb.mapViewToDevice(pg.Point(xs[i_right], ys[i_right]))
                    if p1 and p2:
                        d = self.dist_to_segment(mouse_screen_pt, p1, p2)
                        if d < local_min:
                            local_min = d
                            # find closest point on this segment
                            local_pt = (
                                (xs[i_left], ys[i_left])
                                if np.abs(xs[i_left] - mouse_x)
                                < np.abs(xs[i_right] - mouse_x)
                                else (xs[i_right], ys[i_right])
                            )

                if i_right < len(xs):
                    p = vb.mapViewToDevice(pg.Point(xs[i_right], ys[i_right]))
                    if p:
                        d = np.hypot(
                            p.x() - mouse_screen_pt.x(), p.y() - mouse_screen_pt.y()
                        )
                        if d < local_min:
                            local_min = d
                            local_pt = (xs[i_right], ys[i_right])

                if i_left >= 0:
                    p = vb.mapViewToDevice(pg.Point(xs[i_left], ys[i_left]))
                    if p:
                        d = np.hypot(
                            p.x() - mouse_screen_pt.x(), p.y() - mouse_screen_pt.y()
                        )
                        if d < local_min:
                            local_min = d
                            local_pt = (xs[i_left], ys[i_left])

                return local_min, local_pt

            if hasattr(item, "point_names"):
                # scatter plot with named points (Performance Plot)
                view_range_x = vb.viewRange()[0]
                view_width = view_range_x[1] - view_range_x[0]

                # filter indices within reasonable X range (broad phase)
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
                            candidate_point = (x_data[i], y_data[i])

            elif hasattr(item, "segments"):
                # batched segments.
                # this is an optimization where multiple curves are drawn as one continuous line with gaps (NaNs).
                # we need to check which segment we are closest to.
                for start, end, info_name in item.segments:
                    seg_x = x_data[start:end]
                    seg_y = y_data[start:end]

                    # optimization: skip segments clearly out of X range
                    if len(seg_x) > 0:
                        if mouse_x < seg_x[0] - 1 or mouse_x > seg_x[-1] + 1:
                            continue

                    d, pt = check_segment(seg_x, seg_y, info_name)
                    if d < candidate_dist:
                        candidate_dist = d
                        candidate_name = info_name
                        candidate_point = pt
            else:
                # standard curve
                d, pt = check_segment(x_data, y_data, candidate_name)
                candidate_dist = d
                candidate_point = pt

            if candidate_dist < min_dist:
                min_dist = candidate_dist
                closest_curve = item
                closest_name = candidate_name
                closest_point = candidate_point

        tooltip = plot_widget.custom_tooltip
        if closest_curve and closest_point:
            x_val, y_val = closest_point

            # format y-coord respecting symlog
            if plot_widget.symlog:
                y_str = symlog_formatter(y_val)
            else:
                y_str = f"{y_val:.6e}"

            coord_str = f"n={int(x_val)}, y={y_str}"
            tooltip.setText(f"{closest_name}\n{coord_str}")
            tooltip.setPos(x_val, y_val)
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
        self.show_filters = self.filters_check.isChecked()
        self.update_plots()

    def on_symlog_changed(self, state):
        """
        Callback for symlog checkbox.
        """
        self.symlog = state == Qt.CheckState.Checked.value
        # update error_plot and performance_plot axis
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

        # setup quick filters for new data
        self.setup_quick_filters_ui()

        # apply initial filtering (using default empty filters = Show All)
        self.apply_quick_filters_and_update()

    def apply_quick_filters_and_update(self):
        """
        Applies in-memory filters and updates the visualization.
        """
        print("Applying Quick Filters...")
        t0 = time.time()
        self.filtered_data = filter_dataset(self.data, self.quick_filters)
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

    def update_plots(self):
        """
        Redraws all plots and populates the data table using pre-calculated visualization data.
        """
        t_start = time.time()

        # 1. prepare data using pure logic
        options = ViewOptions(
            show_partial_sums=self.show_partial_sums,
            show_limits=self.show_limits,
            show_real=self.show_real,
            show_imaginary=self.show_imaginary,
            force_show_imaginary=self.force_show_imaginary,
            show_filters=self.show_filters,
            symlog=self.symlog,
        )

        viz_data = prepare_viz_data(self.filtered_data, options)

        # 2. update UI
        self.convergence_plot.clear()
        self.error_plot.clear()
        self.performance_plot.clear()
        self.legend_list.clear()
        self.table.setRowCount(0)

        # re-add tooltips
        for plot in [self.convergence_plot, self.error_plot, self.performance_plot]:
            if hasattr(plot, "custom_tooltip"):
                plot.addItem(plot.custom_tooltip)
                plot.custom_tooltip.hide()

        # render curves
        def render_curves(plot, curves):
            for c in curves:
                curve_item = plot.plot(
                    c.x, c.y, pen=c.pen, name=c.name, connect="finite"
                )
                if c.segments:
                    curve_item.segments = c.segments

        render_curves(self.convergence_plot, viz_data.convergence_curves)
        render_curves(self.error_plot, viz_data.error_curves)

        # render performance
        if viz_data.performance_data:
            p = viz_data.performance_data
            perf_curve = self.performance_plot.plot(
                p.x, p.y, pen=None, symbol="o", symbolBrush=p.brushes, symbolSize=5
            )
            perf_curve.point_names = p.names

        # render table
        for row in viz_data.table_rows:
            r_idx = self.table.rowCount()
            self.table.insertRow(r_idx)

            self.table.setItem(r_idx, 0, QTableWidgetItem(row.series_id))
            self.table.setItem(r_idx, 1, QTableWidgetItem(row.name))
            self.table.setItem(r_idx, 2, QTableWidgetItem(row.precision))
            self.table.setItem(r_idx, 3, QTableWidgetItem(row.limit_str))
            self.table.setItem(r_idx, 4, QTableWidgetItem(row.series_params))
            self.table.setItem(r_idx, 5, QTableWidgetItem(row.accel_name))
            self.table.setItem(r_idx, 6, QTableWidgetItem(row.m_value))
            self.table.setItem(r_idx, 7, QTableWidgetItem(row.accel_params))
            self.table.setItem(r_idx, 8, QTableWidgetItem(row.noise_str))

            # collapsible
            col_configs = [
                (9, f"{len(row.series_values)} values", row.series_values),
                (10, f"{len(row.accel_values)} values", row.accel_values),
                (11, row.dev_summary, row.deviations),
                (12, f"{len(row.errors)} errors", row.errors),
                (13, f"{len(row.events)} events", row.events),
            ]

            for col_idx, title, lines in col_configs:
                if lines:
                    self.table.setCellWidget(
                        r_idx,
                        col_idx,
                        CollapsibleCellWidget(title, lines, self.table, r_idx),
                    )
                else:
                    self.table.setItem(r_idx, col_idx, QTableWidgetItem(""))

        # render legend
        for item in viz_data.legend_items:
            self.add_legend_item(item.text, item.color, item.is_dashed)

        print(f"UI update completed in {time.time() - t_start:.3f}s")


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
