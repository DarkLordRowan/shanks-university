#ifndef SAVGOL_HPP
#define SAVGOL_HPP
#pragma once

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <stdexcept>
#include "../custom_concepts.hpp"
#include "../utils/utils_cast.hpp"

/**
 * @file savgol.hpp
 * @brief Savitzky-Golay filter implementation.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks{
namespace filters{

/**
 * @file savgol.hpp
 * @brief Calculates Savitzky-Golay filter for given data
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param data (std::vector<Scalar>), window_length (size_t), polyorder (size_t), derive (size_t), delta (Scalar)
 * @return std::vector<Scalar> (filtered data)
 * @throws std::invalid_argument if window_length > data.size() or polyorder >= window_length
 */
template<AcceptedLike Scalar>
std::vector<Scalar> savgol_filter(
    const std::vector<Scalar>& data,
    size_t window_length,
    size_t polyorder, //order of approximation polynomial
    size_t derive, // order of differentiation
    const Scalar& delta //spacing between points
){

    // Validating filter parameters
    if (window_length > data.size()) throw std::invalid_argument("window's length is bigger than data's size");
    if (polyorder >= window_length) throw std::invalid_argument("polyorder is bigger or equal than window's length");
    const size_t precision = utils::get_precision(data.at(0));

    // Setting up the least squares problem using Eigen
    Scalar N = utils::cast<Scalar>(static_cast<int>(window_length) / 2, precision);

    Eigen::Vector<Scalar, Eigen::Dynamic> v = Eigen::Vector<Scalar, Eigen::Dynamic>::LinSpaced(window_length, -N, N);
    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> x = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Ones(window_length, polyorder + 1);

    // Constructing the Vandermonde matrix
    for (size_t i{1}; i <= polyorder; ++i)
        x.col(i) = (x.col(i-1).array() * v.array()).matrix();

    // Calculating the pseudo-inverse to find polynomial coefficients
    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> coeff_mat = (x.transpose() * x).inverse() * x.transpose();

    // Computing the final filter coefficients
    Eigen::RowVector<Scalar, Eigen::Dynamic> coeffs = utils::cast<Scalar>(utils::fact(derive), precision) * coeff_mat.row(derive) / utils::pow(delta, utils::cast<Scalar>(derive, precision));

    // Convolution with padding: adding 0 at the start, rest 0 on the end
    std::vector<Scalar> padded_vector(data.size() + (window_length - 1) * 2, utils::cast<Scalar>(0.0, precision));
    std::vector<Scalar> result(data.size(), utils::cast<Scalar>(0.0, precision));
    std::copy(data.begin(), data.end(), padded_vector.begin()+1);

    // Applying the filter via convolution
    for (size_t i{0}; i < result.size(); ++i)
        for(size_t j{0}; j < window_length; ++j)
            result[i] += coeffs[j] * padded_vector[i + j];

    return result;
}

} //namespace shanks::filters
} //namespace shanks

#endif
