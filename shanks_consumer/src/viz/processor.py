import json

import pandas as pd


class DataProcessor:
    @staticmethod
    def explode_events(df: pd.DataFrame) -> pd.DataFrame:
        df = df.explode("events", ignore_index=True)
        df["event_type"] = df["events"].apply(
            lambda x: x.get("event_type") if isinstance(x, dict) else None
        )
        df["event_count"] = df["events"].apply(
            lambda x: x.get("count") if isinstance(x, dict) else None
        )

        return df

    @staticmethod
    def normalize_field(
        df: pd.DataFrame,
        target_field: str,
        normalized_field: str,
    ) -> pd.DataFrame:
        df[normalized_field] = df[target_field].apply(
            lambda x: (
                json.dumps(x, sort_keys=True, ensure_ascii=False)
                if isinstance(x, dict)
                else str(x)
            )
        )
        return df

    @staticmethod
    def provide_variation_field(df: pd.DataFrame):
        df["variation"] = df.apply(
            lambda row: (
                f"(m={row['m']}) {row['accel_additional_args_str']} [{row['series_args_str']}]"
            ),
            axis=1,
        )
        return df

    @staticmethod
    def create_pivot_table(df: pd.DataFrame) -> pd.DataFrame:
        pivot = df.pivot_table(
            index=["method_name", "m_value", "additional_args_str"],
            columns="event_type",
            values="event_count",
            fill_value=0,
            aggfunc="sum",
        ).reset_index()
        return pivot

    @staticmethod
    def merge_base_with_pivot(
        df: pd.DataFrame, pivot: pd.DataFrame
    ) -> pd.DataFrame:
        base = df.drop(
            columns=["events", "event_type", "event_count"]
        ).drop_duplicates(
            subset=["method_name", "m_value", "additional_args_str"]
        )

        result_df = base.merge(
            pivot,
            on=["method_name", "m_value", "additional_args_str"],
            how="left",
        )

        if "slow_accel_method" in result_df.columns:
            result_df["slow_accel_method"] = result_df[
                "slow_accel_method"
            ].fillna(0)

        return result_df
