#ifndef SHANKS_RAPID_REFERENCE_SUMS_HPP
#define SHANKS_RAPID_REFERENCE_SUMS_HPP
#pragma once

namespace shanks::series::detail {
/** Precision-dependent alternating sums; remainder <= first omitted magnitude.
 * Both series are verified independently by integral representations derived
 * from Euler's gamma integral and geometric series (DLMF 5.9 and 4.6).
 * Avoid fixed eight-digit decimal reference values in get_sum().
 */
template<AcceptedLike T>
T rapid_reference(const T& scale,bool factorial) {
    const auto precision=utils::helpers<T>::get_precision(scale);
    const T zero=utils::cast<T,int>()(0,precision),one=utils::cast<T,int>()(1,precision);
    // Native floating-point helpers use precision=0 as a sentinel.
    const auto digits=std::max<std::size_t>(precision,std::numeric_limits<long double>::max_digits10);
    const T threshold=utils::math<T>::pow(utils::cast<T,int>()(10,precision),
        utils::cast<T,int>()(-static_cast<int>(digits)-4,precision));
    T sum=zero;
    for(std::size_t k=1;k<8*(digits+25);++k) {
        const T divisor=utils::cast<T,std::size_t>()(k,precision);
        T term=one;
        for(std::size_t j=1;j<=k;++j)
            term*= (factorial ? utils::cast<T,std::size_t>()(j,precision) : one)/divisor;
        const auto bound=utils::math<T>::abs(term);
        if(bound<utils::math<T>::abs(threshold)) {
            if constexpr(IntervalLike<T>) sum+=T(-bound.rightinterval(),bound.rightinterval());
            else if constexpr(is_complex_interval<T>::value) {
                using R=typename real_of<T>::value;
                sum+=T(R(-bound.rightinterval(),bound.rightinterval()),R(0));
            }
            return scale*sum;
        }
        sum+=(k%2 ? -term : term);
    }
    throw std::runtime_error("Rapid reference sum failed its tail bound");
}
}
#endif
