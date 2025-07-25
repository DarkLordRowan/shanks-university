#pragma once

#include "remainders.hpp"

template<typename T, std::unsigned_integral K>
T t_transform<T, K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {
    const T result = T(1) / series->operator()(n+order);
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}
