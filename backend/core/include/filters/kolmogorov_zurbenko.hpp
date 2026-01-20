#ifndef KOLMOGOROV_ZURBENKO_HPP
#define KOLMOGOROV_ZURBENKO_HPP
#pragma once

#include "../custom_concepts.hpp"
#include <vector>
#include "../utils/utils_get_precision.hpp"
#include "../utils/utils_cast.hpp"

/**
 * @file kolmogorov_zurbenko.hpp
 * @brief Kolmogorov-Zurbenko filter implementation.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks{
namespace filters{

/**
 * @brief Calculates Kolmogorov-Zurbenko filter for given data
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param data (std::vector<Scalar>), window_length (size_t), degree (size_t)
 * @return std::vector<Scalar> (filtered data)
 */
template<typename Scalar>
requires AcceptedLike<Scalar> || std::is_integral<Scalar>::value
std::vector<Scalar> kolmogorov_zurbenko_filter(
    const std::vector<Scalar>& data,
    size_t window_length /*m*/,
    size_t degree /*k*/
) {
    const size_t m = window_length, k = degree;
    const size_t size = (m-1)*k + 1;
    const size_t precision = utils::get_precision(data.at(0));
    std::vector<int> coeffs = std::vector<int>(size, 0);
    coeffs[0] = coeffs[size - 1] = 1;
    
    // Iteratively calculating coefficients based on binomial formulas
    const size_t middle_coeff = (size + size % 2) / size_t{2};
    for(size_t l{1}; l < middle_coeff; ++l){
        for (size_t i{0}; m * i <= l; ++i){
            const size_t j = (l > m * i ? l - m*i : 0);
            coeffs[l] += (1 - 2 * static_cast<int>(i&1)) * utils::binomial_coefficient(k, i) * utils::binomial_coefficient(k + j - 1, j);
            coeffs[size - 1 - l] = coeffs[l];
        }
    }

    // Preparing vectors for convolution and handling arbitrary precision
    std::vector<Scalar> zur_coeffs(size + 1, utils::cast<Scalar>(0.0, precision));
    std::vector<Scalar> padded_vector(data.size() + size * 2, utils::cast<Scalar>(0.0, precision));
    std::vector<Scalar> result(data.size(), utils::cast<Scalar>(0.0, precision));

    // Convolution with padding: adding 0 at the start, rest 0 on the end
    std::copy(data.begin(), data.end(), padded_vector.begin()+1);
    
    // Normalizing coefficients for the filter
    for (size_t i{0}; i < coeffs.size(); ++i) {
        zur_coeffs[i] += utils::cast<Scalar>(coeffs[i], precision);
        zur_coeffs[i] /= utils::cast<Scalar>(utils::pow(m,k), precision);
    }
    
    // Applying the convolution to produce the filtered result
    for (size_t i{0}; i < result.size(); ++i)
        for(size_t j{0}; j < size; ++j)
            result[i] += zur_coeffs[j] * padded_vector[i + j];

    return result;
}

} //namespace shanks::filters
} //namespace shanks

#endif