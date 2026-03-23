import sys
import json
import matplotlib

matplotlib.use('qtagg')

from PyQt6 import QtWidgets, QtCore, QtGui
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg, NavigationToolbar2QT
from matplotlib.figure import Figure
from matplotlib.ticker import FuncFormatter, MultipleLocator, FixedLocator, AutoLocator, Locator


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
        all_ticks = list(base_ticks) + [t for t in self.custom_ticks if t not in base_ticks]
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
        if '.' in s:
            s = s.rstrip('0').rstrip('.')
            if s == '' or s == '-':
                return "0"
        return s
    else:
        # Use scientific notation with high precision
        s = f"{val:.15e}"
        if 'e' in s:
            mantissa, exp = s.split('e', 1)
            mantissa = mantissa.rstrip('0').rstrip('.')
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
        if '.' in s:
            s = s.rstrip('0').rstrip('.')
            if s == '' or s == '-':
                return "0"
        return s
    else:
        s = f"{val:.3e}"
        if 'e' in s:
            mantissa, exp = s.split('e', 1)
            mantissa = mantissa.rstrip('0').rstrip('.')
            # Clean up e format
            if exp.startswith('-0'):
                exp = f"-{exp[2:]}"
            elif exp.startswith('0'):
                exp = exp[1:]
            elif exp.startswith('+0'):
                exp = exp[2:]
            elif exp.startswith('+'):
                exp = exp[1:]
            return f"{mantissa}e{exp}"
        return s

def symlog_formatter_factory(log_linthresh):
    """Create a formatter that converts symlog-transformed values back to original."""
    linthresh = 10 ** log_linthresh
    
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
    linthresh = 10 ** log_linthresh
    
    def formatter(y, pos):
        if y == 0.0:
            return "0"
        
        # Use the EXACT inverse: |x| = L * (10^|y| - 1)
        true_abs_x = linthresh * (10 ** abs(y) - 1.0)
        true_x = true_abs_x * (1.0 if y >= 0 else -1.0)
        
        return format_grid_value(true_x)
    
    return formatter

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
        
        self.figure = Figure(constrained_layout=True)
        self.canvas = FigureCanvasQTAgg(self.figure)
        self.toolbar = NavigationToolbar2QT(self.canvas, self)
        
        self.plot_layout.addWidget(self.toolbar)
        self.plot_layout.addWidget(self.canvas)

        main_layout.addWidget(self.scroll_area)
        main_layout.addWidget(self.plot_container, stretch=1)

        self.ax = self.figure.add_subplot(111)

        self.line_configs = []
        self.init_data()
        self.build_ui()
        # Defer initial plot to let Qt layout settle first
        QtCore.QTimer.singleShot(0, self.update_plot)

    def init_data(self):
        # Read grid configuration
        grid_config = self.data.get('grid', {'type': 'Normal'})
        self.grid_type = grid_config.get('type', 'Normal')
        # log_linthresh is log10 of the actual threshold, so convert it
        self.log_linthresh = grid_config.get('log_linthresh', -10.0)
        self.linthresh = 10 ** self.log_linthresh

        # Read bounds
        bounds = self.data.get('bounds', {})
        self.bounds = {
            'x_min': bounds.get('x_min', 0.0),
            'x_max': bounds.get('x_max', 100.0),
            'y_min': bounds.get('y_min', -1.0),
            'y_max': bounds.get('y_max', 1.0)
        }

        for line_data in self.data.get('lines', []):
            # y values are already transformed screen-space values
            y_vals = line_data['y']
            x_vals = line_data['x']
            
            color = [c/255.0 for c in line_data['color']]
            
            cfg = {
                'original_name': line_data['name'],
                'name': line_data['name'],
                'color': color,
                'width': line_data['width'],
                'style': line_data['style'],
                'marker': 'None',
                'visible': True,
                'x': x_vals,
                'y': y_vals
            }
            self.line_configs.append(cfg)

        self.global_cfg = {
            'legend_visible': True,
            'legend_loc': 'outside right',
        }

        # Grid configuration with defaults
        self.grid_settings = {
            'x_step': 0,  # 0 = auto
            'y_step': 0,  # 0 = auto
            'x_ticks': [],
            'y_ticks': []
        }

    def build_ui(self):
        # Global Settings
        global_group = QtWidgets.QGroupBox("Global Settings")
        global_layout = QtWidgets.QFormLayout(global_group)

        self.cb_legend = QtWidgets.QCheckBox("Show Legend")
        self.cb_legend.setChecked(self.global_cfg['legend_visible'])
        self.cb_legend.stateChanged.connect(self.on_global_changed)

        self.combo_legend_loc = QtWidgets.QComboBox()
        self.combo_legend_loc.addItems(['best', 'upper right', 'upper left', 'lower left', 'lower right', 'right', 'center left', 'center right', 'lower center', 'upper center', 'center', 'outside right', 'outside left', 'outside bottom'])
        self.combo_legend_loc.setCurrentText(self.global_cfg['legend_loc'])
        self.combo_legend_loc.currentTextChanged.connect(self.on_global_changed)

        global_layout.addRow(self.cb_legend)
        global_layout.addRow("Legend Loc:", self.combo_legend_loc)

        # Show grid info
        grid_info = QtWidgets.QLabel(f"Grid: {self.grid_type}")
        if self.grid_type == 'Symlog':
            grid_info.setText(f"Grid: {self.grid_type} (log_linthresh={self.log_linthresh})")
        global_layout.addRow(grid_info)

        self.panel_layout.addWidget(global_group)

        # Region of Interest (ROI) Settings
        roi_group = QtWidgets.QGroupBox("Region of Interest (ROI)")
        roi_layout = QtWidgets.QFormLayout(roi_group)

        # X Min
        self.spin_x_min = QtWidgets.QDoubleSpinBox()
        self.spin_x_min.setRange(-1e308, 1e308)
        self.spin_x_min.setDecimals(10)
        self.spin_x_min.setValue(self.bounds['x_min'])
        self.spin_x_min.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("X Min:", self.spin_x_min)

        # X Max
        self.spin_x_max = QtWidgets.QDoubleSpinBox()
        self.spin_x_max.setRange(-1e308, 1e308)
        self.spin_x_max.setDecimals(10)
        self.spin_x_max.setValue(self.bounds['x_max'])
        self.spin_x_max.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("X Max:", self.spin_x_max)

        # Y Min
        self.spin_y_min = QtWidgets.QDoubleSpinBox()
        self.spin_y_min.setRange(-1e308, 1e308)
        self.spin_y_min.setDecimals(10)
        self.spin_y_min.setValue(self.bounds['y_min'])
        self.spin_y_min.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("Y Min:", self.spin_y_min)

        # Y Max
        self.spin_y_max = QtWidgets.QDoubleSpinBox()
        self.spin_y_max.setRange(-1e308, 1e308)
        self.spin_y_max.setDecimals(10)
        self.spin_y_max.setValue(self.bounds['y_max'])
        self.spin_y_max.valueChanged.connect(self.on_roi_changed)
        roi_layout.addRow("Y Max:", self.spin_y_max)

        self.panel_layout.addWidget(roi_group)

        # Grid Settings
        grid_group = QtWidgets.QGroupBox("Grid Configuration")
        grid_layout = QtWidgets.QFormLayout(grid_group)

        # X Step
        self.spin_x_step = QtWidgets.QDoubleSpinBox()
        self.spin_x_step.setRange(0, 1e308)
        self.spin_x_step.setDecimals(10)
        self.spin_x_step.setValue(0)
        self.spin_x_step.setSpecialValueText("Auto")
        self.spin_x_step.valueChanged.connect(self.on_grid_changed)
        grid_layout.addRow("X Step:", self.spin_x_step)

        # Y Step
        self.spin_y_step = QtWidgets.QDoubleSpinBox()
        self.spin_y_step.setRange(0, 1e308)
        self.spin_y_step.setDecimals(10)
        self.spin_y_step.setValue(0)
        self.spin_y_step.setSpecialValueText("Auto")
        self.spin_y_step.valueChanged.connect(self.on_grid_changed)
        grid_layout.addRow("Y Step:", self.spin_y_step)

        # Custom X Ticks
        self.edit_x_ticks = QtWidgets.QLineEdit()
        self.edit_x_ticks.setPlaceholderText("e.g. 1, 2.5, 5, 10")
        self.edit_x_ticks.textChanged.connect(self.on_grid_changed)
        grid_layout.addRow("Custom X Ticks:", self.edit_x_ticks)

        # Custom Y Ticks
        self.edit_y_ticks = QtWidgets.QLineEdit()
        self.edit_y_ticks.setPlaceholderText("e.g. -1, 0, 1, 2")
        self.edit_y_ticks.textChanged.connect(self.on_grid_changed)
        grid_layout.addRow("Custom Y Ticks:", self.edit_y_ticks)

        # Reset button
        self.btn_reset_grid = QtWidgets.QPushButton("Reset to Auto")
        self.btn_reset_grid.clicked.connect(self.on_reset_grid)
        grid_layout.addRow(self.btn_reset_grid)

        self.panel_layout.addWidget(grid_group)

        # Lines Settings
        lines_group = QtWidgets.QGroupBox("Line Configurations")
        lines_layout = QtWidgets.QVBoxLayout(lines_group)

        for idx, cfg in enumerate(self.line_configs):
            line_box = QtWidgets.QGroupBox(f"{cfg['original_name']}")
            form = QtWidgets.QFormLayout(line_box)

            # Vis
            cb_vis = QtWidgets.QCheckBox("Visible")
            cb_vis.setChecked(True)
            cb_vis.stateChanged.connect(lambda state, i=idx: self.update_line_cfg(i, 'visible', state == QtCore.Qt.Checked))

            # Name
            edit_name = QtWidgets.QLineEdit(cfg['name'])
            edit_name.textChanged.connect(lambda text, i=idx: self.update_line_cfg(i, 'name', text))

            # Color
            btn_color = QtWidgets.QPushButton("Choose")
            r,g,b,a = cfg['color']
            btn_color.setStyleSheet(f"background-color: rgba({int(r*255)},{int(g*255)},{int(b*255)}, {a});")
            btn_color.clicked.connect(lambda _, i=idx, btn=btn_color: self.pick_color(i, btn))

            # Width
            spin_width = QtWidgets.QDoubleSpinBox()
            spin_width.setRange(0.0, 20.0)
            spin_width.setSingleStep(0.5)
            spin_width.setValue(cfg['width'])
            spin_width.valueChanged.connect(lambda val, i=idx: self.update_line_cfg(i, 'width', val))

            # Style
            combo_style = QtWidgets.QComboBox()
            combo_style.addItems(['Solid', 'Dashed', 'Dotted', 'Dash-Dot', 'None'])
            combo_style.setCurrentText(cfg['style'])
            combo_style.currentTextChanged.connect(lambda text, i=idx: self.update_line_cfg(i, 'style', text))

            # Marker
            combo_marker = QtWidgets.QComboBox()
            combo_marker.addItems(['None', 'Circle', 'Square', 'Triangle', 'Cross', 'Star', 'Plus'])
            combo_marker.currentTextChanged.connect(lambda text, i=idx: self.update_line_cfg(i, 'marker', text))

            form.addRow(cb_vis)
            form.addRow("Name:", edit_name)
            form.addRow("Color:", btn_color)
            form.addRow("Width:", spin_width)
            form.addRow("Style:", combo_style)
            form.addRow("Marker:", combo_marker)

            lines_layout.addWidget(line_box)

        self.panel_layout.addWidget(lines_group)
        self.panel_layout.addStretch()

    def on_global_changed(self, *_):
        self.global_cfg['legend_visible'] = self.cb_legend.isChecked()
        self.global_cfg['legend_loc'] = self.combo_legend_loc.currentText()
        self.update_plot()

    def on_roi_changed(self, *_):
        self.bounds['x_min'] = self.spin_x_min.value()
        self.bounds['x_max'] = self.spin_x_max.value()
        self.bounds['y_min'] = self.spin_y_min.value()
        self.bounds['y_max'] = self.spin_y_max.value()
        self.update_plot()

    def on_grid_changed(self, *_):
        self.grid_settings = {
            'x_step': self.spin_x_step.value(),
            'y_step': self.spin_y_step.value(),
            'x_ticks': self.parse_ticks(self.edit_x_ticks.text()),
            'y_ticks': self.parse_ticks(self.edit_y_ticks.text())
        }
        self.update_plot()

    def on_reset_grid(self):
        self.spin_x_step.setValue(0)
        self.spin_y_step.setValue(0)
        self.edit_x_ticks.clear()
        self.edit_y_ticks.clear()
        self.grid_settings = {
            'x_step': 0,
            'y_step': 0,
            'x_ticks': [],
            'y_ticks': []
        }
        self.update_plot()

    def parse_ticks(self, text):
        """Parse comma-separated tick values"""
        if not text.strip():
            return []
        try:
            return [float(x.strip()) for x in text.split(',') if x.strip()]
        except ValueError:
            return []

    def update_line_cfg(self, idx, key, val):
        self.line_configs[idx][key] = val
        if key in ['style', 'marker', 'width', 'name', 'visible', 'color']:
            self.update_plot()

    def pick_color(self, idx, btn):
        curr = self.line_configs[idx]['color']
        initial = QtGui.QColor(int(curr[0]*255), int(curr[1]*255), int(curr[2]*255), int(curr[3]*255))
        color = QtWidgets.QColorDialog.getColor(initial, self, "Pick Color", QtWidgets.QColorDialog.ShowAlphaChannel)
        if color.isValid():
            r, g, b, a = color.getRgbF()
            self.line_configs[idx]['color'] = [r, g, b, a]
            rgba_css = f"rgba({int(r*255)}, {int(g*255)}, {int(b*255)}, {a})"
            btn.setStyleSheet(f"background-color: {rgba_css};")
            self.update_plot()

    def update_plot(self):
        self.ax.clear()

        style_map = {
            'Solid': '-',
            'Dashed': '--',
            'Dotted': ':',
            'Dash-Dot': '-.',
            'None': ''
        }
        
        marker_map = {
            'None': '',
            'Circle': 'o',
            'Square': 's',
            'Triangle': '^',
            'Cross': 'x',
            'Star': '*',
            'Plus': '+'
        }

        has_data = False
        for cfg in self.line_configs:
            if not cfg['visible']:
                continue
            has_data = True
            ls = style_map.get(cfg['style'], '-')
            mk = marker_map.get(cfg['marker'], '')
            self.ax.plot(
                cfg['x'], cfg['y'],
                label=cfg['name'],
                color=cfg['color'],
                linewidth=cfg['width'],
                linestyle=ls,
                marker=mk,
                markersize=6
            )

        self.ax.set_xlabel('n')
        self.ax.set_ylabel('Value')

        # Data is already symlog-transformed, so always use linear scale
        self.ax.set_yscale('linear')
        
        # Set custom formatter to display original values
        if self.grid_type == 'Symlog':
            self.ax.yaxis.set_major_formatter(FuncFormatter(symlog_grid_formatter_factory(self.log_linthresh)))
        else:
            self.ax.yaxis.set_major_formatter(FuncFormatter(lambda y, pos: format_grid_value(y)))

        # Set initial bounds from exported data
        self.ax.set_xlim(self.bounds['x_min'], self.bounds['x_max'])
        self.ax.set_ylim(self.bounds['y_min'], self.bounds['y_max'])
        
        # Configure grid and tick locators (MUST be after set_yscale!)
        self.ax.grid(True, which='both', linestyle='--', alpha=0.5)
        
        # X-axis tick configuration - combine standard spacing with custom ticks
        if self.grid_settings['x_step'] > 0:
            x_base = MultipleLocator(self.grid_settings['x_step'])
        else:
            x_base = AutoLocator()
        
        if self.grid_settings['x_ticks']:
            x_locator = CombinedLocator(x_base, self.grid_settings['x_ticks'])
        else:
            x_locator = x_base
        self.ax.xaxis.set_major_locator(x_locator)
        
        # Y-axis tick configuration - combine standard spacing with custom ticks
        if self.grid_settings['y_step'] > 0:
            y_base = MultipleLocator(self.grid_settings['y_step'])
        else:
            y_base = AutoLocator()
        
        if self.grid_settings['y_ticks']:
            y_locator = CombinedLocator(y_base, self.grid_settings['y_ticks'])
        else:
            y_locator = y_base
        self.ax.yaxis.set_major_locator(y_locator)

        if has_data and self.global_cfg['legend_visible']:
            loc = self.global_cfg['legend_loc']
            
            if loc == 'outside right':
                self.ax.legend(loc='upper left', bbox_to_anchor=(1.02, 1), borderaxespad=0)
            elif loc == 'outside left':
                self.ax.legend(loc='upper right', bbox_to_anchor=(-0.02, 1), borderaxespad=0)
            elif loc == 'outside bottom':
                self.ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.1), borderaxespad=0, ncol=2)
            else:
                self.ax.legend(loc=loc)

        self.canvas.draw()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    
    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        path, _ = QtWidgets.QFileDialog.getOpenFileName(None, "Select series_data.json", "", "JSON Files (*.json)")
        
    if path:
        try:
            with open(path, 'r') as f:
                data = json.load(f)
            window = PlotWindow(data)
            window.show()
            sys.exit(app.exec())
        except Exception as e:
            print(f"Failed to load or display data: {e}")
    else:
        print("No file selected.")
