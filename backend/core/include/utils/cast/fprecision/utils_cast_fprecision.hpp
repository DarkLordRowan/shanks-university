#ifndef UTILS_CAST_FPRECISION_HPP
#define UTILS_CAST_FPRECISION_HPP
#pragma once

#ifdef INC_FPRECISION

template <typename From>
    requires std::floating_point<From> || std::integral<From>
struct utils::cast<arb::float_precision, From> {
    arb::float_precision operator()(const From& x, size_t precision = size_t{0});
};

template <typename From>
    requires std::floating_point<From> || std::integral<From>
inline arb::float_precision utils::cast<arb::float_precision, From>::operator()(const From& x, size_t precision) {
    if (precision > 0)
        if constexpr (std::is_same<From, long double>::value)
            return arb::float_precision(static_cast<double>(x), precision);
        else
            return arb::float_precision(x, precision);
    else if constexpr (std::is_same<From, long double>::value)
        return arb::float_precision(static_cast<double>(x), precision);
    else
        return arb::float_precision(x, precision);
}

template <typename To>
    requires std::floating_point<To> || std::integral<To>
struct utils::cast<To, arb::float_precision> {
    To operator()(const arb::float_precision& x, size_t precision = size_t{0});
};

template <typename To>
    requires std::floating_point<To> || std::integral<To>
inline To utils::cast<To, arb::float_precision>::operator()(const arb::float_precision& x,
                                                            [[maybe_unused]] size_t precision) {
    if constexpr (std::is_same<To, long double>::value)
        return static_cast<long double>(static_cast<double>(x));
    else
        return static_cast<To>(x);
}

#endif

#endif