#ifndef LIB_HPP
#define LIB_HPP
#pragma once

// clang-format off
// Critical Section

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>

#include "gsl/gsl_sf_expint.h"
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_lambert.h>

#include "custom_types/mpreal.h"
#include "custom_types/operation_counter.hpp"

#include "custom_concepts/float_like.hpp"

#include "custom_types/intervalprecision.fwd.hpp"

#include "custom_concepts/interval_like.hpp"
#include "custom_concepts/unsigned_int_like.hpp"
#include "custom_concepts/precisable_like.hpp"
#include "custom_concepts/complex_like.hpp"
#include "custom_concepts/profiling_like.hpp"

template <typename T>
concept AcceptedLike = requires { requires FloatLike<T> || ComplexLike<T> || IntervalLike<T>; };

#include <iomanip>

#include "utils.hpp"

#include "custom_types/intervalprecision.hpp"


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
}  // namespace Eigen

#endif