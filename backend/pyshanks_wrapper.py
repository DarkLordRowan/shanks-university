from __future__ import annotations
from dataclasses import dataclass
from typing import Dict, Type, Any, Optional, Callable, Iterable

import inspect
import types

try:
    from . import pyshanks
except Exception as e:
    raise RuntimeError(f"Не удалось импортировать pyshanks: {e}") from e


# === модели-результаты ===
@dataclass(frozen=True)
class SeriesResult:
    sum: float
    term: Optional[float] = None
    trace: Optional[list[float]] = None


@dataclass(frozen=True)
class AccelResult:
    accelerated: float
    difference: Optional[float] = None


# === утилиты обнаружения ===
def _is_class(obj: Any) -> bool:
    return isinstance(obj, type)

def _has_attr(obj: Any, name: str) -> bool:
    return hasattr(obj, name)

def _call_if_exists(obj: Any, name: str, *args, **kwargs):
    if hasattr(obj, name):
        return getattr(obj, name)(*args, **kwargs)
    raise AttributeError

def _first_present(names: Iterable[str], obj: Any) -> Optional[str]:
    for n in names:
        if hasattr(obj, n):
            return n
    return None


class ShanksEngine:
    """
    Фасад над бинарным модулем.
    Автоматически находит:
      - все классы-ряды (*Series) с методом S_n(n)
      - все классы-ускорители (*Algorithm/*Transform)
    И предоставляет единый способ их вызова.
    """

    def __init__(self) -> None:
        self.series_classes: Dict[str, Type] = {}
        self.accel_classes: Dict[str, Type] = {}

        for name in dir(pyshanks):
            obj = getattr(pyshanks, name)
            if not _is_class(obj):
                continue
            # Ряды
            if name.endswith("Series") and _has_attr(obj, "S_n"):
                self.series_classes[self._key(name)] = obj
            # Ускорители / преобразования
            elif name.endswith("Algorithm") or name.endswith("Transform"):
                self.accel_classes[self._key(name)] = obj

        if not self.series_classes:
            raise RuntimeError("В pyshanks не найдено ни одного класса ряда (*Series с методом S_n).")

    @staticmethod
    def _key(name: str) -> str:
        # человеко-понятный ключ: ExpSeries -> exp, WynnEpsilonAlgorithm -> wynn-epsilon
        import re
        words = re.findall(r"[A-Z][a-z0-9]*", name)
        key = "-".join(w.lower() for w in words if w.lower() not in {"series", "algorithm", "transform"})
        return key or name.lower()

    # ---------- публичное API ----------
    def list_series(self) -> Dict[str, str]:
        return {k: v.__name__ for k, v in self.series_classes.items()}

    def list_accels(self) -> Dict[str, str]:
        return {k: v.__name__ for k, v in self.accel_classes.items()}

    def series_compute(self, series_key: str, x: float, n: int) -> SeriesResult:
        cls = self.series_classes.get(series_key)
        if cls is None:
            raise KeyError(f"Неизвестный ряд: {series_key}")

        # большинство ваших рядов принимают x в конструктор
        inst = cls(float(x))

        # обязательно есть S_n(n)
        s = float(inst.S_n(int(n)))

        # если в биндинге есть метод term(n) — вернём, если нет — None
        term_val = None
        if hasattr(inst, "term"):
            try:
                term_val = float(inst.term(int(n)))
            except Exception:
                term_val = None

        trace = getattr(inst, "trace", None)
        return SeriesResult(sum=s, term=term_val, trace=trace)

    def accelerate(self, accel_key: str, series_key: str, x: float, n: int, order: int) -> AccelResult:
        """
        Универсальный вызов ускорителя с перебором типичных сигнатур:
          1) a = Accel(series); a.accelerate(n, order)
          2) a = Accel();       a.accelerate(series, n, order)
          3) a = Accel(series); a.apply(n, order) / a.transform(n, order)
          4) статические методы: Accel.accelerate(series, n, order) и т.п.
        Если ни одна не подошла — выдаём понятную ошибку.
        """
        acls = self.accel_classes.get(accel_key)
        if acls is None:
            raise KeyError(f"Неизвестный ускоритель: {accel_key}")

        scls = self.series_classes.get(series_key)
        if scls is None:
            raise KeyError(f"Неизвестный ряд: {series_key}")

        s_obj = scls(float(x))

        candidates_ctor = [
            lambda: acls(s_obj),  # Accel(series)
            lambda: acls(),       # Accel()
        ]
        method_names = ("accelerate", "apply", "transform", "__call__")

        # 1) Попробуем a = Accel(series); a.<method>(n, order)
        for ctor in candidates_ctor:
            try:
                a_obj = ctor()
            except Exception:
                continue
            m = _first_present(method_names, a_obj)
            if m:
                try:
                    res = getattr(a_obj, m)(int(n), int(order))
                    acc = float(res)
                    base = float(s_obj.S_n(int(n)))
                    return AccelResult(accelerated=acc, difference=acc - base)
                except Exception:
                    pass

        # 2) Попробуем статические методы: Accel.<method>(series, n, order)
        for m in method_names:
            if hasattr(acls, m) and isinstance(getattr(acls, m), (types.FunctionType, types.BuiltinFunctionType, types.MethodType, types.BuiltinMethodType,)):
                try:
                    res = getattr(acls, m)(s_obj, int(n), int(order))
                    acc = float(res)
                    base = float(s_obj.S_n(int(n)))
                    return AccelResult(accelerated=acc, difference=acc - base)
                except Exception:
                    pass

        # 3) Иногда порядок аргументов другой: (n, series, order)
        for m in method_names:
            if hasattr(acls, m):
                try:
                    res = getattr(acls, m)(int(n), s_obj, int(order))
                    acc = float(res)
                    base = float(s_obj.S_n(int(n)))
                    return AccelResult(accelerated=acc, difference=acc - base)
                except Exception:
                    pass

        raise RuntimeError(
            f"Не удалось вызвать ускоритель '{accel_key}'. "
            f"Проверьте сигнатуру его методов в биндингах pybind11."
        )

    # Диагностика: показать методы класса (как их видит Python)
    def inspect_class(self, name: str) -> Dict[str, Any]:
        obj = getattr(pyshanks, name, None)
        if obj is None or not _is_class(obj):
            raise KeyError(f"Класс '{name}' не найден в pyshanks")
        return {
            "name": name,
            "methods": [m for m in dir(obj) if not m.startswith("__")],
            "signature": str(inspect.signature(obj)) if hasattr(obj, "__init__") else None,
        }
