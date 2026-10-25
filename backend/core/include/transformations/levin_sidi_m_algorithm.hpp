#ifndef LEVIN_SIDI_M_ALGORITHM_HPP
#define LEVIN_SIDI_M_ALGORITHM_HPP
#pragma once

#define DEFAULT_GAMMA 100.5
/** M transform: Weniger, Computer Physics Reports 10 (1989), Sections 9.2, 9.4.
 * https://arxiv.org/pdf/math/0306302, Eqs. (9.2-5), (9.2-6), (9.4-1).
 * Historical C++ class name retained. n is a zero-based starting index;
 * order is k. gamma must be finite, positive and >= k-1; integers are allowed.
 * Order zero returns s_n without reading a_n. u uses omega_i=(-gamma-i)*a_i.
 * Other remainder variants retain the shared catalogue definitions.
 * Singular or non-finite results throw rather than claiming convergence.
 * Original authors: Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace algos {

template <AcceptedLike T, UnsignedIntLike K>
class levin_sidi_m_algorithm final : public series_acceleration<T, K> {
public:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /// Positive real parameter gamma. For theoretical stability, it often satisfies gamma >= order - 1.
    float_type gamma_in_use = utils::cast<float_type, double>()(DEFAULT_GAMMA);
    /// Pointer to the remainder transformation strategy being used.
    std::unique_ptr<const shanks::remainders::transform_base<T, K>> remainder;

    /// The specific Levin variant variant (u, t, v, etc.) used for remainder estimation.
    shanks::remainders::remainder_type remainder_type_in_use;

    explicit levin_sidi_m_algorithm(
        shanks::remainders::remainder_type remainder_type_to_use = shanks::remainders::remainder_type::u_type,
        const float_type& gamma_to_use = utils::cast<float_type, double>()(DEFAULT_GAMMA))
        : series_acceleration<T, K>() {
        update_gamma(gamma_to_use);
        update_type(remainder_type_to_use);
    }

    ~levin_sidi_m_algorithm() = default;  // Default destructor is sufficient since unique_ptr handles deletion

    T operator()(const K n, const K order, const series_result<T>& data) const override;

    void update_type(const shanks::remainders::remainder_type remainder_type_to_use);

    void validate_gamma(const float_type& _gamma) const {
        if(!utils::helpers<float_type>::isfinite(_gamma) || _gamma<=utils::cast<float_type,int>()(0))
            throw std::domain_error("gamma must be finite and positive");
    }

    void update_gamma(const float_type& new_gamma) {
        validate_gamma(new_gamma);
        gamma_in_use = new_gamma;
    }

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

    inline std::size_t how_much(const std::size_t n, const std::size_t order) const {
        if(order==0)return n+1;
        return n + order + std::size_t(1) +
               std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::t_wave_type ||
                           remainder_type_in_use == shanks::remainders::remainder_type::v_type) +
               std::size_t(2) * std::size_t(remainder_type_in_use == shanks::remainders::remainder_type::v_wave_type);
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
    const auto required=how_much(n,order);
    if(data.Sn.size()<required || (order>0 && data.an.size()<required))
        throw std::out_of_range("Insufficient M-transform input");
    if(order==0)return data.Sn.at(n);
    validate_gamma(gamma_in_use);
    const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
    if(gamma_in_use<utils::cast<float_type,K>()(order-1,p))
        throw std::domain_error("gamma must be >= order-1");
    const T zero=utils::cast<T,int>()(0,p),one=utils::cast<T,int>()(1,p);
    const T gamma=utils::cast<T,float_type>()(gamma_in_use,p);
    const T anchor=gamma+utils::cast<T,K>()(n,p)+utils::cast<T,K>()(order,p);
    T numerator=zero,denominator=zero,choose=one;
    // Weniger (9.2-6): normalized falling factorials; the common sign cancels.
    // n is the start, order is k. Build binomials in T, never bounded K.
    for(K j=0;j<=order;++j) {
        const K index=n+j;
        const T start=gamma+utils::cast<T,K>()(index,p);
        T weight=one;
        for(K m=0;m+1<order;++m) {
            const T offset=utils::cast<T,K>()(m,p);
            weight*=(start-offset)/(anchor-offset);
        }
        // u remainder: omega_i=(-gamma-i)*a_i, Weniger (9.4-1).
        const T scale=remainder_type_in_use==shanks::remainders::remainder_type::u_type
            ? -gamma-utils::cast<T,K>()(index,p)*utils::cast<T,int>()(2,p) : one;
        weight*=remainder->operator()(index,index,data.an,scale);
        weight*=j%2 ? -choose : choose;
        numerator+=weight*data.Sn.at(index);
        denominator+=weight;
        if(j<order)choose*=utils::cast<T,K>()(order-j,p)/utils::cast<T,K>()(j+1,p);
    }
    if(denominator==zero)throw std::overflow_error("Singular M-transform denominator");
    const T result=numerator/denominator;
    if(!utils::helpers<T>::isfinite(result))throw std::overflow_error("Non-finite M-transform");
    return result;
}

}  // namespace algos
}  // namespace shanks

#endif
