#ifndef UTILS_HPP
#define UTILS_HPP
#pragma once

/**
 * @file utils.hpp
 * @brief This file contains the #include directives for all utility modules.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

#include <string>
#include <vector>

#include "custom_concepts.hpp"
/**
 * @file utils_base.hpp
 * @brief This file contains the base utility definitions and the main utils struct.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Structure for more convenient passing and receiving data between algos and series
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T AcceptedLike Type
 */
template <AcceptedLike T>
struct series_result {
    std::vector<T> Sn; /**< vector of partial sums S_{n} of type T*/
    std::vector<T> an; /**< vector of a_{n} = S_{n} - S_{n-1} */
};

/**
 * @brief Get the value_type if the type has one, otherwise get the original type
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Input type
 */
template <typename T>
struct GetUnderlyingType {
    using value = T;
};

template <typename T>
struct GetUnderlyingType<std::complex<T>> {
    using value = T;
};

template <typename T>
struct GetUnderlyingType<intprec::interval<T>> {
    using value = T;
};

template <typename T>
struct real_of {
    using value = T;
};

template <FloatLike T>
struct real_of<std::complex<T>> {
    using value = T;
};

#include <type_traits>

#include "custom_concepts.hpp"
#include "custom_types/intervalprecision.hpp"

// #include "gsl/gsl_sf_expint.h"
// #include <gsl/gsl_sf_gamma.h>
// #include <gsl/gsl_sf_lambert.h>

#include "utils/utils_cast.hpp"
#include "utils/utils_get_precision.hpp"
#include "utils/utils_helper.hpp"
#include "utils/utils_math.hpp"

// Core utility headers

#endif