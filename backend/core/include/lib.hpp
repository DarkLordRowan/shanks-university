#ifndef LIB_HPP
#define LIB_HPP
#pragma once

// clang-format off
// Critical Section

// system includes and std
#include <algorithm>
#include <assert.h>
#include <concepts>
#include <iomanip>
#include <cassert>
#include <climits>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <complex>   // Need <complex> to support FFT functions for fast multiplications
#include <cstdlib>
#include <random>	// Needed for random_precision class and PRNGs in general
#include <limits>
#include <sstream>
#include <type_traits>
#include <utility>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>

#include "gsl/gsl_sf_expint.h"
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_lambert.h>
#include <gsl/gsl_sf_airy.h>
#include <gsl/gsl_sf_clausen.h>
#include <gsl/gsl_errno.h>

#include "custom_types/mpreal.h"
#include "custom_types/operation_counter.fwd.hpp"
#include "custom_types/iprecision.hpp"
#include "custom_types/fprecision.hpp"
#include "custom_types/fractionprecision.hpp"

#include "custom_concepts/float_like.hpp"

#include "custom_types/intervalprecision.fwd.hpp"

#include "custom_concepts/interval_like.hpp"
#include "custom_concepts/unsigned_int_like.hpp"
#include "custom_concepts/precisable_like.hpp"
#include "custom_concepts/complex_like.hpp"
#include "custom_concepts/profiling_like.hpp"

template <typename T>
concept AcceptedLike = requires { requires FloatLike<T> || ComplexLike<T> || IntervalLike<T>; };

#include "utils.hpp"

#include "custom_types/intervalprecision.hpp"
#include "custom_types/operation_counter.hpp"


#include "series_base.hpp"
#include "series_iterators.hpp"
#include "series.hpp"

#include "series_acceleration.hpp"
#include "methods.hpp"

#include "filters/kolmogorov_zurbenko.hpp"
#include "filters/savitzky_golay.hpp"

#include "noise/noise_generator.hpp"

// clang-format on

namespace Eigen {

template <>
struct NumTraits<intprec::interval<float>>
    : NumTraits<float>  // permits to get the epsilon, dummy_precision, lowest, highest functions
{
    typedef intprec::interval<float> Real;
    typedef intprec::interval<float> NonInteger;
    typedef intprec::interval<float> Nested;

    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};

template <>
struct NumTraits<intprec::interval<double>>
    : NumTraits<double>  // permits to get the epsilon, dummy_precision, lowest, highest functions
{
    typedef intprec::interval<double> Real;
    typedef intprec::interval<double> NonInteger;
    typedef intprec::interval<double> Nested;
    //
    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};

template <>
struct NumTraits<intprec::interval<long double>>
    : NumTraits<long double>  // permits to get the epsilon, dummy_precision, lowest, highest functions
{
    typedef intprec::interval<long double> Real;
    typedef intprec::interval<long double> NonInteger;
    typedef intprec::interval<long double> Nested;

    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};

#ifdef __MPREAL_H__

template <>
struct NumTraits<intprec::interval<mpfr::mpreal>>
    : NumTraits<double>  // permits to get the epsilon, dummy_precision, lowest, highest functions
{
    typedef intprec::interval<mpfr::mpreal> Real;
    typedef intprec::interval<mpfr::mpreal> NonInteger;
    typedef intprec::interval<mpfr::mpreal> Nested;

    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};

#endif

#ifdef INC_FPRECISION

template <>
struct NumTraits<intprec::interval<arb::float_precision>>
    : NumTraits<double>  // permits to get the epsilon, dummy_precision, lowest, highest functions
{
    typedef intprec::interval<arb::float_precision> Real;
    typedef intprec::interval<arb::float_precision> NonInteger;
    typedef intprec::interval<arb::float_precision> Nested;

    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};

#endif

}  // namespace Eigen

#endif