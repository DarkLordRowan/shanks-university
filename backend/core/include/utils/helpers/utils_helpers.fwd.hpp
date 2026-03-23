#ifndef UTILS_HELPERS_FWD_HPP
#define UTILS_HELPERS_FWD_HPP
#pragma once

template <typename T>
struct utils::helpers {
    static T nextafter(const T& x, const T& y);
    static T fmod(const T& x, const T& y);
    static T floor(const T& x);
    static T ceil(const T& x);
    static T get_nan();
    static T frexp(const T& x, int* exp);
    static std::size_t get_precision(const T& x);
    static std::string to_string(const T& x);
    static bool isfinite(const T& x);
    static bool isnan(const T& x);
    static bool isinf(const T& x);
    static T epsilon(std::size_t precision = std::size_t{0});
    static T numeric_max(std::size_t precision = std::size_t{0});
};

#include "fprecision/utils_helpers_fprecision.hpp"
#include "interval/utils_helpers_interval.hpp"
#include "mpreal/utils_helpers_mpreal.hpp"
#include "operation_counter/utils_helpers_op_counter.hpp"
#include "std_complex/utils_helpers_std_complex.hpp"
#include "std_floating_point/utils_helpers_std_floating_point.hpp"
#include "std_integral/utils_helpers_std_integral.hpp"

#endif