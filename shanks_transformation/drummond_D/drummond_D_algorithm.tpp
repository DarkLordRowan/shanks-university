#pragma once

#include "drummond_D_algorithm.hpp"
#include <vector>


template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result(const K n, const K order) const {

	T numerator = T(0), denominator = T(0);
	T rest;

	for (K j = 0; j <= n; ++j) {
		rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(static_cast<T>(n), j)*remainder_func->operator()(n,j, this->series);
		numerator   += rest * this->series->S_n(n+j);
		denominator += rest;
	}

	T result = numerator / denominator;

	if (!std::isfinite(result))
		throw std::overflow_error("division by zero");
	return result;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result_rec(const K n, const K order) const {

	std::vector<T>   Num(order + 1, 0);
	std::vector<T> Denom(order+1, 0);

    //init the base values
	for (K i = 0; i < order+1; ++i) {
		Denom[i] = remainder_func->operator()(n, i, this->series);
		  Num[i] = this->series->S_n(n+i) * Denom[i];
	}

    //recurrence
	for (K i = 1; i <= order; ++i)
		for (K j = 0; j <= order - i; ++j) {
			Denom[j] = Denom[j+1] - Denom[j];
			Num[j] = Num[j+1] - Num[j];
		}

	T result = Num[0] / Denom[0];

	if (!std::isfinite(result))
		throw std::overflow_error("division by zero");
	return result;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
drummonds_D_algorithm<T,K,series_templ>::drummonds_D_algorithm(const series_templ& series, char variant, bool useRecFormulas) : 
series_acceleration<T, K, series_templ>(series),
useRecFormulas(useRecFormulas)
{
    //check variant else default 'u'
    switch(variant){
        case 't':
            remainder_func = new t_transform<T, K>();
            break;
        case 'v':
            remainder_func = new v_transform<T, K>();
            break;
        case 'd':
            remainder_func = new d_transform<T, K>();
            break;
        case 'w':
            remainder_func = new v_transform_2<T, K>();
            break;
        default:
            remainder_func = new u_transform<T, K>();
            break;
    }
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T drummonds_D_algorithm<T,K,series_templ>::operator()(const K n, const K order) const { 
    T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!std::isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}
