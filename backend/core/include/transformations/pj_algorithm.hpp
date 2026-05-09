#ifndef PJ_ALGORITHM_HPP
#define PJ_ALGORITHM_HPP
#pragma once

/**
 * @file pj_algorithm.hpp
 * @brief Contains implementation of pJ-transformation for sequence acceleration.
 *
 * For theory, see:
 * Herbert, H. H. Homeier(2018) SCALAR LEVIN-TYPE SEQUENCE TRANSFORMATIONS
 * @authors Naumov A.U.
 */

namespace shanks {
namespace algos {

template <AcceptedLike T, UnsignedIntLike K>
class pj_algorithm final : public series_acceleration<T, K> {
public:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /// Unique pointer to the remainder estimator strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;
    /// The specific type of remainder variant currently active.
    shanks::remainders::remainder_type remainder_type_in_use{shanks::remainders::remainder_type::u_type};
    /// int parameter
    const int p;
    /// real parameter
    const float_type beta;

    /**
     * @brief Parameterized constructor to initialize pJ-transformation
     * @param variant Type of remainder estimator to use
     *        Determines the specific variant of pJ's transformation:
     *        - u_type: Standard remainder estimator
     *        - t_type: Alternative remainder estimator
     *        - v_type: Alternative remainder estimator
     *        - t_wave_type: Modified remainder estimator
     *        - v_wave_type: Modified remainder estimator
     * @param auxilary_series std::function<T(K)> lambda function to get auxilary series a_{n}
     * @authors Naumov A.U.
     */
    explicit pj_algorithm(
        const shanks::remainders::remainder_type remainder_type_to_use = shanks::remainders::remainder_type::u_type,
        const int p = 2, const float_type beta = float_type(1))
        : series_acceleration<T, K>(), p(p), beta(beta) {
        update_type(remainder_type_to_use);
    };

    /**
     * @brief Executes pJ-transformation to accelerate series convergence.
     *
     * This method acts as the entry point for the transformation, delegating the work
     * to either the direct or recursive implementation based on the configuration.
     *
     * For theory, see: Herbert, H. H. Homeier(2018) SCALAR LEVIN-TYPE SEQUENCE TRANSFORMATIONS, p. 18
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
        series_acceleration<T, K>::acceleration_name = std::to_string(p) + std::string("j_algorithm ");
        switch (remainder_type_in_use) {
            case shanks::remainders::remainder_type::u_type: {
                series_acceleration<T, K>::acceleration_name += "with u-variant";
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
        series_acceleration<T, K>::acceleration_name += "and beta = " + utils::helpers<T>::to_string(beta);

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
T pj_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Calculate minimum required size based on the chosen remainder variant
    const std::size_t required_size = pj_algorithm<T, K>::how_much(n, order);

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
    for (K i = static_cast<K>(0); i < order + static_cast<K>(1); ++i) {
        Denom[i] += remainder->operator()(n + i, n + i, data.an);
        Num[i] += data.Sn.at(n + i) * Denom[i];
    }

    // see: Herbert, H. H. Homeier(2018) SCALAR LEVIN-TYPE SEQUENCE TRANSFORMATIONS [107, p. 18]
    std::function<float_type(K, K)> psi;
    if (p == static_cast<K>(2)) {
        psi = [&precision, this](K n, K k) {
            if (k == static_cast<K>(0)) return utils::cast<float_type, int>()(1, precision);
            const float_type num =
                utils::cast<float_type, K>()(n + k, precision) + beta - utils::cast<float_type, int>()(-1, precision);
            const float_type denom = num + utils::cast<float_type, int>()(2, precision);
            return utils::math<float_type>::pow(num / denom, utils::cast<float_type, K>()(k, precision));
        };  //
    } else {
        psi = [&precision, this](K n, K k) {
            if (k == static_cast<K>(0)) return utils::cast<float_type, int>()(1, precision);
            const float_type num = (utils::cast<float_type, K>()(n + k, precision) + beta -
                                    utils::cast<float_type, int>()(-1, precision)) /
                                   utils::cast<float_type, int>()(p - 2, precision);
            const float_type denom =
                (utils::cast<float_type, K>()(n + k, precision) + beta - utils::cast<float_type, int>()(1, precision)) /
                utils::cast<float_type, int>()(p - 2, precision);
            float_type res = num / denom;
            for (K i{1}; i <= k; ++i)
                res *= (num + utils::cast<float_type, K>()(i, precision)) /
                       (denom + utils::cast<float_type, K>()(i, precision));
            return res;
        };
    }

    // Implement recursive scheme, see: Herbert, H. H. Homeier(2018) SCALAR LEVIN-TYPE SEQUENCE TRANSFORMATIONS [98, p.
    // 17]
    for (K i = static_cast<K>(1); i <= order; ++i)
        for (K j = static_cast<K>(0); j <= order - i; ++j) {
            const T right = utils::cast<T, float_type>()(psi(n + j, i));
            Denom[j] = utils::math<T>::fma(-right, Denom[j], Denom[j + 1]);
            Num[j] = utils::math<T>::fma(-right, Num[j], Num[j + 1]);
        }

    Num[0] /= Denom[0];
    if (!utils::helpers<T>::isfinite(Num[0])) throw std::overflow_error("division by zero");
    return Num[0];
}

}  // namespace algos
}  // namespace shanks

#endif
