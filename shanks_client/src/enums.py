from enum import Enum


class SortField(str, Enum):
    created_at = "created_at"
    series_name = "series.name"
    accel_name = "accel_name"
    n_count = "n_count"


class SortOrder(str, Enum):
    asc = "asc"
    desc = "desc"
