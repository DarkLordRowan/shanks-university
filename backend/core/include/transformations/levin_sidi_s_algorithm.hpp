#ifndef LEVIN_SIDI_S_ALGORITHM_HPP
#define LEVIN_SIDI_S_ALGORITHM_HPP
#pragma once
#include "levin_weights.hpp"
/**
 * @file levin_sidi_s_algorithm.hpp
 * @brief Contains implementation of the Levin-Sidi S-transformation (related to Drummond's D-transformation).
 * @authors Naumov A.U.
 *
 * For theory, see:
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 *   Cambridge University Press. (Chapter 8, pp. 57-58)
 * Sidi, A. (2003). A new class of nonlinear transformations for accelerating the convergence
 *   of infinite integrals and series. arXiv:math/0306302.
 */

namespace shanks {
namespace algos {

/**
 * @brief Levin-Sidi S-transformation class template (analogous to Drummond's D-transformation).
 *
 * This class implements the Levin-Sidi S-transformation, which is particularly effective
 * for series with specific asymptotic behaviors. The transformation utilizes Pochhammer
 * symbols for weighting and can be executed via direct summation or a recursive scheme
 * for enhanced stability.
 *
 * References:
 * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 * - Sidi, A. (2003). A new class of nonlinear transformations. arXiv:math/0306302.
 *
 * @authors Naumov A.U.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike).
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class levin_sidi_s_algorithm final : public series_acceleration<T, K> {
public:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /// Positive real parameter (β > 0). Default to 1.0.
    float_type beta_in_use = utils::cast<float_type, int>()(1);

    /// Pointer to the remainder transformation strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;

    /// Flag to use recurrence formulas (true) or direct formulas (false)
    bool use_recurrent_formula{false};

    /// Type of Levin transformation variant (u, t, v, t~, v~)
    shanks::remainders::remainder_type remainder_type_in_use{shanks::remainders::remainder_type::u_type};

    /**
     * @brief Computes the S-transformation using direct summation formulas.
     *
     * For theory, see: Sidi (2003, arXiv:math/0306302), pp. 57-58, Eqs. (8.2)-(8.7)
     * ([https://arxiv.org/pdf/math/0306302.pdf]) General form: S_{k,n} =  [∑_{j=0}^k (-1)^j C(k,j)
     * (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] / [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
     *
     * @authors Naumov A.U.
     * @param n Starting index for the transformation
     * @param order Order of transformation (k value)
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return Accelerated sum estimate S_{k,n}
     */
    inline T calc_result(const K n, const K order, const series_result<T>& data) const;

    /**
     * @brief Computes the S-transformation using recurrence formulas.
     *
     * For theory, see: Sidi (2003, arXiv:math/0306302), pp. 57-58, Eqs. (8.3)-(8.5)
     * ([https://arxiv.org/pdf/math/0306302.pdf]) Recursive implementation for better numerical stability in some cases.
     *
     * @authors Naumov A.U.
     * @param n Starting index for the transformation
     * @param order Order of transformation (k value)
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return Accelerated sum estimate S_{k,n}
     */
    inline T calc_result_rec(const K n, const K order, const series_result<T>& data) const;

    /**
     * @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
     *
     * @authors Naumov A.U.
     * @param variant Type of remainder transformation to use
     *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
     *        Determines the remainder estimate R_n used in the transformation
     * @param use_recurrent_formula Flag to use recurrence formulas instead of direct summation
     *        true: use recursive implementation, false: use direct summation
     * @param parameter Positive real parameter β (must be > 0)
     *        Default value: 1.0. Affects the Pochhammer symbol terms in the transformation.
     *        For theory, see: Sidi (2003, arXiv:math/0306302), p. 39
     */
    explicit levin_sidi_s_algorithm(
        shanks::remainders::remainder_type remainder_type_in_use = shanks::remainders::remainder_type::u_type,
        bool use_recurrent_formula = false, const float_type& beta_to_use = utils::cast<float_type, int>()(1))
        : series_acceleration<T, K>(), use_recurrent_formula(use_recurrent_formula) {
        // parameter is "beta" parameter
        // beta must be nonzero positive real number
        // beta = 1 is default
        // check parameter else default
        update_beta(beta_to_use);
        update_type(remainder_type_in_use);
    }

    /**
     * @brief Executes the Levin-Sidi S-transformation to accelerate series convergence.
     *
     * Computes the accelerated sum using the S-transformation (Drummond's D-transformation),
     * which is particularly effective for series with specific asymptotic behaviors.
     *
     * For theory, see:
     * - General framework: Sidi (2003, arXiv:math/0306302), Eqs. (8.2)-(8.7)
     * - Convergence properties: Sidi (2003, Practical Extrapolation Methods), Chapter 8
     *
     * @authors Naumov A.U.
     * @param n The starting index for the transformation
     *        Valid values: n >= 0
     * @param order The order of transformation (k value)
     *        Valid values: order >= 0
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum after S-transformation
     * @throws std::overflow_error if division by zero or numerical instability occurs
     * @throws std::out_of_range if the input vectors are too small for the requested parameters.
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

    /**
     * @brief Updates the beta parameter used in the transformation.
     * @authors Naumov A.U.
     * @param new_beta The new beta value. Must be greater than zero, otherwise defaults to 1.0.
     */
    void update_beta(const float_type& new_beta) {
        beta_in_use = (new_beta > utils::cast<float_type, int>()(0) ? new_beta : utils::cast<float_type, int>()(1));
    }

    /**
     * @brief Changes the remainder estimator variant.
     * @authors Naumov A.U.
     * @param remainder_type_to_use The new remainder variant to employ.
     */
    void update_type(const shanks::remainders::remainder_type remainder_type_to_use) {
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
                remainder.reset(new shanks::remainders::u_transform<T, K>());  // Default to u-variant
            }
        }
    }

    /**
     * @brief Retrieves the descriptive name of the currently active Levin-Sidi S variant.
     * @authors Naumov A.U.
     * @return std::string The name and current configuration of the algorithm.
     */
    std::string get_name() override {
        series_acceleration<T, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
        series_acceleration<T, K>::acceleration_name += "levin sidi s algorithm ";
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

    inline std::size_t how_much(const std::size_t n, const std::size_t order) const {
        return n + order + std::size_t(1) +
               std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::t_wave_type ||
                           remainder_type_in_use == shanks::remainders::remainder_type::v_type) +
               std::size_t(2) * std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::v_wave_type);
    }
};

template <AcceptedLike T, UnsignedIntLike K>
inline T levin_sidi_s_algorithm<T, K>::calc_result(const K n, const K order, const series_result<T>& data) const {
    const auto precision=utils::helpers<T>::get_precision(data.Sn.at(n));
    auto inverse=[&](K index) {
        return remainder->operator()(index,index,data.an,
            utils::cast<T,float_type>()(remainder_type_in_use==shanks::remainders::remainder_type::u_type
                ? beta_in_use : utils::cast<float_type,int>()(1,precision),precision));
    };
    return detail::levin_weighted_ratio(n,order,data,beta_in_use,inverse,true,false);
}

template <AcceptedLike T, UnsignedIntLike K>
inline T levin_sidi_s_algorithm<T, K>::calc_result_rec(const K n, const K order, const series_result<T>& data) const {
    const auto precision=utils::helpers<T>::get_precision(data.Sn.at(n));
    auto inverse=[&](K index) {
        return remainder->operator()(index,index,data.an,
            utils::cast<T,float_type>()(remainder_type_in_use==shanks::remainders::remainder_type::u_type
                ? beta_in_use : utils::cast<float_type,int>()(1,precision),precision));
    };
    return detail::levin_weighted_ratio(n,order,data,beta_in_use,inverse,true,true);
}

template <AcceptedLike T, UnsignedIntLike K>
T levin_sidi_s_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Calculate total required terms in Sn and an vectors
    const std::size_t required_size = levin_sidi_s_algorithm<T, K>::how_much(n, order);

    if (data.Sn.size() < required_size || data.an.size() < required_size) {
        throw std::out_of_range("The Sn or an smaller then required for S_{" + utils::helpers<K>::to_string(order) +
                                "}^{" + utils::helpers<K>::to_string(n) + "}\n" +
                                "the size of Sn and an must be at least " +
                                utils::helpers<size_t>::to_string(required_size));
    }

    // Trivial case: order 0 returns the original partial sum
    if (order == static_cast<K>(0)) return data.Sn.at(n);

    // Delegate to either the direct summation or the recursive implementation
    const T result = (use_recurrent_formula ? calc_result_rec(n, order, data) : calc_result(n, order, data));

    if (!utils::helpers<T>::isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

}  // namespace algos
}  // namespace shanks

#endif
