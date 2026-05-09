#ifndef DRUMMOND_D_ALGORITHM_HPP
#define DRUMMOND_D_ALGORITHM_HPP
#pragma once
/**
 * @file drummond_d_algorithm.hpp
 * @brief Contains implementation of Drummond's D-transformation for sequence acceleration.
 *
 * For theory, see:
 * Drummond, J.E. (1976). A method for the summation of slowly convergent series.
 * Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications, Section 9.5.
 * @authors Naumov A.U.
 */

namespace shanks {
namespace algos {

/**
 * @brief Drummond's D-transformation class template for accelerating slowly convergent series.
 *
 * This transformation is particularly effective for alternating series and sequences
 * with specific remainder behavior patterns. It uses different remainder variants
 * to handle various types of slowly convergent sequences. The algorithm provides
 * both direct and recursive implementations to balance between simplicity and efficiency.
 *
 * References:
 * - Drummond, J.E. (1976). A method for the summation of slowly convergent series.
 * - Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
 * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 * @authors Naumov A.U.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all mathematical computations and storage
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and order specification
 *           Valid values: K >= 0, typically size_t or unsigned int
 */
template <AcceptedLike T, UnsignedIntLike K>
class drummond_d_algorithm final : public series_acceleration<T, K> {
public:
    /// Unique pointer to the remainder estimator strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;
    /// The specific type of remainder variant currently active.
    shanks::remainders::remainder_type remainder_type_in_use{shanks::remainders::remainder_type::u_type};
    /// Configuration flag: if true, uses recursive formulas for calculation; otherwise uses direct formula.
    bool use_recurrent_formula{false};

    /**
     * @brief Calculates D-transformation directly using the explicit formula.
     *
     * For theory, see: Drummond (1976), Eq. (2.1) and Sidi (2003), Section 9.5-4
     * D_n^{(k)} = [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j} S_{n+j}] / [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j}]
     * where w_{n,j} is the remainder estimator function.
     * @authors Naumov A.U.
     *
     * @param n The starting index for partial sums (S_n)
     *        Valid values: n >= 0, determines the starting point of transformation
     * @param order The order of transformation (number of terms to use)
     *        Valid values: order >= 0, higher orders use more terms but may provide better acceleration
     * @return The accelerated partial sum after D-transformation
     * @throws std::overflow_error if division by zero occurs
     */
    inline T calc_result(const K n, const K order, const series_result<T>& data) const;

    /**
     * @brief Calculates D-transformation using recurrence relations for improved efficiency.
     *
     * For theory, see: Sidi (2003), Section 9.5-5
     * Implements a recursive scheme for Drummond's transformation, which is typically
     * more numerically stable and efficient for higher transformation orders.
     * It utilizes forward difference operations on remainder-weighted terms.
     * @authors Naumov A.U.
     *
     * @param n The starting index for partial sums (S_n)
     *        Valid values: n >= 0, determines the starting point of transformation
     * @param order The order of transformation (number of terms to use)
     *        Valid values: order >= 0, higher orders use more terms but may provide better acceleration
     * @return The accelerated partial sum after D-transformation
     * @throws std::overflow_error if division by zero occurs
     */
    inline T calc_result_rec(const K n, const K order, const series_result<T>& data) const;

    /**
     * @brief Parameterized constructor to initialize Drummond's D-algorithm.
     * @param variant Type of remainder estimator to use
     *        Determines the specific variant of Drummond's transformation:
     *        - u_type: Standard remainder estimator
     *        - t_type: Alternative remainder estimator
     *        - v_type: Alternative remainder estimator
     *        - t_wave_type: Modified remainder estimator
     *        - v_wave_type: Modified remainder estimator
     * @param use_recurrent_formula Flag indicating whether to use recurrence formulas
     *        true: Use recursive computation (better for large orders)
     *        false: Use direct computation (simpler but potentially slower)
     * @authors Naumov A.U.
     */
    explicit drummond_d_algorithm(
        const shanks::remainders::remainder_type remainder_type_to_use = shanks::remainders::remainder_type::u_type,
        const bool use_recurrent_formula = false)
        : series_acceleration<T, K>(), use_recurrent_formula(use_recurrent_formula) {
        update_type(remainder_type_to_use);
    };

    /**
     * @brief Executes Drummond's D-transformation to accelerate series convergence.
     *
     * This method acts as the entry point for the transformation, delegating the work
     * to either the direct or recursive implementation based on the configuration.
     *
     * For theory, see: Drummond (1976), Main Theorem and Sidi (2003), Theorem 9.5.1
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

    /**
     * @brief Updates the remainder estimator type used by the algorithm.
     *
     * Re-initializes the internal remainder estimator pointer with the specified variant.
     *
     * @authors Naumov A.U.
     * @param remainder_type_to_use The new remainder variant to employ.
     */
    void update_type(const remainders::remainder_type remainder_type_to_use) {
        remainder_type_in_use = remainder_type_to_use;

        // Re-instantiate the remainder strategy based on the requested type
        switch (remainder_type_to_use) {
            case shanks::remainders::remainder_type::u_type: {
                remainder.reset(new shanks::remainders::u_transform<T, K>());
                break;
            }
            case shanks::remainders::remainder_type::t_type: {
                remainder.reset(new shanks::remainders::t_transform<T, K>());
                break;
            }
            case shanks::remainders::remainder_type::v_type: {
                remainder.reset(new shanks::remainders::v_transform<T, K>());
                break;
            }
            case shanks::remainders::remainder_type::t_wave_type: {
                remainder.reset(new shanks::remainders::t_wave_transform<T, K>());
                break;
            }
            case shanks::remainders::remainder_type::v_wave_type: {
                remainder.reset(new shanks::remainders::v_wave_transform<T, K>());
                break;
            }
            default: {
                remainder_type_in_use = shanks::remainders::remainder_type::u_type;
                remainder.reset(new shanks::remainders::u_transform<T, K>());
            }
        }
    }

    /**
     * @brief Returns the descriptive name of the specific Drummond variant currently in use.
     * @authors Naumov A.U.
     * @return std::string A string containing the variant name and configuration details.
     */
    std::string get_name() override {
        series_acceleration<T, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
        series_acceleration<T, K>::acceleration_name += "drummond d algorithm ";
        switch (remainder_type_in_use) {
            case shanks::remainders::remainder_type::u_type: {
                series_acceleration<T, K>::acceleration_name += "with u-variant ";
                break;
            }
            case shanks::remainders::remainder_type::t_type: {
                series_acceleration<T, K>::acceleration_name += "with t-variant";
                break;
            }
            case shanks::remainders::remainder_type::v_type: {
                series_acceleration<T, K>::acceleration_name += "with v-variant";
                break;
            }
            case shanks::remainders::remainder_type::t_wave_type: {
                series_acceleration<T, K>::acceleration_name += "with t-wave-variant";
                break;
            }
            case shanks::remainders::remainder_type::v_wave_type: {
                series_acceleration<T, K>::acceleration_name += "with v-wave-variant";
                break;
            }
            default: {
                remainder_type_in_use = shanks::remainders::remainder_type::u_type;
                remainder.reset(new shanks::remainders::u_transform<T, K>());
            }
        }

        return series_acceleration<T, K>::acceleration_name;
    }

    inline std::size_t how_much(const std::size_t n, const std::size_t order) const {
        return n + order + std::size_t(1) +
               std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::t_wave_type ||
                           remainder_type_in_use == shanks::remainders::remainder_type::v_type) +
               std::size_t(2) * std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::v_wave_type);
    }
};

template <AcceptedLike T, UnsignedIntLike K>
inline T drummond_d_algorithm<T, K>::calc_result(const K n, const K order, const series_result<T>& data) const {
    const std::size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    T numerator, denominator, rest;
    numerator = denominator = rest = utils::cast<T, int>()(0, precision);

    // For theory, see: Drummond (1976), Eq. (2.1)
    // D_n^{(k)} = [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j} S_{n+j}] / [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j}]
    // Core loop for the direct Drummond D-transformation formula
    for (K j = static_cast<K>(0); j <= order; ++j) {
        // Compute weight term: (-1)^j * C(n, j) * w_{n,j}
        rest = utils::math<T>::template minus_one_raised_to_power_n<K>(j);
        rest *= utils::cast<T, K>()(utils::math<K>::binomial_coefficient(order, j), precision);
        // Call the remainder strategy object
        rest *= remainder->operator()(n + j, n + j, data.an);

        // Accumulate weighted partial sums and total weight
        numerator += rest * data.Sn.at(n + j);
        denominator += rest;
    }

    // Final normalization step
    numerator /= denominator;

    return numerator;
}

template <AcceptedLike T, UnsignedIntLike K>
inline T drummond_d_algorithm<T, K>::calc_result_rec(const K n, const K order, const series_result<T>& data) const {
    const std::size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    // For theory, see: Sidi (2003), Section 9.5-5
    // Temporary vectors to store numerator and denominator coefficients for the recursive scheme
    std::vector<T> Num = std::vector<T>(order + static_cast<K>(1), utils::cast<T, int>()(0, precision));
    std::vector<T> Denom = std::vector<T>(order + static_cast<K>(1), utils::cast<T, int>()(0, precision));

    // Initialize base values: N_j^{(0)} = w_{n,j} S_{n+j}, D_j^{(0)} = w_{n,j}
    for (K i = static_cast<K>(0); i < order + static_cast<K>(1); ++i) {
        Denom[i] += remainder->operator()(n + i, n + i, data.an);
        Num[i] += data.Sn.at(n + i) * Denom[i];
    }

    // Apply forward difference recurrence:
    // N_j^{(i)} = N_{j+1}^{(i-1)} - N_j^{(i-1)}
    // D_j^{(i)} = D_{j+1}^{(i-1)} - D_j^{(i-1)}
    for (K i = static_cast<K>(1); i <= order; ++i)
        for (K j = static_cast<K>(0); j <= order - i; ++j) {
            Denom[j] = Denom[j + static_cast<K>(1)] - Denom[j];
            Num[j] = Num[j + static_cast<K>(1)] - Num[j];
        }

    // Final result: D_n^{(order)} = N_0^{(order)} / D_0^{(order)}
    Num[0] /= Denom[0];
    return Num[0];
}

template <AcceptedLike T, UnsignedIntLike K>
T drummond_d_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Calculate minimum required size based on the chosen remainder variant
    const std::size_t required_size = drummond_d_algorithm<T, K>::how_much(n, order);

    if (data.Sn.size() < required_size || data.an.size() < required_size) {
        throw std::out_of_range("The Sn or an smaller then required for D_{" + utils::helpers<K>::to_string(order) +
                                "}^{" + utils::helpers<K>::to_string(n) + "}\n" +
                                "the size of Sn and an must be at least " +
                                utils::helpers<size_t>::to_string(required_size));
    }

    // Trivial case: order 0 returns the original partial sum
    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    // Branch to selected implementation strategy
    const T result = (use_recurrent_formula ? calc_result_rec(n, order, data) : calc_result(n, order, data));

    if (!utils::helpers<T>::isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

}  // namespace algos
}  // namespace shanks

#endif
