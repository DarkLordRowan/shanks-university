#ifndef UTILS_MATH_OP_COUNTER_HPP
#define UTILS_MATH_OP_COUNTER_HPP
#pragma once

#ifdef SHANKS_ENABLE_PROFILING

#define OpsWrapperMacro(T) shanks::profiling::OperationCounting<T>

template <typename T>
struct utils::math<OpsWrapperMacro(T)> {
    template <std::integral K>
    static OpsWrapperMacro(T) minus_one_raised_to_power_n(const K j);
    static OpsWrapperMacro(T) pow(const OpsWrapperMacro(T) & x, const OpsWrapperMacro(T) & y);
    static OpsWrapperMacro(T) atan2(const OpsWrapperMacro(T) & x, const OpsWrapperMacro(T) & y);
    static OpsWrapperMacro(T)
        fma(const OpsWrapperMacro(T) & a, const OpsWrapperMacro(T) & b, const OpsWrapperMacro(T) & c);
    static OpsWrapperMacro(T) sqrt(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) exp(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) log(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) log10(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) hypot(const OpsWrapperMacro(T) & a, const OpsWrapperMacro(T) & b);
    static OpsWrapperMacro(T) erf(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) zeta(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) ci_x(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) si_x(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) e_x(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) k_x(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) inc_gamma(const OpsWrapperMacro(T) & x, const OpsWrapperMacro(T) & alpha);
    static OpsWrapperMacro(T) lambertW0(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) airy_ai(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) clausen(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) sin(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) asin(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) cos(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) acos(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) tan(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) atan(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) sinh(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) asinh(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) cosh(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) acosh(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) tanh(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) atanh(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) abs(const OpsWrapperMacro(T) & x);

    // availability of special function
    using has_erf = utils::math<T>::has_erf;
    using has_zeta = utils::math<T>::has_zeta;
    using has_ci_x = utils::math<T>::has_ci_x;
    using has_si_x = utils::math<T>::has_si_x;
    using has_e_x = utils::math<T>::has_e_x;
    using has_k_x = utils::math<T>::has_k_x;
    using has_inc_gamma = utils::math<T>::has_inc_gamma;
    using has_lambertW0 = utils::math<T>::has_lambertW0;
    using has_airy_ai = utils::math<T>::has_airy_ai;
    using has_clausen = utils::math<T>::has_clausen;
    using has_bessel_first_kind = utils::math<T>::has_bessel_first_kind;
};

template <typename T>
template <std::integral K>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::minus_one_raised_to_power_n(const K j) {
    if constexpr (std::is_signed<K>::value)
        return OpsWrapperMacro(T)(std::abs(j) & 1 ? -1 : 1);
    else
        return OpsWrapperMacro(T)(j & 1 ? -1 : 1);
}

template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::pow(const OpsWrapperMacro(T) & x, const OpsWrapperMacro(T) & y) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::pow(x.value, y.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::atan2(const OpsWrapperMacro(T) & x, const OpsWrapperMacro(T) & y) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::atan2(x.value, y.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::fma(const OpsWrapperMacro(T) & a, const OpsWrapperMacro(T) & b,
                                                        const OpsWrapperMacro(T) & c) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::fma(a.value, b.value, c.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::sqrt(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::sqrt(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::exp(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::exp(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::log(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::log(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::log10(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::log10(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::hypot(const OpsWrapperMacro(T) & a, const OpsWrapperMacro(T) & b) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::hypot(a.value, b.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::erf(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::erf(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::zeta(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::zeta(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::ci_x(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::ci_x(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::si_x(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::si_x(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::e_x(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::e_x(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::k_x(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::k_x(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::inc_gamma(const OpsWrapperMacro(T) & x,
                                                              const OpsWrapperMacro(T) & alpha) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::inc_gamma(x.value, alpha.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::lambertW0(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::lambertW0(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::airy_ai(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::airy_ai(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::clausen(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::clausen(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::sin(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::sin(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::asin(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::asin(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::cos(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::cos(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::acos(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::acos(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::tan(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::tan(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::atan(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::atan(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::sinh(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::sinh(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::asinh(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::asinh(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::cosh(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::cosh(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::acosh(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::acosh(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::tanh(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::tanh(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::atanh(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::atanh(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::math<OpsWrapperMacro(T)>::abs(const OpsWrapperMacro(T) & x) {
    shanks::profiling::global_operation_counts.special++;
    return OpsWrapperMacro(T)(utils::math<T>::abs(x.value));
}

#undef OpsWrapperMacro

#endif

#endif