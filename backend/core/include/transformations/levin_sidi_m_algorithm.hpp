#ifndef LEVIN_SIDI_M_ALGORITHM_HPP
#define LEVIN_SIDI_M_ALGORITHM_HPP
#pragma once

#define DEFAULT_GAMMA 100.5

/**
 * @file levin_sidi_m_algorithm.hpp
 * @brief This file contains the definition of analogues of Levin-Sidi M-transformation.
 *
 * For theory, see:
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 *   Cambridge University Press. (Chapter 9, pp. 285-369)
 * Sidi, A. (2003). A new class of nonlinear transformations for accelerating the convergence
 *   of infinite integrals and series. arXiv:math/0306302.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace algos {

/**
 * @brief Levin-Sidi M-transformation class template.
 *
 * This class implements the Levin-Sidi M-transformation, which is particularly effective
 * for series that belong to the b(1)/LIN/FAC classes (factorial and linear convergence).
 * The transformation leverages factorial-like terms and Pochhammer symbols to provide
 * aggressive acceleration for specific sequence patterns.
 *
 * References:
 * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 * - Sidi, A. (2003). A new class of nonlinear transformations. arXiv:math/0306302.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike).
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class levin_sidi_m_algorithm final : public series_acceleration<T, K> {
protected:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /// Positive real parameter gamma. For theoretical stability, it often satisfies gamma >= order - 1.
    float_type gamma_in_use = utils::cast<float_type, double>()(DEFAULT_GAMMA);
    /// Pointer to the remainder transformation strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;

    /// The specific Levin variant variant (u, t, v, etc.) used for remainder estimation.
    shanks::remainders::remainder_type remainder_type_in_use;

public:
    /**
     * @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param variant Type of remainder transformation to use
     *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
     *        Determines the remainder estimate R_n used in the transformation
     * @param gamma_ Positive real positive parameter such that gamma >= order - 1, see p. 64
     * [https://arxiv.org/pdf/math/0306302.pdf] Default value: 10.0. Affects the factorial terms in the transformation.
     *        For theory, see: Sidi (2003, arXiv:math/0306302), p. 64
     */
    explicit levin_sidi_m_algorithm(
        shanks::remainders::remainder_type remainder_type_to_use = shanks::remainders::remainder_type::u_type,
        const float_type& gamma_to_use = utils::cast<float_type, double>()(DEFAULT_GAMMA))
        : series_acceleration<T, K>() {
        update_gamma(gamma_to_use);
        update_type(remainder_type_to_use);
    }

    /**
     * @brief Virtual destructor.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    ~levin_sidi_m_algorithm() = default;  // Default destructor is sufficient since unique_ptr handles deletion

    /**
     * @brief Implementation of Levin-Sidi M-transformation for series acceleration.
     *
     * Computes the accelerated sum using the M-transformation, which is particularly
     * effective for series with factorial or linear convergence patterns.
     *
     * For theory, see:
     * - General framework: Sidi (2003, arXiv:math/0306302), Eqs. (9.2)-(9.6)
     * - Convergence properties: Sidi (2003, Practical Extrapolation Methods), pp. 285, 369
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param n The number of terms to use in the transformation
     *        Valid values: n > 0 (algorithm requires at least 1 term)
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (starting index k)
     *        Valid values: order >= 0
     *        The parameter gamma must satisfy gamma >= order - 1
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum after M-transformation
     * @throws std::out_of_range if the input vectors are too small for the requested calculation.
     * @throws std::domain_error if n=0 or gamma < n-1
     * @throws std::overflow_error if division by zero or numerical instability occurs
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

    /**
     * @brief Updates the remainder estimator variant.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param remainder_type_to_use The new remainder variant to use.
     */
    void update_type(const shanks::remainders::remainder_type remainder_type_to_use);

    /**
     * @brief Updates the gamma parameter.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param new_gamma The new gamma value.
     */
    void update_gamma(const float_type& new_gamma) { gamma_in_use = new_gamma; }

    /**
     * @brief Returns the descriptive name of the currently active Levin-Sidi M variant.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return std::string The name and current configuration of the algorithm.
     */
    std::string get_name() override {
        series_acceleration<T, K>::acceleration_name = "levin sidi m algorithm ";
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
        }
        series_acceleration<T, K>::acceleration_name += "and gamma = " + utils::helpers<T>::to_string(gamma_in_use);

        return series_acceleration<T, K>::acceleration_name;
    }
};

template <AcceptedLike T, UnsignedIntLike K>
void levin_sidi_m_algorithm<T, K>::update_type(const shanks::remainders::remainder_type remainder_type_to_use) {
    remainder_type_in_use = remainder_type_to_use;

    // Re-instantiate the remainder strategy object based on the requested variant
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

template <AcceptedLike T, UnsignedIntLike K>
T levin_sidi_m_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Determine the total number of terms required for both Sn and an vectors
    const K required_size =
        n + order + static_cast<K>(1) +
        static_cast<K>(remainder_type_in_use == shanks::remainders::remainder_type::t_wave_type ||
                       remainder_type_in_use == shanks::remainders::remainder_type::v_type) +
        static_cast<K>(2) * static_cast<K>(remainder_type_in_use == shanks::remainders::remainder_type::v_wave_type);
    const size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    if (data.Sn.size() < required_size || data.an.size() < required_size) {
        throw std::out_of_range("The Sn or an smaller then required for M_{" + utils::helpers<K>::to_string(order) +
                                "}^{" + utils::helpers<K>::to_string(n) + "}\n" +
                                "the size of Sn and an must be at least " +
                                utils::helpers<size_t>::to_string(required_size));
    }

    // Trivial case: order 0 returns the current partial sum
    if (order == static_cast<K>(0)) return data.Sn.at(n);

    // Validate that gamma satisfies the theoretical constraint for stability: gamma >= n - 1
    if (gamma_in_use - utils::cast<float_type, K>()(n) - utils::cast<float_type, int>()(1) <
        utils::cast<float_type, int>()(0)) {
        throw std::domain_error("gamma cannot be lesser than n - 1");
    }

    T numerator, denominator, rest;
    rest = numerator = denominator = utils::cast<T, int>()(0, precision);
    float_type up, down, down_coef, up_coef;
    up = down = utils::cast<float_type, int>()(1, precision);
    down_coef = up_coef = utils::cast<float_type, int>()(0, precision);

    // Precompute initial Pochhammer symbol terms
    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.4)
    // Compute: (γ+k+2)_{n-1}/(γ+k+1)_{n} = Γ(γ+k+n+1)/Γ(γ+k+2) × Γ(γ+k+1)/Γ(γ+k+n+1)
    // Precompute the initial ratio of Pochhammer symbols
    down_coef += gamma_in_use + utils::cast<float_type, K>()(order + static_cast<K>(2), precision);
    up_coef += down_coef - utils::cast<float_type, K>()(n, precision);

    // Compute (γ+k+2)_{n-1} = ∏_{m=0}^{n-2} (γ+k+2+m)
    // Compute (γ+k+1)_{n} = ∏_{m=0}^{n-1} (γ+k+1+m)
    for (K m = static_cast<K>(0); m + static_cast<K>(1) < n; ++m) {
        up *= (up_coef + utils::cast<float_type, K>()(m, precision));
        down *= (down_coef + utils::cast<float_type, K>()(m, precision));
    }
    up /= down;

    // Update coefficients for the inner product terms
    down_coef = gamma_in_use + utils::cast<float_type, K>()(order + static_cast<K>(1), precision);
    up_coef = down_coef - utils::cast<float_type, K>()(n + static_cast<K>(1), precision);
    // Main summation loop
    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.2)
    // Main summation loop for the M-transformation formula
    for (K j = static_cast<K>(0); j <= n; ++j) {
        // Calculate the sign, binomial coefficient, and weight components
        rest = utils::math<T>::template minus_one_raised_to_power_n<K>(j);
        rest *= utils::cast<T, K>()(utils::math<K>::binomial_coefficient(n, j), precision);
        rest *= utils::cast<T, float_type>()(up, precision);            // Multiply by Pochhammer ratio term
        rest /= utils::cast<T, K>()(j + static_cast<K>(1), precision);  // Multiply by 1/(j+1) factor
        up /= (up_coef + utils::cast<float_type, K>()(j, precision));   // Update Pochhammer ratio for next iteration
        up *= (down_coef + utils::cast<float_type, K>()(
                               j, precision));  // (γ+k+1-j)_{j}/(γ+k+2-n)_{j} → (γ+k+1-j)_{j+1}/(γ+k+2-n)_{j+1}
        // Multiply by remainder term 1/R_{k+j}
        rest *= remainder->operator()(
            order + j, order + j, data.an,
            utils::cast<T, float_type>()(-gamma_in_use - utils::cast<float_type, K>()(n, precision), precision));

        // Accumulate numerator and denominator
        numerator += rest * data.Sn.at(order + j);
        denominator += rest;

        // TODO проверить корректность пересчета бин. коэф.
        //// Update binomial coefficient for next iteration: C(n, j+1) = C(n, j) * (n-j)/(j+1)
        // binomial_coef *= utils::cast<T>()(n - j);
        // binomial_coef /= utils::cast<T>()(j + static_cast<K>(1));
    }

    // Calculate the final result and check for validity
    numerator /= denominator;

    if (!utils::helpers<T>::isfinite(numerator)) throw std::overflow_error("division by zero");

    return numerator;
}

}  // namespace algos
}  // namespace shanks

#endif
