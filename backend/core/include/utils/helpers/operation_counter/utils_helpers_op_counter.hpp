#ifndef UTILS_HELPERS_OP_COUNTER_HPP
#define UTILS_HELPERS_OP_COUNTER_HPP
#pragma once

#ifdef SHANKS_ENABLE_PROFILING

#define OpsWrapperMacro(T) shanks::profiling::OperationCounting<T>

template <typename T>
struct utils::helpers<OpsWrapperMacro(T)> {
    static OpsWrapperMacro(T) nextafter(const OpsWrapperMacro(T) & x, const OpsWrapperMacro(T) & y);
    static OpsWrapperMacro(T) fmod(const OpsWrapperMacro(T) & x, const OpsWrapperMacro(T) & y);
    static OpsWrapperMacro(T) floor(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) ceil(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) get_nan();
    static OpsWrapperMacro(T) frexp(const OpsWrapperMacro(T) & x, int* exp);
    static std::size_t get_precision(const OpsWrapperMacro(T) & x);
    static std::string to_string(const OpsWrapperMacro(T) & x);
    static bool isfinite(const OpsWrapperMacro(T) & x);
    static bool isnan(const OpsWrapperMacro(T) & x);
    static bool isinf(const OpsWrapperMacro(T) & x);
    static OpsWrapperMacro(T) epsilon(std::size_t precision = std::size_t{0});
    static OpsWrapperMacro(T) numeric_max(std::size_t precision = std::size_t{0});
};

template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::nextafter(const OpsWrapperMacro(T) & x,
                                                                 const OpsWrapperMacro(T) & y) {
    return OpsWrapperMacro(T)(utils::helpers<T>::nextafter(x.value, y.value));
}

template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::fmod(const OpsWrapperMacro(T) & x,
                                                            const OpsWrapperMacro(T) & y) {
    return OpsWrapperMacro(T)(utils::helpers<T>::fmod(x.value, y.value));
}
template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::floor(const OpsWrapperMacro(T) & x) {
    return OpsWrapperMacro(T)(utils::helpers<T>::floor(x.value));
}
template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::ceil(const OpsWrapperMacro(T) & x) {
    return OpsWrapperMacro(T)(utils::helpers<T>::ceil(x.value));
}

template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::get_nan() {
    return OpsWrapperMacro(T)(utils::helpers<T>::get_nan());
}

template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::frexp(const OpsWrapperMacro(T) & x, int* exp) {
    return OpsWrapperMacro(T)(utils::helpers<T>::frexp(x.value, exp));
}

template <typename T>
std::size_t utils::helpers<OpsWrapperMacro(T)>::get_precision(const OpsWrapperMacro(T) & x) {
    return utils::helpers<T>::get_precision(x.value);
}

template <typename T>
std::string utils::helpers<OpsWrapperMacro(T)>::to_string(const OpsWrapperMacro(T) & x) {
    return utils::helpers<T>::to_string(x.value);
}

template <typename T>
bool utils::helpers<OpsWrapperMacro(T)>::isfinite(const OpsWrapperMacro(T) & x) {
    return utils::helpers<T>::isfinite(x.value);
}
template <typename T>
bool utils::helpers<OpsWrapperMacro(T)>::isnan(const OpsWrapperMacro(T) & x) {
    return utils::helpers<T>::isnan(x.value);
}
template <typename T>
bool utils::helpers<OpsWrapperMacro(T)>::isinf(const OpsWrapperMacro(T) & x) {
    return utils::helpers<T>::isinf(x.value);
}

template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::epsilon(std::size_t precision) {
    return OpsWrapperMacro(T)(utils::helpers<T>::epsilon(precision));
}
template <typename T>
OpsWrapperMacro(T) utils::helpers<OpsWrapperMacro(T)>::numeric_max(std::size_t precision) {
    return OpsWrapperMacro(T)(utils::helpers<T>::numeric_max(precision));
}

#undef OpsWrapperMacro
#endif

#endif