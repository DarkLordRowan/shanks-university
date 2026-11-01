"""Input allocation and CSV precision regressions with real bindings."""
from decimal import Decimal

import pytest
from shanks.runner.domain.params import AccelParamJSON
from shanks.runner.domain.precision import PrecisionType, cast_natural_series_value


def parameter(n=(0, 7), m=(0, 20)):
    return AccelParamJSON(n=n, m=m, events=[], precision=PrecisionType("F64"), name="LubkinW", init_args={})


def test_high_order_input_budget_covers_actual_stencils():
    assert parameter().size_floor >= 7+3*20+1
    assert parameter(m=(0,)).size_floor >= 7+3


@pytest.mark.parametrize("n, m", [([], [1]), ([1], []), ([-1], [1]), ([1], [-1]), ([True], [1]), ([1], [1.5])])
def test_invalid_index_grids(n, m):
    with pytest.raises(ValueError):
        parameter(n, m).size_floor


@pytest.mark.parametrize("precision", ["F64", "Arb256", "CArb256"])
def test_csv_rejects_malformed_numbers(precision):
    with pytest.raises((ValueError, RuntimeError)):
        cast_natural_series_value(PrecisionType(precision), "not-a-number")


def test_complex_arb_csv_retains_decimal_digits():
    value = cast_natural_series_value(PrecisionType("CArb256"), "1.000000000000000000000000000000000001")
    assert Decimal(str(value.real)) != Decimal(1)
    assert Decimal(str(value.imag)) == 0



@pytest.mark.parametrize("suffix", ["FLong", "CFLong"])
def test_long_double_boundary_available_without_profiling(suffix):
    from shanks.runner.domain.precision import cast_precision_value
    assert cast_precision_value(PrecisionType(suffix), "1.25") is not None
