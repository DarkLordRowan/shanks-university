"""Precision regressions using the actual pyshanks bindings, not mocks.

PYTHONPATH=backend/runner/src python -m pytest backend/runner/tests
"""
from decimal import Decimal

import pytest
import pyshanks as ps
from shanks.runner.infra.export.serializer import TrialResultSerializer


@pytest.mark.parametrize("text", [
    "1.000000000000000000000000000000000001",
    "0.999999999999999999999999999999999998",
    "0.000000000000000000000000000000000001",
    "1.000000000000000000001e-500",
    "1.000000000000000000001e500",
    "1234567890123456789012345678901234567890",
])
def test_serialization_retains_exact_decimal_value(text):
    serializer = TrialResultSerializer()
    for value in (text, Decimal(text)):
        assert Decimal(serializer._beautify_numeric(value)) == Decimal(text)
        assert Decimal(serializer._normalize_arg(value)) == Decimal(text)


def test_similar_arguments_do_not_merge():
    args = {"x": Decimal("1.00000000000000000000001"), "y": Decimal("1.00000000000000000000002")}
    actual = TrialResultSerializer()._sanitize_arguments(args)
    assert actual["x"] != actual["y"]
    assert {k: Decimal(v) for k, v in actual.items()} == args


@pytest.mark.parametrize("text, expected", [("1.23000", "1.23"), ("0.0000", "0"),
    ("1000", "1000"), ("nan", "nan"), ("inf", "inf"), ("1.2e-500", "1.2e-500")])
def test_formatting_is_only_lexical(text, expected):
    assert TrialResultSerializer()._beautify_numeric(text) == expected


def test_actual_mpfr_binding_serialization():
    value = ps.Arb("1.000000000000000000000000000000000000001", 256)
    serializer = TrialResultSerializer()
    assert Decimal(serializer._beautify_numeric(value)) == Decimal(str(value))
    assert Decimal(serializer._normalize_arg(value)) == Decimal(str(value))
    assert Decimal(serializer._normalize_arg(value)) != Decimal(1)



def test_numeric_serialization_in_nonprofiling_build():
    serializer = TrialResultSerializer()
    assert serializer._sanitize_value(1.25) == "1.25"
    assert serializer._sanitize_numeric(1.25, as_struct=True) == {"real": "1.25", "imag": "0.0"}
    value = ps.Arb("1.000000000000000000000000000000000000001", 256)
    assert Decimal(serializer._sanitize_value(value)) != Decimal(1)
