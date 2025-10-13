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
            if hasattr(self.fig.canvas, 'manager') and self.fig.canvas.manager is not None:
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
            abs(point.partial_sum - true_value) if true_value is not None else None for point in computed
        ]
        accel_deviations = [
            abs(point.accel_value - true_value) if true_value is not None else None for point in computed
        ]

        for ax in [self.ax1, self.ax2, self.ax3]:
            ax.clear()

        colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd"]

        if true_value is not None:
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

        valid_indices = []
        for i in range(len(n_values)):
            pd = partial_deviations[i]
            ad = accel_deviations[i]
            if pd is not None and ad is not None and pd > 1e-16 and ad > 1e-16:
                valid_indices.append(i)

        if valid_indices:
            valid_n = [n_values[i] for i in valid_indices]
            acceleration_ratio = []
            for i in valid_indices:
                pd = partial_deviations[i]
                ad = accel_deviations[i]
                if pd is not None and ad is not None and ad != 0:
                    acceleration_ratio.append(pd / ad)
                else:
                    acceleration_ratio.append(0)

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
                    textcoords=" offset points",
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

        if self.fig is not None:
            self.fig.suptitle(title, fontsize=13, fontweight="bold")

        best_accel = (
            min([d for d in accel_deviations if d is not None]) if accel_deviations and any(d is not None for d in accel_deviations) else float("inf")
        )
        best_partial = (
            min([d for d in partial_deviations if d is not None]) if partial_deviations and any(d is not None for d in partial_deviations) else float("inf")
        )

        # Format parameters for stats text
        series_params_stats = ""
        if series_params:
            series_params_stats = "\n" + "\n".join([f"  {k}: {v}" for k, v in series_params.items()])

        accel_params_stats = ""
        if accel_params:
            accel_params_stats = "\n" + "\n".join([f"  {k}: {v}" for k, v in accel_params.items()])

        limit_text = f"Предел: {true_value:.12f}\n" if true_value is not None else "Предел: N/A\n"
        stats_text = (
            f"Ряд: {trial.series.name}{series_params_stats}\n"
            f"Метод: {trial.accel.name} [m={trial.accel.m_value}]{accel_params_stats}\n"
            f"{limit_text}"
            f"Лучшее ускоренное: {best_accel:.2e}\n"
            f"Лучшая частичная: {best_partial:.2e}"
        )

        if self.fig is not None:
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
            # Create parameter string for filename
            accel_params = getattr(trial.accel, "additional_args", {})
            param_str = ""
            if accel_params:
                param_str = "_" + "_".join([f"{k}_{v}" for k, v in accel_params.items()])

            filename = f"trial_{self.current_index + 1:03d}_{trial.series.name}_{trial.accel.name}{param_str}.png"
            filepath = os.path.join(self.save_dir, filename)
            plt.savefig(filepath, dpi=150, bbox_inches="tight")
            print(f"Saved: {filepath}")

        if self.fig is not None and hasattr(self.fig.canvas, 'draw'):
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
