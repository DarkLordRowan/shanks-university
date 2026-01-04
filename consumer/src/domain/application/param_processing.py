"""
Parameter processing utilities.
Author: Shevyrov A.N., Yadrentsev I. M.
"""

from typing import Any, Iterable


def generate_range(start: float, stop: float, step: float) -> Iterable[float]:
    """Generate a range of floats from start to stop with a given step."""
    if step == 0:
        raise ValueError("Step cannot be zero in a range construction")
    count = int((stop - start) / step)
    return (start + i * step for i in range(count))


def autowrap(value: Any) -> Iterable[Any]:
    """Automatically wrap the input value into an appropriate iterable form.
    This function handles various input types including None, booleans,
    range specifications, strings, and numeric types.
    Converts as follows:
    - None -> []
    - bool -> [bool]
    - dict with 'start', 'stop', 'step' -> generates range of floats
    - str/bytes -> [str/bytes]
    - Iterable (excluding str, bytes, dict) -> returns as is
    - other types -> attempts to convert to float and returns [float]

    :param value: Input value to be wrapped.
    :type value: Any
    :raises KeyError: _start_, _stop_, or _step_ missing in range construction
    :raises ValueError: invalid float conversion or zero step in range
    :raises ValueError: invalid type for float conversion
    :return: Wrapped iterable of appropriate type.
    :rtype: Iterable[Any]
    """
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
        return [float(value)]  # pyright: ignore
    except ValueError as e:
        raise ValueError(
            f"Unknown value type failed to be converted to a float: {value}"
        ) from e
