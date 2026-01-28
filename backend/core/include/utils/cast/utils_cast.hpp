#ifndef UTILS_CAST_H
#define UTILS_CAST_H
#pragma once

/**
 * @file utils_cast.hpp
 * @brief This file contains implementation of casting utilities.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Wrapper around std::static_cast (support for precisable types included by precision var)
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (From)
 * @param precision (size_t) desirable precision for precisable type(precision > 0), otherwise static_cast<To>(x) is
 * called
 * @return To (casted value)
 */
template <typename To, typename From>
constexpr To utils::cast(const From& x, const std::size_t precision) {
    // Unwrap if needed (OperationCounting support via ADL_Unwrapper)
    if constexpr (is_operation_counting<From>::value) {
        return utils::cast<To>(ADL_Unwrapper<From>::unwrap(x), precision);
    }
    // Wrap if needed (OperationCounting support via ADL_Wrapper)
    else if constexpr (is_operation_counting<To>::value) {
        using UnderlyingTo = typename get_wrapped_type<To>::type;
        return ADL_Wrapper<To>::wrap(utils::cast<UnderlyingTo>(x, precision));
    }
    // Default static cast for standard types
    else if constexpr (is_standard_types<To>::value)
        return static_cast<To>(x);
    else if constexpr (isComplexLike<To>::value && !isComplexLike<From>::value)
        return std::complex(utils::cast<typename real_of<To>::value>(x));
    // Cast to precisable type
    else if constexpr (is_precisable<To>::value)
        if (precision == size_t{0})
            return static_cast<To>(x);
        else {
            // Cast if mpfr is used
            if constexpr (std::is_same<To, mpfr::mpreal>::value)
                if constexpr (std::is_same<From, mpfr::mpreal>::value)
                    return mpfr::mpreal(x);
                else
                    return mpfr::mpreal(x, mpfr::digits2bits(precision));
            else
                static_assert(dependent_false<To>::value, "utils::cast with precision not implemented for this type");
        }
    // Cast to interval type
    else if constexpr (is_interval<To>::value) {
        if constexpr (isFloatLike<From>::value)
            return static_cast<To>(x);
        else if constexpr (std::is_integral<From>::value && isFloatLike<typename To::value_type>::value)
            return static_cast<To>(utils::cast<typename To::value_type>(x, precision));
        else if constexpr (is_interval<From>::value)
            return static_cast<To>(x);
    } else {
        static_assert(dependent_false<To>::value, "utils::cast not implemented for this type");
    }
}

#endif
