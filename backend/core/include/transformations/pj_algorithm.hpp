#ifndef PJ_ALGORITHM_HPP
#define PJ_ALGORITHM_HPP
#pragma once
// Homeier (2000), https://arxiv.org/pdf/math/0005209, (98), (105), (107).
// Original author: Naumov A.U. Checks precede any vector access.

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
        if(order==0)return n+1;
        return n + order + std::size_t(1) +
               std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::t_wave_type ||
                           remainder_type_in_use == shanks::remainders::remainder_type::v_type) +
               std::size_t(2) * std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::v_wave_type);
    }
};

template <AcceptedLike T, UnsignedIntLike K>
T pj_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    const auto required=how_much(n,order);
    if(data.Sn.size()<required || (order>0 && data.an.size()<required))
        throw std::out_of_range("Insufficient Homeier transform data");
    const auto precision=utils::helpers<T>::get_precision(data.Sn.at(n));
    const T zero=utils::cast<T,int>()(0,precision),one=utils::cast<T,int>()(1,precision);
    if(!utils::helpers<float_type>::isfinite(beta) || beta<=utils::cast<float_type,int>()(0,precision))
        throw std::domain_error("beta must be finite and positive");
    if(p<1)throw std::domain_error("p must be a positive integer");
    if(order==0)return data.Sn.at(n);
    const std::size_t span=static_cast<std::size_t>(order);
    const T beta_t=utils::cast<T,float_type>()(beta,precision);
    std::vector<T> num(span+1),den(span+1);
    for(std::size_t j=0;j<=span;++j) {
        const K index=n+static_cast<K>(j);
        den[j]=remainder->operator()(index,index,data.an,beta_t);
        num[j]=data.Sn.at(index)*den[j];
    }
    for(std::size_t level=1;level<=span;++level)for(std::size_t j=0;j<=span-level;++j) {
        const std::size_t k=level-1; // Eq. (98) uses Psi^(k-1), not Psi^k.
        T psi=one;
        const T start=utils::cast<T,std::size_t>()(n+j+k,precision)+beta_t-one;
        for(std::size_t m=0;m<k;++m) {
            // Product form of (107), also valid at p=1 and p=2.
            const T top=start+utils::cast<T,int>()(p-2,precision)*utils::cast<T,std::size_t>()(m,precision);
            psi*=top/(top+utils::cast<T,int>()(2,precision));
        }
        num[j]=num[j+1]-psi*num[j];
        den[j]=den[j+1]-psi*den[j];
    }
    if(den[0]==zero)throw std::overflow_error("Singular Homeier transform denominator");
    const T result=num[0]/den[0];
    if(!utils::helpers<T>::isfinite(result))throw std::overflow_error("Non-finite Homeier transform");
    return result;
}

}  // namespace algos
}  // namespace shanks

#endif
