from typing import Any, Iterable


def ensure_number(x: Any) -> Any:
    if isinstance(x, bool):
        return x
    if isinstance(x, (int, float)):
        return x
    if isinstance(x, str):
        try:
            if any(sep in x for sep in [".", "e", "E"]):
                return float(x)
            return int(x)
        except ValueError:
            return x
    try:
        return float(x)
    except Exception:
        return x


def generate_range(start: float, stop: float, step: float) -> Iterable[float]:
    if step == 0:
        raise ValueError("step cannot be zero")
    count = int((stop - start) / step)
    return (start + i * step for i in range(count))


def autowrap(value: Any):
    if value is None:
        return []
    if isinstance(value, dict) and {"start", "stop", "step"} <= value.keys():
        return range(
            ensure_number(value["start"]),
            ensure_number(value["stop"]),
            ensure_number(value["step"]),
        )
    if isinstance(value, (str, bytes, bytearray)):
        return [value]
    if isinstance(value, Iterable) and not isinstance(value, (str, bytes, dict)):
        return [ensure_number(v) for v in value]
    return [ensure_number(value)]
