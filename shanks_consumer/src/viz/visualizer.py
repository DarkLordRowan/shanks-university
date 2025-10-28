import pathlib

import pandas as pd
import plotly.express as px


class DataVisualizer:
    @staticmethod
    def create_scatter_plot(
        df: pd.DataFrame, output_file: pathlib.Path
    ) -> None:
        if (
            "slow_accel_method" not in df.columns
            or "divergent_accel_method" not in df.columns
        ):
            raise ValueError(
                "Отсутствуют необходимые колонки для визуализации"
            )

        fig = px.scatter(
            df,
            x="slow_accel_method",
            y="divergent_accel_method",
            color="method_name",
            size="error_rate",
            hover_data=["m_value", "additional_args_str"],
            log_x=True,
            log_y=True,
            title="Методы ускорения — устойчивость и ошибки",
        )

        fig.write_html(output_file, include_plotlyjs="cdn")

    @staticmethod
    def create_computed_line_plot(
        df: pd.DataFrame,
        series_name: str,
        method_name: str,
        output_file: pathlib.Path,
    ) -> None:

        if df.empty:
            raise ValueError("Нет данных для выбранного метода/ряда")

        fig = px.line(
            df,
            x="n",
            y="accel_value",
            color="variation",
            hover_data=[
                "partial_sum",
                "partial_sum_deviation",
                "accel_value_deviation",
            ],
            markers=True,
            title=f"Сходимость метода {method_name} для ряда {series_name}",
        )

        variations = df["variation"].unique()
        for variation in variations:
            variation_df = df[df["variation"] == variation]
            fig.add_scatter(
                x=variation_df["n"],
                y=variation_df["partial_sum"],
                mode="lines+markers",
                line=dict(dash="dash", width=1),
                marker=dict(size=4, symbol="x"),
                name=f"{variation} (частичные суммы)",
                legendgroup=variation,
                showlegend=True,
                hovertemplate=(
                    "Итерация: %{x}<br>"
                    "Частичная сумма: %{y:.10f}<br>"
                    "Вариация: " + variation + "<extra></extra>"
                ),
            )

        series_limit = (
            df["lim"].iloc[0]
            if "lim" in df.columns and not df["lim"].isna().all()
            else None
        )

        if series_limit is not None and pd.notna(series_limit):
            fig.add_hline(
                y=series_limit,
                line_dash="dot",
                line_color="red",
                line_width=3,
                annotation_text=f"Предел ряда: {series_limit:.10f}",
                annotation_position="top right",
                annotation_font_size=12,
                annotation_font_color="red",
                annotation_bgcolor="white",
            )
        fig.write_html(output_file, include_plotlyjs="cdn")
