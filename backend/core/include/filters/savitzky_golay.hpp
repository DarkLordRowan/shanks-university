#ifndef SAVITZKY_GOLAY_HPP
#define SAVITZKY_GOLAY_HPP
#pragma once

/**
 * @file savitzky_golay.hpp
 * @brief Savitzky-Golay filter implementation.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace filters {

/**
 * @file savitzky_golay.hpp
 * @brief Calculates Savitzky-Golay filter for given data
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param data (std::vector<Scalar>), window_length (size_t), polyorder (size_t), derive (size_t), delta (Scalar)
 * @return std::vector<Scalar> (filtered data)
 * @throws std::invalid_argument if window_length > data.size() or polyorder >= window_length
 */
template <AcceptedLike Scalar>
std::vector<Scalar> savitzky_golay_filter(const std::vector<Scalar>& data, size_t window_length,
                                          size_t polyorder,    // order of approximation polynomial
                                          size_t derive,       // order of differentiation
                                          const Scalar& delta  // spacing between points
) {
    // Validating filter parameters
    if (window_length > data.size()) throw std::invalid_argument("window's length is bigger than data's size");
    if (polyorder >= window_length) throw std::invalid_argument("polyorder is bigger or equal than window's length");
    const size_t precision = utils::helpers<Scalar>::get_precision(data.at(0));

    // Setting up the least squares problem using Eigen
    Scalar N_val = utils::cast<Scalar, int>()(static_cast<int>(window_length) / 2, precision);
    Eigen::Vector<Scalar, Eigen::Dynamic> v(window_length);
    for (size_t i = 0; i < window_length; ++i) {
        v[i] = -N_val + utils::cast<Scalar, int>()(static_cast<int>(i), precision);
    }

    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> x =
        Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Ones(window_length, polyorder + 1);

    // Constructing the Vandermonde matrix
    for (size_t i{1}; i <= polyorder; ++i) {
        for (size_t r = 0; r < window_length; ++r) {
            x(r, i) = x(r, i - 1) * v(r);
        }
    }

    // Calculating the pseudo-inverse to find polynomial coefficients
    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> coeff_mat = (x.transpose() * x).inverse() * x.transpose();

    // Computing the final filter coefficients
    Eigen::RowVector<Scalar, Eigen::Dynamic> coeffs =
        utils::cast<Scalar, size_t>()(utils::math<size_t>::fact(derive), precision) * coeff_mat.row(derive) /
        utils::math<Scalar>::pow(delta, utils::cast<Scalar, size_t>()(derive, precision));


    const size_t N_pad = window_length / 2;
    std::vector<Scalar> padded_vector(data.size() + N_pad * 2, utils::cast<Scalar, int>()(0, precision));
    std::vector<Scalar> result(data.size(), utils::cast<Scalar, int>()(0, precision));
    // Convolution with padding: padding with the first and the second element
    std::copy(data.begin(), data.end(), padded_vector.begin() + N_pad);
    for (size_t i{0}; i < N_pad; ++i) padded_vector[i] = data.front();
    for (size_t i{0}; i < N_pad; ++i) padded_vector[padded_vector.size() - N_pad - 1 + i] = data.back();

    // Applying the filter via convolution
    for (size_t i{0}; i < result.size(); ++i)
        for (size_t j{0}; j < window_length; ++j) result[i] += coeffs[j] * padded_vector[i + j];

    return result;
}

}  // namespace filters
}  // namespace shanks

#endif
