from typing import Any, Iterable

from src.logger import logged_debug


def generate_range(start: float, stop: float, step: float) -> Iterable[float]:
    if step == 0:
        raise ValueError("Step cannot be zero in a range construction")
    count = int((stop - start) / step)
    return (start + i * step for i in range(count))


@logged_debug
def autowrap(value: Any):
    if value is None:
        return []
    if isinstance(value, bool):
        return [value]
    if isinstance(value, dict) and {"start", "stop", "step"} <= value.keys():
        try:
            return generate_range(
                float(value["start"]),
                float(value["stop"]),
                float(value["step"]),
            )
        except KeyError as e:
            raise KeyError(
                f"Please provide every field in a range construction: {value}"
            ) from e
        except ValueError as e:
            raise ValueError(
                f"Float is expected in a range construction: {value}"
            ) from e
    if isinstance(value, (str, bytes, bytearray)):
        return [value]
    if isinstance(value, Iterable) and not isinstance(value, (str, bytes, dict)):
        return value

    try:
        return [float(value)]
    except ValueError as e:
        raise ValueError(
            f"Unknown value type failed to be converted to a float: {value}"
        ) from e
