#pragma once

#include "remainders.hpp"

template<std::floating_point T, std::unsigned_integral K>
T d_transform<T,K>::operator()(const K n, const K order, const series_base<T, K>* series, T scale ) const {
	const T result = T(1) / series->operator()(n + order + 1);

	if (!std::isfinite(result)) throw std::overflow_error("division by zero");
	
	return result;
}