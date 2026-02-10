#ifndef LEVIN_ALGORITHM_HPP
#define LEVIN_ALGORITHM_HPP
#pragma once
/**
 * @file levin_algorithm.hpp
 * @brief This file contains the declaration of the Levin algorithm class.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * For theory, see:
 * Levin, D. (1973). Development of non-linear transformations for improving convergence of sequences.
 *   Intern. J. Computer Math., B3, 371-388.
 * Sidi, A. (1979). Convergence properties of some nonlinear sequence transformations.
 *   Math. Comp., 33, 315-326.
 * Sidi, A., & Levin, D. (1981). Two new classes of nonlinear transformations for accelerating
 *   the convergence of infinite integrals and series. Appl. Math. Comp., 9, 175-215.
 */

namespace shanks {
namespace algos {

/**
 * @brief Levin Algorithm class template implementing various Levin transformations.
 *
 * This class implements the Levin transformation for series acceleration, which is particularly
 * effective for sequences with specific asymptotic behaviors. The algorithm comes in several
 * variants (u, t, v, t~, v~) that use different remainder estimates. It supports both
 * direct summation and recursive computation schemes.
 *
 * References:
 * - Levin, D. (1973). Development of non-linear transformations for improving convergence of sequences.
 * - Sidi, A. (1979). Convergence properties of some nonlinear sequence transformations.
 * - Sidi, A., & Levin, D. (1981). Two new classes of nonlinear transformations.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike)
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class levin_algorithm final : public series_acceleration<T, K> {
protected:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /// Parameter for u-variant transformation (β > 0). Default value is 1.0.
    float_type beta_in_use = utils::cast<float_type, int>()(1);

    /// Pointer to the remainder transformation strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;

    /// Flag to use recurrence formulas (true) or direct formulas (false)
    bool use_recurrent_formula{false};

    /// The specific Levin transformation variant (u, t, v, t~, v~) currently active.
    shanks::remainders::remainder_type remainder_type_in_use{shanks::remainders::remainder_type::u_type};

    /**
     * @brief Computes the Levin transformation using direct summation formulas.
     *
     * For theory, see: Levin (1973), Eq. (2.3) and Sidi (1979), Eq. (2.1)
     * General form: T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
     *                      [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param n Base index for the transformation.
     * @param order Order of the transformation (k).
     * @param data Series data structure containing partial sums.
     * @return T Accelerated sum estimate.
     */
    inline T calc_result(const K n, const K order, const series_result<T>& data) const;

    /**
     * @brief Computes the Levin transformation using recursive formulas for improved stability.
     *
     * For theory, see: Sidi (1979), Section 3 and Brezinski's E-algorithm implementation
     * Recursive implementation for better numerical stability in some cases.
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param n Base index for the transformation.
     * @param order Order of the transformation (k).
     * @param data Series data structure containing partial sums.
     * @return T Accelerated sum estimate.
     */
    inline T calc_result_rec(const K n, const K order, const series_result<T>& data) const;

public:
    /**
     * @brief Parameterized constructor to initialize the Levin Algorithm.
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
     * @param remainder_type_to_use Type of Levin transformation variant to use
     *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
     *        Determines the remainder estimate R_n used in the transformation
     * @param use_recurrent_formula Flag to use recurrence formulas instead of direct summation
     *        true: use recursive implementation, false: use direct summation
     * @param beta_to_use Parameter for u-variant transformation (must be > 0), see p. 39
     * [https://arxiv.org/pdf/math/0306302.pdf] Default value: 1.0. Affects the remainder estimate in u-variant. For
     * theory, see: Sidi & Levin (1981), Eq. (3.4) and surrounding discussion
     */
    explicit levin_algorithm(
        const shanks::remainders::remainder_type remainder_type_to_use = shanks::remainders::remainder_type::u_type,
        const bool use_recurrent_formula = false, const float_type& beta_to_use = utils::cast<float_type, int>()(1));

    /**
     * @brief Implementation of Levin transformation for series acceleration.
     *
     * Computes the accelerated sum using the specified Levin transformation variant.
     * The algorithm can use either direct summation or recurrence formulas based on constructor setting.
     *
     * For theory, see:
     * - General framework: Levin (1973), Eq. (2.3)
     * - Convergence properties: Sidi (1979), Theorems 3.1, 4.2
     * - Variant-specific properties: Sidi & Levin (1981), Sections 3-4
     * - More information, see 3.9.13 in[https://dlmf.nist.gov/3.9]
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     *
     * @param n The number of terms to use in the transformation
     *        Valid values: n > 0 (algorithm requires at least 1 term)
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (k value)
     *        Valid values: order >= 0
     *        Higher orders eliminate more terms from the asymptotic expansion but may be less stable
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum after Levin transformation
     * @throws std::domain_error if n=0 is provided as input
     * @throws std::overflow_error if division by zero or numerical instability occurs
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

    /**
     * @brief Setter to update beta parameter
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param new_beta new beta parameter, must be real positive number, see p. 39
     * [https://arxiv.org/pdf/math/0306302.pdf]
     */
    void update_beta(const float_type& new_beta) {
        beta_in_use = (new_beta > utils::cast<float_type, int>()(0) ? new_beta : utils::cast<float_type, int>()(1));
    }

    /**
     * @brief Changes the remainder estimator variant.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param remainder_type_to_use The new remainder variant (u, t, v, etc.).
     */
    void update_type(const shanks::remainders::remainder_type remainder_type_to_use) {
        remainder_type_in_use = remainder_type_to_use;

        // Re-initialize the remainder strategy based on the selection
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
     * @brief Retrieves the descriptive name of the currently active Levin variant.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return std::string The name and current configuration of the algorithm.
     */
    std::string get_name() override {
        series_acceleration<T, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
        series_acceleration<T, K>::acceleration_name += "levin l algorithm ";
        switch (remainder_type_in_use) {
            case shanks::remainders::remainder_type::u_type: {
                series_acceleration<T, K>::acceleration_name += "with u-variant ";
                break;
            }
            case shanks::remainders::remainder_type::t_type: {
                series_acceleration<T, K>::acceleration_name += "with t-variant ";
                break;
            }
            case shanks::remainders::remainder_type::v_type: {
                series_acceleration<T, K>::acceleration_name += "with v-variant ";
                break;
            }
            case shanks::remainders::remainder_type::t_wave_type: {
                series_acceleration<T, K>::acceleration_name += "with t-wave-variant ";
                break;
            }
            case shanks::remainders::remainder_type::v_wave_type: {
                series_acceleration<T, K>::acceleration_name += "with v-wave-variant ";
                break;
            }
            default: {
                remainder_type_in_use = shanks::remainders::remainder_type::u_type;
                remainder.reset(new shanks::remainders::u_transform<T, K>());
            }
        }
        series_acceleration<T, K>::acceleration_name += "and beta = " + utils::helpers<T>::to_string(beta_in_use);

        return series_acceleration<T, K>::acceleration_name;
    }
};

template <AcceptedLike T, UnsignedIntLike K>
levin_algorithm<T, K>::levin_algorithm(const shanks::remainders::remainder_type remainder_type_to_use,
                                       const bool use_recurrent_formula, const float_type& beta_to_use)
    : series_acceleration<T, K>(), use_recurrent_formula(use_recurrent_formula) {
    update_beta(beta_to_use);
    update_type(remainder_type_to_use);
}

template <AcceptedLike T, UnsignedIntLike K>
inline T levin_algorithm<T, K>::calc_result(const K n, const K order, const series_result<T>& data) const {
    const size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    T numerator, denominator, rest;
    float_type C_njk;
    numerator = denominator = rest = utils::cast<T, int>()(0, precision);
    C_njk = utils::cast<float_type, int>()(0, precision);

    // Compute (-1)^j * C(k,j)
    // For theory, see: Levin (1973), Eq. (2.3)
    // T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
    //           [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
    // Main loop for the direct Levin transformation formula
    for (K j = static_cast<K>(0); j <= order; ++j) {
        // Compute (-1)^j * C(k,j) - the sign and binomial coefficient part
        rest += -rest + utils::math<T>::template minus_one_raised_to_power_n<K>(j);
        rest *= utils::cast<T, K>()(utils::math<K>::binomial_coefficient(order, j), precision);

        // Compute (n+j+1)^{k-1}/(n+k+1)^{k-1} - the weighting factors C_njk
        C_njk = utils::math<float_type>::pow(
            beta_in_use + utils::cast<float_type, K>()(n + j + static_cast<K>(1), precision),
            utils::cast<float_type, K>()(order - static_cast<K>(1), precision));
        C_njk /= utils::math<float_type>::pow(
            beta_in_use + utils::cast<float_type, K>()(n + order + static_cast<K>(1), precision),
            utils::cast<float_type, K>()(order - static_cast<K>(1), precision));

        // Compute 1/R_{n+j} where R_{n+j} is the remainder estimate
        rest *= remainder->operator()(
            n + j, n + j, data.an,
            utils::cast<T, float_type>()((remainder_type_in_use == shanks::remainders::remainder_type::u_type
                                              ? beta_in_use
                                              : utils::cast<float_type, int>()(1, precision))));

        rest *= C_njk;

        // Accumulate weighted partial sums and total weights
        denominator += rest;
        numerator += rest * data.Sn.at(n + j);
    }

    // Normalization yields the accelerated sum estimate
    numerator /= denominator;

    return numerator;
}

template <AcceptedLike T, UnsignedIntLike K>
inline T levin_algorithm<T, K>::calc_result_rec(const K n, const K order, const series_result<T>& data) const {
    const size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    // For theory, see: Sidi (1979), Section 3 - Recursive implementation using E-algorithm
    // Initialize auxiliary vectors for the recursive scheme
    std::vector<T> Num(order + static_cast<K>(1), utils::cast<T, int>()(0, precision));
    std::vector<T> Denom(order + static_cast<K>(1), utils::cast<T, int>()(0, precision));
    float_type scale = utils::cast<float_type, int>()(0, precision);

    // Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
    // Order 0 transformations correspond to weighted terms
    for (K i = static_cast<K>(0); i < order + static_cast<K>(1); ++i) {
        Denom[i] += remainder->operator()(
            n + i, n + i, data.an,
            utils::cast<T, float_type>()(remainder_type_in_use == shanks::remainders::remainder_type::u_type
                                             ? beta_in_use
                                             : utils::cast<float_type, int>()(1, precision),
                                         precision));

        Num[i] += data.Sn.at(n + i) * Denom[i];
    }

    // Recursive refinement using a scheme similar to the E-algorithm
    for (K i = static_cast<K>(1); i <= order; ++i)
        for (K j = static_cast<K>(0); j <= order - i; ++j) {
            // For theory, see: Brezinski's E-algorithm recurrence
            // E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
            // Scaling factor for the recursive update
            scale = (beta_in_use + utils::cast<float_type, K>()(n + j, precision));
            scale *= utils::math<float_type>::pow(
                utils::cast<float_type, int>()(1, precision) -
                    utils::cast<float_type, int>()(1, precision) /
                        (beta_in_use + utils::cast<float_type, K>()(n + j + i + 1, precision)),
                utils::cast<float_type, K>()(i));
            scale /= (beta_in_use + utils::cast<float_type, K>()(n + j + i, precision));

            Denom[j] =
                utils::math<T>::fma(utils::cast<T, float_type>()(-scale), Denom[j], Denom[j + static_cast<K>(1)]);
            Num[j] = utils::math<T>::fma(utils::cast<T, float_type>()(-scale), Num[j], Num[j + static_cast<K>(1)]);
        }

    // Final normalization
    Num[0] /= Denom[0];

    return Num[0];
}

template <AcceptedLike T, UnsignedIntLike K>
T levin_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Calculate required vector sizes based on the variant
    const K required_size =
        n + order + static_cast<K>(1) +
        static_cast<K>(remainder_type_in_use == shanks::remainders::remainder_type::t_wave_type ||
                       remainder_type_in_use == shanks::remainders::remainder_type::v_type) +
        static_cast<K>(2) * static_cast<K>(remainder_type_in_use == shanks::remainders::remainder_type::v_wave_type);

    if (data.Sn.size() < required_size || data.an.size() < required_size) {
        throw std::out_of_range("The Sn or an smaller then required for L_{" + utils::helpers<K>::to_string(order) +
                                "}^{" + utils::helpers<K>::to_string(n) + "}\n" +
                                "the size of Sn and an must be at least " +
                                utils::helpers<size_t>::to_string(required_size));
    }

    if (order == static_cast<K>(0)) return data.Sn.at(n);

    const T result = (use_recurrent_formula ? calc_result_rec(n, order, data) : calc_result(n, order, data));

    if (!utils::helpers<T>::isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

}  // namespace algos
}  // namespace shanks

#endif
