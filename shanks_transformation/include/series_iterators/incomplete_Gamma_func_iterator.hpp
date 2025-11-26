#ifndef INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#define INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
//#include <gsl/gsl_sf_gamma.h>

/**
* @brief Maclaurin series of -0.5 log(2-2cos(x))function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class incomplete_Gamma_func_iterator final : public series_base_iterator<T, K>{
public:

	incomplete_Gamma_func_iterator() : series_base_iterator<T, K>() {}

	T alpha;

	T sum() const override{

		//if constexpr (std::is_floating_point<T>::value){
		//	return std::tgamma(this->alpha) - static_cast<T>(gsl_sf_gamma_inc(static_cast<double>(alpha), static_cast<double>(this->x)));
		//} else {
			return static_cast<T>(0);
		//}

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag());
		} else {
			return !isfinite(this->x);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = pow(this->x, alpha) / alpha;
		else this->current_state *= static_cast<T>(-1) * this->x * (alpha + static_cast<T>(this->n-1)) /
		((alpha + static_cast<T>(this->n)) * static_cast<T>(this->n));

		this->n+=1;
		return this->current_state;
	}

};
#endif