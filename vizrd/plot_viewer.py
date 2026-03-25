import json
import sys

import matplotlib

matplotlib.use("qtagg")

from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg, NavigationToolbar2QT
from matplotlib.figure import Figure
import matplotlib

matplotlib.use("qtagg")
matplotlib.rcParams['savefig.dpi'] = 300

from matplotlib.backend_bases import ResizeEvent
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg, NavigationToolbar2QT
from matplotlib.figure import Figure
from matplotlib.ticker import (
    AutoLocator,
    FixedLocator,
    FuncFormatter,
    Locator,
    MultipleLocator,
)
from PyQt6 import QtCore, QtGui, QtWidgets

# Default axis labels
DEFAULT_X_LABEL = "Шаг n"
DEFAULT_Y_LABEL = "Значение x\u2099"  # x with subscript n (Unicode U+2099)


class CombinedLocator(Locator):
    """A locator that combines standard spacing with custom ticks."""

    def __init__(self, base_locator, custom_ticks):
        super().__init__()
        self.base_locator = base_locator
        self.custom_ticks = sorted(set(custom_ticks)) if custom_ticks else []

    def __call__(self):
        # Get ticks from base locator
        base_ticks = self.base_locator()

        # Combine with custom ticks
        all_ticks = list(base_ticks) + [
            t for t in self.custom_ticks if t not in base_ticks
        ]
        return sorted(set(all_ticks))

    def view_limits(self, vmin, vmax):
        return self.base_locator.view_limits(vmin, vmax)

    def set_axis(self, axis):
        super().set_axis(axis)
        self.base_locator.set_axis(axis)


import math


def format_value(val):
    """Format a value for display, avoiding scientific notation for small exponents."""
    if val == 0.0:
        return "0"
    if not math.isfinite(val):
        return str(val)

    abs_val = abs(val)
    log10 = math.log10(abs_val)
    exponent = int(math.floor(log10))

    # Avoid scientific notation for 10^-8 to 10^12
    if -7 <= exponent <= 7:
        s = f"{val:.18f}"
        if "." in s:
            s = s.rstrip("0").rstrip(".")
            if s == "" or s == "-":
                return "0"
        return s
    else:
        # Use scientific notation with high precision
        s = f"{val:.15e}"
        if "e" in s:
            mantissa, exp = s.split("e", 1)
            mantissa = mantissa.rstrip("0").rstrip(".")
            return f"{mantissa}e{exp}"
        return s


def format_grid_value(val):
    """Format a value for grid/axis labels with reduced precision."""
    if val == 0.0:
        return "0"
    if not math.isfinite(val):
        return str(val)

    abs_val = abs(val)
    log10 = math.log10(abs_val)
    exponent = int(math.floor(log10))

    if -5 <= exponent <= 5:
        s = f"{val:.5f}"
        if "." in s:
            s = s.rstrip("0").rstrip(".")
            if s == "" or s == "-":
                return "0"
        return s
    else:
        s = f"{val:.3e}"
        if "e" in s:
            mantissa, exp = s.split("e", 1)
            mantissa = mantissa.rstrip("0").rstrip(".")
            # Clean up e format
            if exp.startswith("-0"):
                exp = f"-{exp[2:]}"
            elif exp.startswith("0"):
                exp = exp[1:]
            elif exp.startswith("+0"):
                exp = exp[2:]
            elif exp.startswith("+"):
                exp = exp[1:]
            return f"{mantissa}e{exp}"
        return s


def symlog_formatter_factory(log_linthresh):
    """Create a formatter that converts symlog-transformed values back to original."""
    linthresh = 10**log_linthresh

    def formatter(y, pos):
        if y == 0.0:
            return "0"

        # Use the EXACT inverse: |x| = L * (10^|y| - 1)
        true_abs_x = linthresh * (10 ** abs(y) - 1.0)
        true_x = true_abs_x * (1.0 if y >= 0 else -1.0)

        return format_value(true_x)

    return formatter


def symlog_grid_formatter_factory(log_linthresh):
    """Create a grid formatter that converts symlog-transformed values back to original."""
    linthresh = 10**log_linthresh

    def formatter(y, pos):
        if y == 0.0:
            return "0"

        # Use the EXACT inverse: |x| = L * (10^|y| - 1)
        true_abs_x = linthresh * (10 ** abs(y) - 1.0)
        true_x = true_abs_x * (1.0 if y >= 0 else -1.0)

        return format_grid_value(true_x)

    return formatter


class PreviewCanvas(FigureCanvasQTAgg):
    def __init__(self, figure, target_w, target_h):
        self.target_width_inches = target_w
        self.target_height_inches = target_h
        self.base_dpi = 100.0
        self._in_resize_event = False
        super().__init__(figure)

    def set_target_size(self, w_inch, h_inch):
        self.target_width_inches = w_inch
        self.target_height_inches = h_inch
        self.resizeEvent(QtGui.QResizeEvent(self.size(), self.size()))

    def resizeEvent(self, event):
        if self._in_resize_event:
            return
        if self.figure is None:
            return
            
        self._in_resize_event = True
        try:
            w = event.size().width() * self.device_pixel_ratio
            h = event.size().height() * self.device_pixel_ratio

            if self.target_width_inches > 0 and self.target_height_inches > 0:
                dpi = w / self.target_width_inches
                self.figure.set_size_inches(self.target_width_inches, self.target_height_inches, forward=False)
                self.figure.set_dpi(dpi)
                
            QtWidgets.QWidget.resizeEvent(self, event)
            ResizeEvent("resize_event", self)._process()
            self.draw_idle()
        finally:
            self._in_resize_event = False


class PlotContainerWidget(QtWidgets.QWidget):
    def __init__(self, canvas):
        super().__init__()
        self.canvas = canvas
        self.layout = QtWidgets.QVBoxLayout(self)
        self.layout.setContentsMargins(0, 0, 0, 0)
        self.layout.addWidget(self.canvas, alignment=QtCore.Qt.AlignmentFlag.AlignCenter)

    def update_scaling(self):
        self.resizeEvent(QtGui.QResizeEvent(self.size(), self.size()))

    def resizeEvent(self, event):
        super().resizeEvent(event)

        available_w = event.size().width()
        available_h = event.size().height()

        if available_w <= 0 or available_h <= 0:
            return

        desired_w = self.canvas.target_width_inches * self.canvas.base_dpi
        desired_h = self.canvas.target_height_inches * self.canvas.base_dpi

        if desired_w <= 0 or desired_h <= 0:
            return

        if desired_w > available_w or desired_h > available_h:
            scale_w = available_w / desired_w
            scale_h = available_h / desired_h
            scale = min(scale_w, scale_h)
        else:
            scale = 1.0

        final_w = int(max(1, desired_w * scale))
        final_h = int(max(1, desired_h * scale))

        if self.canvas.size() != QtCore.QSize(final_w, final_h):
            self.canvas.setFixedSize(final_w, final_h)


class PlotWindow(QtWidgets.QMainWindow):
    def __init__(self, data):
        super().__init__()
        self.data = data
        self.setWindowTitle("Vizrd Plot Viewer")
        self.resize(1200, 800)

        # Main Widget and Layout
        main_widget = QtWidgets.QWidget()
        self.setCentralWidget(main_widget)
        main_layout = QtWidgets.QHBoxLayout(main_widget)

        # Left Panel (Scroll Area)
        self.scroll_area = QtWidgets.QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setFixedWidth(400)

        self.panel_widget = QtWidgets.QWidget()
        self.scroll_area.setWidget(self.panel_widget)
        self.panel_layout = QtWidgets.QVBoxLayout(self.panel_widget)

        # Right Panel (Plot)
        self.plot_container = QtWidgets.QWidget()
        self.plot_layout = QtWidgets.QVBoxLayout(self.plot_container)

        self.figure = Figure(constrained_layout=True, dpi=300)
        self.canvas = PreviewCanvas(self.figure, 10.0, 6.0)
        self.toolbar = NavigationToolbar2QT(self.canvas, self)

        self.preview_container = PlotContainerWidget(self.canvas)

        self.plot_layout.addWidget(self.toolbar)
        self.plot_layout.addWidget(self.preview_container, stretch=1)

        main_layout.addWidget(self.scroll_area)
        main_layout.addWidget(self.plot_container, stretch=1)

        self.ax = self.figure.add_subplot(111)
        self._updating_from_plot = False

        self.line_configs = []
        self.line_containers = []  # Store references to line UI containers
        self.lines_layout = None  # Will hold the QVBoxLayout for lines
        self.init_data()
        self.build_ui()
        self.canvas.mpl_connect("draw_event", self.on_canvas_draw)
        QtCore.QTimer.singleShot(0, self.update_plot)

    def init_data(self):
        # Read grid configuration
        grid_config = self.data.get("grid", {"type": "Normal"})
        self.grid_type = grid_config.get("type", "Normal")
        # log_linthresh is log10 of the actual threshold, so convert it
        self.log_linthresh = grid_config.get("log_linthresh", -10.0)
        self.linthresh = 10**self.log_linthresh

        # Read bounds
        bounds = self.data.get("bounds", {})
        self.bounds = {
            "x_min": bounds.get("x_min", 0.0),
            "x_max": bounds.get("x_max", 100.0),
            "y_min": bounds.get("y_min", -1.0),
            "y_max": bounds.get("y_max", 1.0),
        }
        self.default_bounds = dict(self.bounds)

        for line_data in self.data.get("lines", []):
            # y values are already transformed screen-space values
            y_vals = line_data["y"]
            x_vals = line_data["x"]

            color = [c / 255.0 for c in line_data["color"]]

            cfg = {
                "original_name": line_data["name"],
                "name": line_data["name"],
                "color": color,
                "width": line_data["width"],
                "style": line_data["style"],
                "marker": line_data.get("marker", "None"),
                "marker_size": line_data.get("marker_size", 6),
                "visible": line_data.get("visible", True),
                "x": x_vals,
                "y": y_vals,
            }
            self.line_configs.append(cfg)

        self.global_cfg = {
            "legend_visible": True,
            "legend_loc": "outside right",
            "font_size": 10,
            "legend_frame": True,
            "legend_title": "",
            "legend_title_fontsize": 12,
            "fig_width": 10.0,
            "fig_height": 6.0,
        }

        # Grid configuration with defaults
        self.grid_settings = {
            "x_step": 0,  # 0 = auto
            "y_step": 0,  # 0 = auto
            "x_ticks": [],
            "y_ticks": [],
            "x_label": DEFAULT_X_LABEL,
            "y_label": DEFAULT_Y_LABEL,
        }

        # Load config from JSON if present (new format)
        config = self.data.get("config", {})
        if config:
            try:
                # Load bounds
                config_bounds = config.get("bounds", {})
                if config_bounds and isinstance(config_bounds, dict):
                    self.bounds.update(config_bounds)
                
                # Load global config
                config_global = config.get("global_cfg", {})
                if config_global and isinstance(config_global, dict):
                    self.global_cfg.update(config_global)
                
                # Load grid settings
                config_grid = config.get("grid_settings", {})
                if config_grid and isinstance(config_grid, dict):
                    self.grid_settings.update(config_grid)
                
                # Load line configs
                config_lines = config.get("line_configs", [])
                if config_lines and isinstance(config_lines, list):
                    if len(config_lines) > len(self.line_configs):
                        print(f"Warning: Config contains {len(config_lines)} lines but data only has {len(self.line_configs)}")
                    for i, line_cfg in enumerate(config_lines):
                        if i < len(self.line_configs) and isinstance(line_cfg, dict):
                            # Update existing line config with saved values
                            self.line_configs[i]["name"] = line_cfg.get("name", self.line_configs[i]["name"])
                            self.line_configs[i]["original_name"] = line_cfg.get("original_name", self.line_configs[i]["original_name"])
                            # Color conversion: saved as 0-255, internal format is 0-1
                            if "color" in line_cfg:
                                self.line_configs[i]["color"] = [c / 255.0 for c in line_cfg["color"]]
                            self.line_configs[i]["width"] = line_cfg.get("width", self.line_configs[i]["width"])
                            self.line_configs[i]["style"] = line_cfg.get("style", self.line_configs[i]["style"])
                            self.line_configs[i]["marker"] = line_cfg.get("marker", self.line_configs[i]["marker"])
                            self.line_configs[i]["marker_size"] = line_cfg.get("marker_size", self.line_configs[i]["marker_size"])
                            self.line_configs[i]["visible"] = line_cfg.get("visible", self.line_configs[i]["visible"])
            except Exception as e:
                print(f"Warning: Failed to load config section: {e}. Using defaults.")

    def build_ui(self):
        # Global Settings
        global_group = QtWidgets.QGroupBox("Global Settings")
        global_layout = QtWidgets.QFormLayout(global_group)

        self.cb_legend = QtWidgets.QCheckBox("Show Legend")
        self.cb_legend.setChecked(self.global_cfg["legend_visible"])
        self.cb_legend.stateChanged.connect(self.on_global_changed)

        self.combo_legend_loc = QtWidgets.QComboBox()
        self.combo_legend_loc.addItems(
            [
                "best",
                "upper right",
                "upper left",
                "lower left",
                "lower right",
                "right",
                "center left",
                "center right",
                "lower center",
                "upper center",
                "center",
                "outside right",
                "outside left",
                "outside bottom",
            ]
        )
        self.combo_legend_loc.setCurrentText(self.global_cfg["legend_loc"])
        self.combo_legend_loc.currentTextChanged.connect(self.on_global_changed)

        # Save Config button
        self.btn_save_config = QtWidgets.QPushButton("Save Config")
        self.btn_save_config.clicked.connect(self.save_config)
        global_layout.addRow(self.btn_save_config)

        global_layout.addRow(self.cb_legend)
        global_layout.addRow("Legend Loc:", self.combo_legend_loc)

        # Font size (for axis labels, ticks, legend)
        self.spin_font_size = QtWidgets.QSpinBox()
        self.spin_font_size.setRange(4, 72)
        self.spin_font_size.setValue(self.global_cfg["font_size"])
        self.spin_font_size.valueChanged.connect(self.on_global_changed)
        global_layout.addRow("Font Size:", self.spin_font_size)

        # Legend frame
        self.cb_legend_frame = QtWidgets.QCheckBox("Show Frame")
        self.cb_legend_frame.setChecked(self.global_cfg["legend_frame"])
        self.cb_legend_frame.stateChanged.connect(self.on_global_changed)
        global_layout.addRow(self.cb_legend_frame)

        # Legend title
        self.edit_legend_title = QtWidgets.QLineEdit()
        self.edit_legend_title.setPlaceholderText("Optional title")
        self.edit_legend_title.setText(self.global_cfg["legend_title"])
        self.edit_legend_title.textChanged.connect(self.on_global_changed)
        global_layout.addRow("Legend Title:", self.edit_legend_title)

        # Legend title font size
        self.spin_legend_title_fontsize = QtWidgets.QSpinBox()
        self.spin_legend_title_fontsize.setRange(4, 72)
        self.spin_legend_title_fontsize.setValue(
            self.global_cfg["legend_title_fontsize"]
        )
        self.spin_legend_title_fontsize.valueChanged.connect(self.on_global_changed)
        global_layout.addRow("Title Font Size:", self.spin_legend_title_fontsize)

        # Figure Size controls
        self.spin_fig_width = QtWidgets.QDoubleSpinBox()
        self.spin_fig_width.setRange(1.0, 50.0)
        self.spin_fig_width.setSingleStep(0.5)
        self.spin_fig_width.setValue(self.global_cfg["fig_width"])
        self.spin_fig_width.valueChanged.connect(self.on_global_changed)
        global_layout.addRow("Fig Width (in):", self.spin_fig_width)

        self.spin_fig_height = QtWidgets.QDoubleSpinBox()
        self.spin_fig_height.setRange(1.0, 50.0)
        self.spin_fig_height.setSingleStep(0.5)
        self.spin_fig_height.setValue(self.global_cfg["fig_height"])
        self.spin_fig_height.valueChanged.connect(self.on_global_changed)
        global_layout.addRow("Fig Height (in):", self.spin_fig_height)

        # Show grid info
        grid_info = QtWidgets.QLabel(f"Grid: {self.grid_type}")
        if self.grid_type == "Symlog":
            grid_info.setText(
                f"Grid: {self.grid_type} (log_linthresh={self.log_linthresh})"
            )
        global_layout.addRow(grid_info)

        self.panel_layout.addWidget(global_group)

        # Region of Interest (ROI) Settings
        roi_group = QtWidgets.QGroupBox("Region of Interest (ROI)")
        roi_layout = QtWidgets.QFormLayout(roi_group)

        # X Min
        self.spin_x_min = QtWidgets.QDoubleSpinBox()
        self.spin_x_min.setRange(-1e308, 1e308)
        self.spin_x_min.setDecimals(10)
        self.spin_x_min.setValue(self.bounds["x_min"])
        self.spin_x_min.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("X Min:", self.spin_x_min)

        # X Max
        self.spin_x_max = QtWidgets.QDoubleSpinBox()
        self.spin_x_max.setRange(-1e308, 1e308)
        self.spin_x_max.setDecimals(10)
        self.spin_x_max.setValue(self.bounds["x_max"])
        self.spin_x_max.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("X Max:", self.spin_x_max)

        # Y Min
        self.spin_y_min = QtWidgets.QDoubleSpinBox()
        self.spin_y_min.setRange(-1e308, 1e308)
        self.spin_y_min.setDecimals(10)
        self.spin_y_min.setValue(self.bounds["y_min"])
        self.spin_y_min.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("Y Min:", self.spin_y_min)

        # Y Max
        self.spin_y_max = QtWidgets.QDoubleSpinBox()
        self.spin_y_max.setRange(-1e308, 1e308)
        self.spin_y_max.setDecimals(10)
        self.spin_y_max.setValue(self.bounds["y_max"])
        self.spin_y_max.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("Y Max:", self.spin_y_max)

        # Set initial values from config if loaded (bounds always exists at this point)
        if self.bounds:
            self.spin_x_min.setValue(self.bounds["x_min"])
            self.spin_x_max.setValue(self.bounds["x_max"])
            self.spin_y_min.setValue(self.bounds["y_min"])
            self.spin_y_max.setValue(self.bounds["y_max"])

        self.btn_reset_roi = QtWidgets.QPushButton("Reset ROI to Default")
        self.btn_reset_roi.clicked.connect(self.on_reset_roi)
        roi_layout.addRow(self.btn_reset_roi)

        self.panel_layout.addWidget(roi_group)

        # Grid Settings
        grid_group = QtWidgets.QGroupBox("Grid Configuration")
        grid_layout = QtWidgets.QFormLayout(grid_group)

        # X Step
        self.spin_x_step = QtWidgets.QDoubleSpinBox()
        self.spin_x_step.setRange(0, 1e308)
        self.spin_x_step.setDecimals(10)
        self.spin_x_step.setValue(self.grid_settings.get("x_step", 0))
        self.spin_x_step.setSpecialValueText("Auto")
        self.spin_x_step.valueChanged.connect(self.on_grid_changed)
        grid_layout.addRow("X Step:", self.spin_x_step)

        # Y Step
        self.spin_y_step = QtWidgets.QDoubleSpinBox()
        self.spin_y_step.setRange(0, 1e308)
        self.spin_y_step.setDecimals(10)
        self.spin_y_step.setValue(self.grid_settings.get("y_step", 0))
        self.spin_y_step.setSpecialValueText("Auto")
        self.spin_y_step.valueChanged.connect(self.on_grid_changed)
        grid_layout.addRow("Y Step:", self.spin_y_step)

        # Custom X Ticks
        self.edit_x_ticks = QtWidgets.QLineEdit()
        x_ticks = self.grid_settings.get("x_ticks", [])
        self.edit_x_ticks.setText(", ".join(map(str, x_ticks)) if x_ticks else "")
        self.edit_x_ticks.setPlaceholderText("e.g. 1, 2.5, 5, 10")
        self.edit_x_ticks.textChanged.connect(self.on_grid_changed)
        grid_layout.addRow("Custom X Ticks:", self.edit_x_ticks)

        # Custom Y Ticks
        self.edit_y_ticks = QtWidgets.QLineEdit()
        y_ticks = self.grid_settings.get("y_ticks", [])
        self.edit_y_ticks.setText(", ".join(map(str, y_ticks)) if y_ticks else "")
        self.edit_y_ticks.setPlaceholderText("e.g. -1, 0, 1, 2")
        self.edit_y_ticks.textChanged.connect(self.on_grid_changed)
        grid_layout.addRow("Custom Y Ticks:", self.edit_y_ticks)

        # X Axis Label
        self.edit_x_label = QtWidgets.QLineEdit()
        self.edit_x_label.setText(self.grid_settings.get("x_label", DEFAULT_X_LABEL))
        self.edit_x_label.textChanged.connect(self.on_grid_changed)
        grid_layout.addRow("X Axis Label:", self.edit_x_label)

        # Y Axis Label
        self.edit_y_label = QtWidgets.QLineEdit()
        self.edit_y_label.setText(self.grid_settings.get("y_label", DEFAULT_Y_LABEL))
        self.edit_y_label.textChanged.connect(self.on_grid_changed)
        grid_layout.addRow("Y Axis Label:", self.edit_y_label)

        # Reset button
        self.btn_reset_grid = QtWidgets.QPushButton("Reset to Auto")
        self.btn_reset_grid.clicked.connect(self.on_reset_grid)
        grid_layout.addRow(self.btn_reset_grid)

        self.panel_layout.addWidget(grid_group)

        # Lines Settings
        lines_group = QtWidgets.QGroupBox("Line Configurations")
        self.lines_layout = QtWidgets.QVBoxLayout(lines_group)
        self.lines_layout.setSpacing(4)

        for idx, cfg in enumerate(self.line_configs):
            # Main container for each line
            line_container = QtWidgets.QFrame()
            line_container.setFrameShape(QtWidgets.QFrame.Shape.StyledPanel)
            line_container.setStyleSheet(
                "QFrame { border: 1px solid #666; border-radius: 3px; }"
            )
            line_container.setProperty("line_id", idx)
            line_main_layout = QtWidgets.QVBoxLayout(line_container)
            line_main_layout.setContentsMargins(4, 4, 4, 4)
            line_main_layout.setSpacing(2)

            # Compact header row: [>] [x] [color] [name] [up/down]
            header_layout = QtWidgets.QHBoxLayout()
            header_layout.setSpacing(4)

            # Expand/collapse button
            btn_expand = QtWidgets.QPushButton(">")
            btn_expand.setFixedSize(24, 24)
            btn_expand.setCheckable(True)
            btn_expand.setChecked(False)
            btn_expand.setToolTip("Show/Hide details")
            btn_expand.setStyleSheet(
                "QPushButton { font-weight: bold; font-size: 10px; }"
            )

            # Visibility checkbox (compact)
            cb_vis = QtWidgets.QCheckBox()
            cb_vis.setChecked(cfg["visible"])
            cb_vis.setToolTip("Visible")
            cb_vis.setFixedSize(20, 20)
            cb_vis.stateChanged.connect(
                lambda state, c=line_container: self.update_line_cfg(
                    c.property("line_id"), "visible", bool(state)
                )
            )

            # Color selector (compact square button)
            btn_color = QtWidgets.QPushButton()
            btn_color.setFixedSize(24, 24)
            btn_color.setToolTip("Click to change color")
            btn_color.setFlat(True)
            r, g, b, a = cfg["color"]
            btn_color.setStyleSheet(f"""
                QPushButton {{
                    background-color: rgb({int(r * 255)},{int(g * 255)},{int(b * 255)});
                    border: 2px solid #666;
                    border-radius: 3px;
                }}
                QPushButton:hover {{
                    border: 2px solid #000;
                }}
                QPushButton:pressed {{
                    background-color: rgb({int(r * 255)},{int(g * 255)},{int(b * 255)});
                }}
            """)
            btn_color.clicked.connect(
                lambda _, c=line_container, btn=btn_color: self.pick_color(
                    c.property("line_id"), btn
                )
            )

            # Name field (no label)
            edit_name = QtWidgets.QLineEdit(cfg["name"])
            edit_name.setPlaceholderText("Line name")
            edit_name.textChanged.connect(
                lambda text, c=line_container: self.update_line_cfg(
                    c.property("line_id"), "name", text
                )
            )

            # Up/Down buttons container
            move_buttons_layout = QtWidgets.QVBoxLayout()
            move_buttons_layout.setSpacing(0)
            move_buttons_layout.setContentsMargins(0, 0, 0, 0)

            # Up button
            btn_up = QtWidgets.QPushButton("▲")
            btn_up.setFixedSize(24, 12)
            btn_up.setToolTip("Move up")
            btn_up.setStyleSheet("QPushButton { font-size: 8px; padding: 0px; }")
            btn_up.clicked.connect(self.move_line_up)

            # Down button
            btn_down = QtWidgets.QPushButton("▼")
            btn_down.setFixedSize(24, 12)
            btn_down.setToolTip("Move down")
            btn_down.setStyleSheet("QPushButton { font-size: 8px; padding: 0px; }")
            btn_down.clicked.connect(self.move_line_down)

            move_buttons_layout.addWidget(btn_up)
            move_buttons_layout.addWidget(btn_down)

            header_layout.addWidget(btn_expand)
            header_layout.addWidget(cb_vis)
            header_layout.addWidget(btn_color)
            header_layout.addWidget(edit_name, stretch=1)
            header_layout.addLayout(move_buttons_layout)

            line_main_layout.addLayout(header_layout)

            # Collapsible details container
            details_widget = QtWidgets.QWidget()
            details_widget.setVisible(False)  # Hidden by default
            details_layout = QtWidgets.QFormLayout(details_widget)
            details_layout.setContentsMargins(0, 0, 0, 0)
            details_layout.setSpacing(4)

            # Width
            spin_width = QtWidgets.QDoubleSpinBox()
            spin_width.setRange(0.0, 20.0)
            spin_width.setSingleStep(0.5)
            spin_width.setValue(cfg["width"])
            spin_width.valueChanged.connect(
                lambda val, c=line_container: self.update_line_cfg(
                    c.property("line_id"), "width", val
                )
            )
            details_layout.addRow("Width:", spin_width)

            # Style
            combo_style = QtWidgets.QComboBox()
            combo_style.addItems(["Solid", "Dashed", "Dotted", "Dash-Dot", "None"])
            combo_style.setCurrentText(cfg["style"])
            combo_style.currentTextChanged.connect(
                lambda text, c=line_container: self.update_line_cfg(
                    c.property("line_id"), "style", text
                )
            )
            details_layout.addRow("Style:", combo_style)

            # Marker row with size
            marker_row = QtWidgets.QHBoxLayout()
            combo_marker = QtWidgets.QComboBox()
            combo_marker.addItems(
                ["None", "Circle", "Square", "Triangle", "Cross", "Star", "Plus"]
            )
            combo_marker.setCurrentText(cfg.get("marker", "None"))
            combo_marker.currentTextChanged.connect(
                lambda text, c=line_container: self.update_line_cfg(
                    c.property("line_id"), "marker", text
                )
            )

            spin_marker_size = QtWidgets.QSpinBox()
            spin_marker_size.setRange(1, 50)
            spin_marker_size.setValue(cfg.get("marker_size", 6))
            spin_marker_size.setFixedWidth(50)
            spin_marker_size.valueChanged.connect(
                lambda val, c=line_container: self.update_line_cfg(
                    c.property("line_id"), "marker_size", val
                )
            )

            marker_row.addWidget(combo_marker, stretch=1)
            marker_row.addWidget(spin_marker_size)
            details_layout.addRow("Marker:", marker_row)

            line_main_layout.addWidget(details_widget)

            # Toggle visibility of details
            btn_expand.toggled.connect(
                lambda checked, dw=details_widget, btn=btn_expand: (
                    dw.setVisible(checked),
                    btn.setText("▼" if checked else ">"),
                )
            )

            self.lines_layout.addWidget(line_container)
            self.line_containers.append(line_container)

        self.panel_layout.addWidget(lines_group)
        self.panel_layout.addStretch()

    def on_global_changed(self, *_):
        self.global_cfg["legend_visible"] = self.cb_legend.isChecked()
        self.global_cfg["legend_loc"] = self.combo_legend_loc.currentText()
        self.global_cfg["font_size"] = self.spin_font_size.value()
        self.global_cfg["legend_frame"] = self.cb_legend_frame.isChecked()
        self.global_cfg["legend_title"] = self.edit_legend_title.text()
        self.global_cfg["legend_title_fontsize"] = (
            self.spin_legend_title_fontsize.value()
        )
        
        old_w = self.global_cfg.get("fig_width")
        old_h = self.global_cfg.get("fig_height")
        new_w = self.spin_fig_width.value()
        new_h = self.spin_fig_height.value()

        self.global_cfg["fig_width"] = new_w
        self.global_cfg["fig_height"] = new_h

        if old_w != new_w or old_h != new_h:
            self.canvas.set_target_size(new_w, new_h)
            self.preview_container.update_scaling()

        self.update_plot()

    def on_roi_changed(self, *_):
        self.bounds["x_min"] = self.spin_x_min.value()
        self.bounds["x_max"] = self.spin_x_max.value()
        self.bounds["y_min"] = self.spin_y_min.value()
        self.bounds["y_max"] = self.spin_y_max.value()
        self.update_plot()

    def on_reset_roi(self):
        self.bounds = dict(self.default_bounds)
        self.spin_x_min.blockSignals(True)
        self.spin_x_max.blockSignals(True)
        self.spin_y_min.blockSignals(True)
        self.spin_y_max.blockSignals(True)
        self.spin_x_min.setValue(self.bounds["x_min"])
        self.spin_x_max.setValue(self.bounds["x_max"])
        self.spin_y_min.setValue(self.bounds["y_min"])
        self.spin_y_max.setValue(self.bounds["y_max"])
        self.spin_x_min.blockSignals(False)
        self.spin_x_max.blockSignals(False)
        self.spin_y_min.blockSignals(False)
        self.spin_y_max.blockSignals(False)
        self.update_plot()

    def on_canvas_draw(self, event):
        if self._updating_from_plot:
            return
        xlim = self.ax.get_xlim()
        ylim = self.ax.get_ylim()
        if (
            abs(self.bounds["x_min"] - xlim[0]) < 1e-12
            and abs(self.bounds["x_max"] - xlim[1]) < 1e-12
            and abs(self.bounds["y_min"] - ylim[0]) < 1e-12
            and abs(self.bounds["y_max"] - ylim[1]) < 1e-12
        ):
            return
        self._updating_from_plot = True
        self.spin_x_min.blockSignals(True)
        self.spin_x_max.blockSignals(True)
        self.spin_y_min.blockSignals(True)
        self.spin_y_max.blockSignals(True)
        self.spin_x_min.setValue(xlim[0])
        self.spin_x_max.setValue(xlim[1])
        self.spin_y_min.setValue(ylim[0])
        self.spin_y_max.setValue(ylim[1])
        self.bounds["x_min"] = xlim[0]
        self.bounds["x_max"] = xlim[1]
        self.bounds["y_min"] = ylim[0]
        self.bounds["y_max"] = ylim[1]
        self.spin_x_min.blockSignals(False)
        self.spin_x_max.blockSignals(False)
        self.spin_y_min.blockSignals(False)
        self.spin_y_max.blockSignals(False)
        self._updating_from_plot = False

    def on_grid_changed(self, *_):
        self.grid_settings = {
            "x_step": self.spin_x_step.value(),
            "y_step": self.spin_y_step.value(),
            "x_ticks": self.parse_ticks(self.edit_x_ticks.text()),
            "y_ticks": self.parse_ticks(self.edit_y_ticks.text()),
            "x_label": self.edit_x_label.text() or DEFAULT_X_LABEL,
            "y_label": self.edit_y_label.text() or DEFAULT_Y_LABEL,
        }
        self.update_plot()

    def on_reset_grid(self):
        self.spin_x_step.setValue(0)
        self.spin_y_step.setValue(0)
        self.edit_x_ticks.clear()
        self.edit_y_ticks.clear()
        self.edit_x_label.setText(DEFAULT_X_LABEL)
        self.edit_y_label.setText(DEFAULT_Y_LABEL)
        self.grid_settings = {
            "x_step": 0,
            "y_step": 0,
            "x_ticks": [],
            "y_ticks": [],
            "x_label": "Шаг n",
            "y_label": "Значение x\u2099",
        }
        self.update_plot()

    def parse_ticks(self, text):
        """Parse comma-separated tick values"""
        if not text.strip():
            return []
        try:
            return [float(x.strip()) for x in text.split(",") if x.strip()]
        except ValueError:
            return []

    def update_line_cfg(self, idx, key, val):
        self.line_configs[idx][key] = val
        if key in [
            "style",
            "marker",
            "marker_size",
            "width",
            "name",
            "visible",
            "color",
        ]:
            self.update_plot()

    def pick_color(self, idx, btn):
        curr = self.line_configs[idx]["color"]
        initial = QtGui.QColor(
            int(curr[0] * 255),
            int(curr[1] * 255),
            int(curr[2] * 255),
            int(curr[3] * 255),
        )
        color = QtWidgets.QColorDialog.getColor(
            initial,
            self,
            "Pick Color",
            QtWidgets.QColorDialog.ColorDialogOption.ShowAlphaChannel,
        )
        if color.isValid():
            r, g, b, a = color.getRgbF()
            self.line_configs[idx]["color"] = [r, g, b, a]
            btn.setStyleSheet(f"""
                QPushButton {{
                    background-color: rgb({int(r * 255)},{int(g * 255)},{int(b * 255)});
                    border: 2px solid #666;
                    border-radius: 3px;
                }}
                QPushButton:hover {{
                    border: 2px solid #000;
                }}
                QPushButton:pressed {{
                    background-color: rgb({int(r * 255)},{int(g * 255)},{int(b * 255)});
                }}
            """)
            self.update_plot()

    def move_line_up(self):
        """Move the line whose up button was clicked one position up."""
        btn = self.sender()
        if not btn:
            return

        # Find the line container that contains this button
        container = btn.parentWidget()
        while container and not isinstance(container, QtWidgets.QFrame):
            container = container.parentWidget()

        if not container:
            return

        idx = container.property("line_id")
        if idx is None or idx <= 0:
            return

        # Swap configs
        self.line_configs[idx - 1], self.line_configs[idx] = (
            self.line_configs[idx],
            self.line_configs[idx - 1],
        )

        # Swap widgets in layout
        self.lines_layout.removeWidget(self.line_containers[idx - 1])
        self.lines_layout.removeWidget(self.line_containers[idx])
        self.lines_layout.insertWidget(idx - 1, self.line_containers[idx])
        self.lines_layout.insertWidget(idx, self.line_containers[idx - 1])

        # Update container references
        self.line_containers[idx - 1], self.line_containers[idx] = (
            self.line_containers[idx],
            self.line_containers[idx - 1],
        )

        # Update line_id properties
        self.line_containers[idx - 1].setProperty("line_id", idx - 1)
        self.line_containers[idx].setProperty("line_id", idx)

        self._update_move_buttons()
        self.update_plot()

    def move_line_down(self):
        """Move the line whose down button was clicked one position down."""
        btn = self.sender()
        if not btn:
            return

        # Find the line container that contains this button
        container = btn.parentWidget()
        while container and not isinstance(container, QtWidgets.QFrame):
            container = container.parentWidget()

        if not container:
            return

        idx = container.property("line_id")
        if idx is None or idx >= len(self.line_configs) - 1:
            return

        # Swap configs
        self.line_configs[idx], self.line_configs[idx + 1] = (
            self.line_configs[idx + 1],
            self.line_configs[idx],
        )

        # Swap widgets in layout
        self.lines_layout.removeWidget(self.line_containers[idx])
        self.lines_layout.removeWidget(self.line_containers[idx + 1])
        self.lines_layout.insertWidget(idx, self.line_containers[idx + 1])
        self.lines_layout.insertWidget(idx + 1, self.line_containers[idx])

        # Update container references
        self.line_containers[idx], self.line_containers[idx + 1] = (
            self.line_containers[idx + 1],
            self.line_containers[idx],
        )

        # Update line_id properties
        self.line_containers[idx].setProperty("line_id", idx)
        self.line_containers[idx + 1].setProperty("line_id", idx + 1)

        self._update_move_buttons()
        self.update_plot()

    def _update_move_buttons(self):
        """Enable/disable up/down buttons based on line positions."""
        for i, container in enumerate(self.line_containers):
            # Find the up/down buttons in the container
            for child in container.findChildren(QtWidgets.QPushButton):
                if child.text() == "▲":
                    child.setEnabled(i > 0)
                elif child.text() == "▼":
                    child.setEnabled(i < len(self.line_containers) - 1)

    def update_plot(self):
        self.ax.clear()

        style_map = {
            "Solid": "-",
            "Dashed": "--",
            "Dotted": ":",
            "Dash-Dot": "-.",
            "None": "",
        }

        marker_map = {
            "None": "",
            "Circle": "o",
            "Square": "s",
            "Triangle": "^",
            "Cross": "x",
            "Star": "*",
            "Plus": "+",
        }

        has_data = False
        for cfg in self.line_configs:
            if not cfg["visible"]:
                continue
            has_data = True
            ls = style_map.get(cfg["style"], "-")
            mk = marker_map.get(cfg["marker"], "")
            marker_size = cfg.get("marker_size", 6)
            self.ax.plot(
                cfg["x"],
                cfg["y"],
                label=cfg["name"],
                color=cfg["color"],
                linewidth=cfg["width"],
                linestyle=ls,
                marker=mk,
                markersize=marker_size,
            )

        font_size = self.global_cfg["font_size"]
        self.ax.set_xlabel(
            self.grid_settings.get("x_label", DEFAULT_X_LABEL), fontsize=font_size
        )
        self.ax.set_ylabel(
            self.grid_settings.get("y_label", DEFAULT_Y_LABEL), fontsize=font_size
        )

        # Data is already symlog-transformed, so always use linear scale
        self.ax.set_yscale("linear")

        # Set custom formatter to display original values
        if self.grid_type == "Symlog":
            self.ax.yaxis.set_major_formatter(
                FuncFormatter(symlog_grid_formatter_factory(self.log_linthresh))
            )
        else:
            self.ax.yaxis.set_major_formatter(
                FuncFormatter(lambda y, pos: format_grid_value(y))
            )

        # Set initial bounds from exported data
        self._updating_from_plot = True
        self.ax.set_xlim(self.bounds["x_min"], self.bounds["x_max"])
        self.ax.set_ylim(self.bounds["y_min"], self.bounds["y_max"])
        self._updating_from_plot = False

        # Configure grid and tick locators (MUST be after set_yscale!)
        self.ax.grid(True, which="both", linestyle="--", alpha=0.5)

        # X-axis tick configuration - combine standard spacing with custom ticks
        if self.grid_settings["x_step"] > 0:
            x_base = MultipleLocator(self.grid_settings["x_step"])
        else:
            x_base = AutoLocator()

        if self.grid_settings["x_ticks"]:
            x_locator = CombinedLocator(x_base, self.grid_settings["x_ticks"])
        else:
            x_locator = x_base
        self.ax.xaxis.set_major_locator(x_locator)
        self.ax.tick_params(axis="both", labelsize=font_size)

        # Y-axis tick configuration - combine standard spacing with custom ticks
        if self.grid_settings["y_step"] > 0:
            y_base = MultipleLocator(self.grid_settings["y_step"])
        else:
            y_base = AutoLocator()

        if self.grid_settings["y_ticks"]:
            y_locator = CombinedLocator(y_base, self.grid_settings["y_ticks"])
        else:
            y_locator = y_base
        self.ax.yaxis.set_major_locator(y_locator)

        if has_data and self.global_cfg["legend_visible"]:
            loc = self.global_cfg["legend_loc"]
            legend_kwargs = {
                "fontsize": font_size,
                "frameon": self.global_cfg["legend_frame"],
            }
            title = self.global_cfg["legend_title"]
            if title:
                legend_kwargs["title"] = title
                legend_kwargs["title_fontsize"] = self.global_cfg[
                    "legend_title_fontsize"
                ]

            if loc == "outside right":
                legend_kwargs.update(
                    {
                        "loc": "upper left",
                        "bbox_to_anchor": (1.02, 1),
                        "borderaxespad": 0,
                    }
                )
                self.ax.legend(**legend_kwargs)
            elif loc == "outside left":
                legend_kwargs.update(
                    {
                        "loc": "upper right",
                        "bbox_to_anchor": (-0.02, 1),
                        "borderaxespad": 0,
                    }
                )
                self.ax.legend(**legend_kwargs)
            elif loc == "outside bottom":
                legend_kwargs.update(
                    {
                        "loc": "upper center",
                        "bbox_to_anchor": (0.5, -0.1),
                        "borderaxespad": 0,
                        "ncol": 2,
                    }
                )
                self.ax.legend(**legend_kwargs)
            else:
                legend_kwargs["loc"] = loc
                self.ax.legend(**legend_kwargs)

        self.canvas.draw()

    def save_config(self):
        """Save current plot configuration to JSON file."""
        path, _ = QtWidgets.QFileDialog.getSaveFileName(
            self, "Save Config", "", "JSON Files (*.json)"
        )
        
        if not path:
            return
        
        # Ensure .json extension
        if not path.endswith(".json"):
            path += ".json"
        
        # Build config dictionary
        config_data = {
            "grid": {
                "type": self.grid_type,
                "log_linthresh": self.log_linthresh
            },
            "bounds": self.bounds,
            "lines": [],
            "config": {
                "bounds": self.bounds,
                "global_cfg": self.global_cfg,
                "grid_settings": self.grid_settings,
                "line_configs": []
            }
        }
        
        # Add line data and configs
        for cfg in self.line_configs:
            # Original line data
            line_data = {
                "name": cfg["original_name"],
                "x": cfg["x"],
                "y": cfg["y"],
                "color": [int(c * 255) for c in cfg["color"]],
                "width": cfg["width"],
                "style": cfg["style"],
                "marker": cfg["marker"],
                "marker_size": cfg["marker_size"],
                "visible": cfg["visible"]
            }
            config_data["lines"].append(line_data)
            
            # Config for this line (without x/y data)
            line_config = {
                "name": cfg["name"],
                "original_name": cfg["original_name"],
                "color": [int(c * 255) for c in cfg["color"]],
                "width": cfg["width"],
                "style": cfg["style"],
                "marker": cfg["marker"],
                "marker_size": cfg["marker_size"],
                "visible": cfg["visible"]
            }
            config_data["config"]["line_configs"].append(line_config)
        
        try:
            with open(path, "w", encoding="utf-8") as f:
                json.dump(config_data, f, indent=2, ensure_ascii=False)
            print(f"Configuration saved to: {path}")
        except Exception as e:
            print(f"Failed to save configuration: {e}")
            QtWidgets.QMessageBox.critical(self, "Error", f"Failed to save configuration:\n{str(e)}")


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)

    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        path, _ = QtWidgets.QFileDialog.getOpenFileName(
            None, "Select series_data.json", "", "JSON Files (*.json)"
        )

    if path:
        try:
            with open(path, "r") as f:
                data = json.load(f)
            window = PlotWindow(data)
            window.show()
            sys.exit(app.exec())
        except Exception as e:
            print(f"Failed to load or display data: {e}")
    else:
        print("No file selected.")
