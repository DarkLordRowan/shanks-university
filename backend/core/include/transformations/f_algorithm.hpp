#ifndef F_ALGORITHM_HPP
#define F_ALGORITHM_HPP
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

template <AcceptedLike T, UnsignedIntLike K>
class f_algorithm final : public series_acceleration<T, K> {
protected:
    /// Unique pointer to the remainder estimator strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;
    /// The specific type of remainder variant currently active.
    shanks::remainders::remainder_type remainder_type_in_use{shanks::remainders::remainder_type::u_type};

    std::function<T (K)> auxilary_series = [](K n){ return utils::cast<T,K>()(n+2);};

public:
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
    explicit f_algorithm( const shanks::remainders::remainder_type remainder_type_to_use = shanks::remainders::remainder_type::u_type )
        : series_acceleration<T, K>()
    {
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
        series_acceleration<T, K>::acceleration_name = "f_algorithm ";
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

        return series_acceleration<T, K>::acceleration_name;
    }
};

template <AcceptedLike T, UnsignedIntLike K>
T f_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Calculate minimum required size based on the chosen remainder variant
    const K required_size =
        n + order + static_cast<K>(1) +
        static_cast<K>(remainder_type_in_use == shanks::remainders::remainder_type::t_wave_type ||
                       remainder_type_in_use == shanks::remainders::remainder_type::v_type) +
        static_cast<K>(2) * static_cast<K>(remainder_type_in_use == shanks::remainders::remainder_type::v_wave_type);

    if (data.Sn.size() < required_size || data.an.size() < required_size) {
        throw std::out_of_range("The Sn or an smaller then required for D_{" + utils::helpers<K>::to_string(order) +
                                "}^{" + utils::helpers<K>::to_string(n) + "}\n" +
                                "the size of Sn and an must be at least " +
                                utils::helpers<size_t>::to_string(required_size));
    }

    const size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    std::vector<T> Num = std::vector<T>(order + static_cast<K>(1), utils::cast<T, int>()(0, precision));
    std::vector<T> Denom = std::vector<T>(order + static_cast<K>(1), utils::cast<T, int>()(0, precision));

    // Initialize base values
    // xn = n+4
    for (K i = static_cast<K>(0); i < order + static_cast<K>(1); ++i) {
        Denom[i] += remainder->operator()(n + i, n + i, data.an) / (auxilary_series(n+i) - utils::cast<T,int>()(1));
        Num[i]   += data.Sn.at(n + i) * Denom[i] / (auxilary_series(n+i) - utils::cast<T,int>()(1));
    }

    // Apply forward difference recurrence:
    // N_j^{(i)} = N_{j+1}^{(i-1)} - N_j^{(i-1)}
    // D_j^{(i)} = D_{j+1}^{(i-1)} - D_j^{(i-1)}
    for (K i = static_cast<K>(1); i <= order; ++i)
        for (K j = static_cast<K>(0); j <= order - i; ++j) {
            const T left  = utils::cast<T,K>()(i, precision) + auxilary_series(n+i+j) - utils::cast<T,int>()(2, precision);
            const T right = utils::cast<T,K>()(i, precision) + auxilary_series(n+i)   - utils::cast<T,int>()(2, precision);
            const T denom = auxilary_series(n+i+j) - auxilary_series(n+i);
            const T DenomTmp = (Denom[j + static_cast<K>(1)] * left - Denom[j] *  right) / denom;
            const T NumTmp   = (Num[j + static_cast<K>(1)] * left -   Num[j] * right) / denom;
            Denom[j] = (utils::helpers<T>::isfinite(DenomTmp) && utils::helpers<T>::isfinite(NumTmp) ? DenomTmp : Denom[j]);
            Num[j]   = (utils::helpers<T>::isfinite(DenomTmp) && utils::helpers<T>::isfinite(NumTmp) ?   NumTmp :   Num[j]);
        }

    // Final result: D_n^{(order)} = N_0^{(order)} / D_0^{(order)}
    Num[0] /= Denom[0];
    return Num[0];
}

}  // namespace algos
}  // namespace shanks

#endif
