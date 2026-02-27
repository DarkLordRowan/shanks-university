#ifndef PJ_ALGORITHM_HPP
#define PJ_ALGORITHM_HPP
#pragma once

namespace shanks {
namespace algos {

template <AcceptedLike T, UnsignedIntLike K>
class pj_algorithm final : public series_acceleration<T, K> {
protected:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /// Unique pointer to the remainder estimator strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;
    /// The specific type of remainder variant currently active.
    shanks::remainders::remainder_type remainder_type_in_use{shanks::remainders::remainder_type::u_type};

    const int p;
    const float_type beta;

public:
    explicit pj_algorithm(
        const shanks::remainders::remainder_type remainder_type_to_use = shanks::remainders::remainder_type::u_type,
        const int p = 2, const float_type beta = float_type(1))
        : series_acceleration<T, K>(), p(p), beta(beta) {
        update_type(remainder_type_to_use);
    };

    T operator()(const K n, const K order, const series_result<T>& data) const override;

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

    std::string get_name() override {
        series_acceleration<T, K>::acceleration_name = "pj_algorithm ";
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
T pj_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
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
        Denom[i] += remainder->operator()(n + i, n + i, data.an);
        Num[i] += data.Sn.at(n + i) * Denom[i];
    }

    std::function<float_type(K, K)> psi;
    if (p == static_cast<K>(2)) {
        psi = [&precision, this](K n, K k) {
            if (k == static_cast<K>(0)) return utils::cast<float_type, int>()(1, precision);
            const float_type num =
                utils::cast<float_type, K>()(n + k, precision) + beta - utils::cast<float_type, int>()(-1, precision);
            const float_type denom = num + utils::cast<float_type, int>()(2, precision);
            return utils::math<float_type>::pow(num / denom, utils::cast<float_type, K>()(k, precision));
        };
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

    for (K i = static_cast<K>(1); i <= order; ++i)
        for (K j = static_cast<K>(0); j <= order - i; ++j) {
            const T right = utils::cast<T, float_type>()(psi(n + j, i));
            Denom[j] = utils::math<T>::fma(-right, Denom[j], Denom[j + 1]);
            Num[j] = utils::math<T>::fma(-right, Num[j], Num[j + 1]);
        }

    Num[0] /= Denom[0];
    return Num[0];
}

}  // namespace algos
}  // namespace shanks

#endif
