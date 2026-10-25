#ifndef INTEGRAL1_ITERATOR_HPP
#define INTEGRAL1_ITERATOR_HPP
#pragma once
#include "../series_base.hpp"

namespace shanks::series {
/** Sequence of composite trapezoid approximations to integral_0^1 log(t)/(1+t)dt.
 * With t=u^2, the integrand is 4u log(u)/(1+u^2), continuously extended by 0
 * at u=0. No endpoint interval is discarded. Sequence index n uses
 * steps*(n+1) panels; the limit is -pi^2/12 (DLMF 25.2).
 */
template<AcceptedLike T,UnsignedIntLike K>
class integral1_iterator final : public series_base_seq_succ<T,K> {
public:
    const K steps;
    const std::size_t max_panels;
    explicit integral1_iterator(T x,K steps,std::size_t max_panels=16384)
        :series_base_seq_succ<T,K>(x),steps(steps),max_panels(max_panels) {
        if(is_invalid()) throw std::invalid_argument("Quadrature requires steps>0 and a finite legacy x");
    }
    explicit integral1_iterator(T x,T steps)=delete;
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x) || steps==0 || steps>max_panels;
    }
    T get_sum() const override {
        const auto p=utils::helpers<T>::get_precision(this->x);
        const T pi=utils::cast<T,int>()(4,p)*utils::math<T>::atan(utils::cast<T,int>()(1,p));
        return -pi*pi/utils::cast<T,int>()(12,p);
    }
    T next(K n,T& state) const override {
        const auto p=utils::helpers<T>::get_precision(this->x);
        const auto index=static_cast<std::size_t>(n);
        if(index>=max_panels/static_cast<std::size_t>(steps))
            throw std::out_of_range("Quadrature exceeds max_panels");
        const std::size_t panels=static_cast<std::size_t>(steps)*(index+1);
        const T one=utils::cast<T,int>()(1,p),four=utils::cast<T,int>()(4,p);
        const T divisor=utils::cast<T,std::size_t>()(panels,p);
        T sum=utils::cast<T,int>()(0,p);
        for(std::size_t j=1;j<panels;++j) {
            const T u=utils::cast<T,std::size_t>()(j,p)/divisor;
            sum+=four*u*utils::math<T>::log(u)/(one+u*u);
        }
        state=sum/divisor;
        return state;
    }
};
}
#endif
