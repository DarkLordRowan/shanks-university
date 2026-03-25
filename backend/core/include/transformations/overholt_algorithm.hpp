#ifndef OVERHOLT_ALGORITHM_HPP
#define OVERHOLT_ALGORITHM_HPP
#pragma once

/**
 * @file overholt_algorithm.hpp
 * @brief Contains implementation of Overholt Process for sequence acceleration.
 *
 * For theory, see:
 * Herbert, H. H. Homeier(2018) SCALAR LEVIN-TYPE SEQUENCE TRANSFORMATIONS
 * @authors Naumov A.U.
 */

namespace shanks {
namespace algos {

template <AcceptedLike T, UnsignedIntLike K>
class overholt_algorithm final : public series_acceleration<T, K> {
protected:
    using float_type = real_of<T>::value;  // type in case of complex or interval

public:
    /**
     * @brief Parameterized constructor to initialize Overholt Process.
     * @authors Naumov A.U.
     */
    explicit overholt_algorithm() : series_acceleration<T, K>("overholt process") {}

    ~overholt_algorithm() = default;  // Default destructor is sufficient since unique_ptr handles deletion

    /**
     * @brief Executes Overholt Process to accelerate series convergence.
     *
     * This method acts as the entry point for the transformation, delegating the work
     * to either the direct or recursive implementation based on the configuration.
     *
     * For theory, see: Herbert, H. H. Homeier(2018) SCALAR LEVIN-TYPE SEQUENCE TRANSFORMATIONS, p. 24
     *
     * @param n The starting index for partial sums (S_n)
     *        Valid values: n >= 0, determines the starting point of transformation
     *        Higher values use more stable terms but may converge slower
     * @param order The order of transformation
     *        Valid values: order >= 0, higher orders use more terms but may provide better acceleration
     *        Typically order <= 10 for numerical stability
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum after Drummond transformation
     * @throws std::overflow_error if division by zero or numerical instability occurs
     * @throws std::out_of_range if the input data vectors are too small.
     * @throws std::overflow_error if numerical instability or division by zero occurs.
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

    static inline std::size_t how_much(const K n, const K order = utils::cast<K, int>()(0)) {
        return n + std::size_t{2} * order + std::size_t{1};
    }
};

template <AcceptedLike T, UnsignedIntLike K>
T overholt_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    const std::size_t required_size = overholt_algorithm<T, K>::how_much(n, order);

    if (data.Sn.size() < required_size || data.an.size() < required_size) {
        throw std::out_of_range("The Sn smaller then required for V_{" + utils::helpers<K>::to_string(order) + "}^{" +
                                utils::helpers<K>::to_string(n) + "}\n" + "the size of Sn must be at least " +
                                utils::helpers<K>::to_string(required_size) + "and size of an - " +
                                utils::helpers<K>::to_string(required_size));
    }

    const std::size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    std::vector<T> V(order + 1, utils::cast<T, int>()(0, precision));
    for (K i{0}; i <= order; ++i) V[i] += data.Sn.at(n + i);

    for (K i{1}; i <= order; ++i) {
        for (K j{0}; j <= order - i; ++j) {
            const T delta_left_s = utils::math<T>::pow(
                data.an.at(n + i + j), utils::cast<T, int>()(i, precision));  /// s_{n + k} - s_{n + k - 1}
            const T delta_right_s = utils::math<T>::pow(
                data.an.at(n + i + j + 1), utils::cast<T, int>()(i, precision));  /// s_{n + k + 1} - s_{n + k}
            const T tmp = (delta_left_s * V[j + 1] - delta_right_s * V[j]) / (delta_left_s - delta_right_s);

            V[j] = (utils::helpers<T>::isfinite(tmp) ? tmp : V[j]);
        }
    }

    // Numerical stability check
    if (!utils::helpers<T>::isfinite(V[0])) throw std::overflow_error("division by zero");

    return V[0];
}

}  // namespace algos
}  // namespace shanks

#endif
