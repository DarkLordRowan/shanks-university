#ifndef SI_X_ITERATOR_HPP
#define SI_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <gsl/gsl_sf.h>

/**
* @brief Maclaurin series of si_x(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class si_x_iterator final : public series_base_iterator<T, K>{
public:

	si_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		if constexpr (std::is_floating_point<T>::value){
			return static_cast<T>(gsl_sf_Si(static_cast<double>(this->x)));
		} 
		#ifdef INC_FPRECISION
		else if constexpr (std::is_same<T, float_precision>::value){
			return float_precision(gsl_sf_Si(static_cast<double>(this->x)), this->x.precision());
		} 
		#endif
		else return static_cast<T>(0);
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag());
		} else {
			return !isfinite(this->x);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= static_cast<T>(-1) * this->x * this->x * static_cast<T>(fma(2,this->n-1,1)) / 
		static_cast<T>(fma(2,this->n,1) * 2 * fma(2,this->n,1) * this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif