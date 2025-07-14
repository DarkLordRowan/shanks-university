#pragma once

#include "remainders.hpp"

template<std::floating_point T, std::unsigned_integral K>
T v_transform<T,K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {
    const T a1 = series->operator()(n+order), a2  = series->operator()(n+order+1);
    const T result = (a2-a1) / (a1 * a2);
	if (!std::isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}