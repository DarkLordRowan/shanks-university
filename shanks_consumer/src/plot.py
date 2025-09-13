import matplotlib.pyplot as plt
from matplotlib import rcParams
import os
import pathlib

class InteractiveConvergencePlot:

    def __init__(self, results, save_dir: pathlib.Path | None = None):
        self.results = list(results)
        self.current_index = 0
        self.fig = None
        self.axes = None
        self.save_dir = save_dir

        if self.save_dir:
            os.makedirs(self.save_dir, exist_ok=True)

        self.setup_plot()

    def setup_plot(self):
        rcParams["font.family"] = "DejaVu Sans"
        rcParams["font.size"] = 11
        rcParams["axes.titlesize"] = 12
        rcParams["axes.titleweight"] = "bold"
        rcParams["axes.labelweight"] = "bold"
        rcParams["lines.linewidth"] = 2
        rcParams["lines.markersize"] = 6

        self.fig, (self.ax1, self.ax2, self.ax3) = plt.subplots(
            1, 3, figsize=(18, 6), dpi=100
        )

        # Only connect keyboard events if we're showing interactively
        if not self.save_dir:
            self.fig.canvas.mpl_connect("key_press_event", self.on_key_press)
            self.fig.canvas.manager.set_window_title(
                "Анализатор сходимости методов"
            )

        self.update_plot()

    def on_key_press(self, event):
        if event.key in ["right", "d"]:
            self.current_index = (self.current_index + 1) % len(self.results)
            self.update_plot()
        elif event.key in ["left", "a"]:
            self.current_index = (self.current_index - 1) % len(self.results)
            self.update_plot()
        elif event.key in ["home", "0"]:
            self.current_index = 0
            self.update_plot()
        elif event.key in ["end", "$"]:
            self.current_index = len(self.results) - 1
            self.update_plot()

    def update_plot(self):
        trial = self.results[self.current_index]
        computed = trial.computed
        true_value = trial.series.lim

        n_values = [point.n for point in computed]
        partial_sums = [point.partial_sum for point in computed]
        accel_values = [point.accel_value for point in computed]
        partial_deviations = [
            abs(point.partial_sum - true_value) for point in computed
        ]
        accel_deviations = [
            abs(point.accel_value - true_value) for point in computed
        ]

        for ax in [self.ax1, self.ax2, self.ax3]:
            ax.clear()

        colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd"]

        self.ax1.axhline(
            y=true_value,
            color="red",
            linestyle="--",
            linewidth=3,
            alpha=0.9,
            label=f"Предел: {true_value:.8f}",
        )
        self.ax1.plot(
            n_values,
            partial_sums,
            "o-",
            color=colors[0],
            markersize=4,
            label="Частичные суммы",
            alpha=0.8,
        )
        self.ax1.plot(
            n_values,
            accel_values,
            "s-",
            color=colors[1],
            markersize=4,
            label="Ускоренный метод",
            alpha=0.8,
        )
        self.ax1.set_xlabel("Порядок (n)")
        self.ax1.set_ylabel("Значение")
        self.ax1.set_title("Сравнение значений методов")
        self.ax1.legend()
        self.ax1.grid(True, alpha=0.3, linestyle="--")
        self.ax1.ticklabel_format(useOffset=False)

        self.ax2.semilogy(
            n_values,
            partial_deviations,
            "o-",
            color=colors[0],
            markersize=4,
            label="Частичные суммы",
            alpha=0.8,
        )
        self.ax2.semilogy(
            n_values,
            accel_deviations,
            "s-",
            color=colors[1],
            markersize=4,
            label="Ускоренный метод",
            alpha=0.8,
        )
        self.ax2.set_xlabel("Порядок (n)")
        self.ax2.set_ylabel("Абсолютное отклонение (log scale)")
        self.ax2.set_title("Отклонения от предела")
        self.ax2.legend()
        self.ax2.grid(True, alpha=0.3, linestyle="--", which="both")

        valid_indices = [
            i
            for i in range(len(n_values))
            if partial_deviations[i] > 1e-16 and accel_deviations[i] > 1e-16
        ]

        if valid_indices:
            valid_n = [n_values[i] for i in valid_indices]
            acceleration_ratio = [
                partial_deviations[i] / accel_deviations[i]
                for i in valid_indices
            ]

            self.ax3.plot(
                valid_n,
                acceleration_ratio,
                "D-",
                color=colors[2],
                markersize=4,
                alpha=0.8,
                label="Коэффициент ускорения",
            )
            self.ax3.axhline(y=1, color="gray", linestyle="--", alpha=0.7)

            if acceleration_ratio:
                max_accel = max(acceleration_ratio)
                max_idx = acceleration_ratio.index(max_accel)
                self.ax3.annotate(
                    f"Max: {max_accel:.1f}x",
                    xy=(valid_n[max_idx], max_accel),
                    xytext=(10, 10),
                    textcoords="offset points",
                    bbox=dict(
                        boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.7
                    ),
                    arrowprops=dict(
                        arrowstyle="->", connectionstyle="arc3,rad=0"
                    ),
                )

        self.ax3.set_xlabel("Порядок (n)")
        self.ax3.set_ylabel("Отношение отклонений")
        self.ax3.set_title(
            "Коэффициент ускорения сходимости\n(Частичная/Ускоренная)"
        )
        self.ax3.legend()
        self.ax3.grid(True, alpha=0.3, linestyle="--")
        self.ax3.set_yscale("log")

        series_params = getattr(trial.series, "arguments", {})
        accel_params = getattr(trial.accel, "additional_args", {})

        title = (
            f"Испытание {self.current_index + 1}/{len(self.results)}: "
            f"{trial.series.name} {series_params} | {trial.accel.name} [m={trial.accel.m_value}]\n"
            f"←/→: навигация | Home/End: первое/последнее испытание"
        )

        self.fig.suptitle(title, fontsize=13, fontweight="bold")

        best_accel = (
            min(accel_deviations) if accel_deviations else float("inf")
        )
        best_partial = (
            min(partial_deviations) if partial_deviations else float("inf")
        )

        stats_text = (
            f"Ряд: {trial.series.name}\n"
            f"Метод: {trial.accel.name}\n"
            f"Предел: {true_value:.12f}\n"
            f"Лучшее ускоренное: {best_accel:.2e}\n"
            f"Лучшая частичная: {best_partial:.2e}"
        )

        self.fig.text(
            0.02,
            0.02,
            stats_text,
            fontsize=10,
            bbox=dict(
                boxstyle="round,pad=0.5",
                facecolor="lightblue",
                alpha=0.8,
                edgecolor="navy",
                linewidth=1.5,
            ),
        )

        plt.tight_layout()
        plt.subplots_adjust(top=0.90, bottom=0.15)

        # Save the plot if save directory is specified
        if self.save_dir:
            filename = f"trial_{self.current_index + 1:03d}_{trial.series.name}_{trial.accel.name}.png"
            filepath = os.path.join(self.save_dir, filename)
            plt.savefig(filepath, dpi=150, bbox_inches="tight")
            print(f"Saved: {filepath}")

        self.fig.canvas.draw()

    def show(self):
        if self.save_dir:
            # If saving mode, iterate through all trials and save them
            for i in range(len(self.results)):
                self.current_index = i
                self.update_plot()
            plt.close(self.fig)  # Close the figure after saving all
        else:
            # Interactive mode
            plt.show()


# Alternative function for batch saving without interactive display
def save_all_plots(results, save_dir):
    """
    Save all trial plots to the specified directory without interactive display.

    Parameters:
    results: List of trial results
    save_dir: Directory to save plots (default: "convergence_plots")
    """
    plotter = InteractiveConvergencePlot(results, save_dir=save_dir)
    plotter.show()  # This will save all plots and exit
