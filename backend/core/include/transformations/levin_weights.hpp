#ifndef SHANKS_LEVIN_WEIGHTS_HPP
#define SHANKS_LEVIN_WEIGHTS_HPP
#pragma once

namespace shanks::algos::detail {
/** Canonical Levin/S factorial ratios: direct sum or recursive finite differences.
 * Weniger (1989), (7.1-7), (8.2-7): https://arxiv.org/pdf/math/0306302.
 * Both paths use the SAME fixed-order weights. This avoids shifting beta and
 * the erroneous order-dependent scaling in the previous recurrences.
 */
template<AcceptedLike T, UnsignedIntLike K, typename Beta, typename Inverse>
T levin_weighted_ratio(K n,K order,const series_result<T>& data,const Beta& beta,
                      Inverse inverse,bool rising,bool recursive) {
    const auto precision=utils::helpers<T>::get_precision(data.Sn.at(n));
    const T zero=utils::cast<T,int>()(0,precision),one=utils::cast<T,int>()(1,precision);
    const T offset=utils::cast<T,Beta>()(beta,precision);
    if(!utils::helpers<T>::isfinite(offset)) throw std::invalid_argument("Non-finite beta");
    const T anchor=offset+utils::cast<T,K>()(n,precision)+utils::cast<T,K>()(order,precision);
    std::vector<T> num,den;
    T numerator=zero,denominator=zero,choose=one;
    for(K j=0;j<=order;++j) {
        const T start=offset+utils::cast<T,K>()(n,precision)+utils::cast<T,K>()(j,precision);
        T weight=one;
        for(K m=0;m+1<order;++m) {
            const T increment=rising ? utils::cast<T,K>()(m,precision) : zero;
            weight*=(start+increment)/(anchor+increment);
        }
        weight*=inverse(n+j);
        if(recursive) {
            den.push_back(weight);
            num.push_back(weight*data.Sn.at(n+j));
        } else {
            const T signed_weight=(j%2 ? -choose : choose)*weight;
            denominator+=signed_weight;
            numerator+=signed_weight*data.Sn.at(n+j);
        }
        if(j<order) choose*=utils::cast<T,K>()(order-j,precision)/utils::cast<T,K>()(j+1,precision);
    }
    if(recursive) {
        for(K level=1;level<=order;++level)
            for(K j=0;j<=order-level;++j) {
                den[j]=den[j+1]-den[j];
                num[j]=num[j+1]-num[j];
            }
        numerator=num[0]; denominator=den[0];
    }
    if(denominator==zero) throw std::overflow_error("Singular Levin-type denominator");
    const T result=numerator/denominator;
    if(!utils::helpers<T>::isfinite(result)) throw std::overflow_error("Non-finite Levin-type transform");
    return result;
}
}
#endif
