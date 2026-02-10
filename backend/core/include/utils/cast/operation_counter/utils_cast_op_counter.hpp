#ifndef UTILS_CAST_OP_COUNTER_HPP
#define UTILS_CAST_OP_COUNTER_HPP
#pragma once

#ifdef SHANKS_ENABLE_PROFILING

#define OpsWrapperMacro(T) shanks::profiling::OperationCounting<T>

template <FloatLike To, FloatLike From>
struct utils::cast<OpsWrapperMacro(To), From> {
    OpsWrapperMacro(To) operator()(const From & x, std::size_t precision = std::size_t{0});
};

template <FloatLike To, FloatLike From>
OpsWrapperMacro(To) utils::cast<OpsWrapperMacro(To), From>::operator()(const From & x, std::size_t precision) {
    return OpsWrapperMacro(To)(utils::cast<To, From>()(x, precision));
}

template <FloatLike To, FloatLike From>
struct utils::cast<To, OpsWrapperMacro(From)> {
    To operator()(const OpsWrapperMacro(From) & x, std::size_t precision = std::size_t{0});
};

template <FloatLike To, FloatLike From>
To utils::cast<To, OpsWrapperMacro(From)>::operator()(const OpsWrapperMacro(From) & x, std::size_t precision) {
    return utils::cast<To, From>()(x.value, precision);
}

template <FloatLike To, FloatLike From>
struct utils::cast<OpsWrapperMacro(To), OpsWrapperMacro(From)> {
    OpsWrapperMacro(To) operator()(const OpsWrapperMacro(From) & x, std::size_t precision = std::size_t{0});
};

template <FloatLike To, FloatLike From>
OpsWrapperMacro(To) utils::cast<OpsWrapperMacro(To), OpsWrapperMacro(From)>::operator()(const OpsWrapperMacro(From) & x,
                                                                                        std::size_t precision) {
    return OpsWrapperMacro(To)(utils::cast<To, From>()(x.value, precision));
}

#undef OpsWrapperMacro

#endif

#endif