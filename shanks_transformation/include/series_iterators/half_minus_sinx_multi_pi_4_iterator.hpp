#ifndef HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#define HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of 0.5asin(2x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class half_minus_sinx_multi_pi_4_iterator final : public series_base_iterator<T, K>{
public:

	half_minus_sinx_multi_pi_4_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		using std::sin;

		return static_cast<T>(0.5) - static_cast<T>(std::numbers::pi * 0.25) * sin(this->x);

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) 
			|| this->x.real() < static_cast<T>(0).real() || this->x.real() > static_cast<T>(0.5 * std::numbers::pi).real();
		} else {
			return !isfinite(this->x) || this->x < static_cast<T>(0) || this->x > static_cast<T>(0.5 * std::numbers::pi);
		}

	}

	T next() override {
		using std::cos;
		
		this->current_state = cos(static_cast<T>(fma(2,this->n,2)) * this->x) / static_cast<T>(fma(2,this->n,1) * fma(2,this->n,3));
		
		this->n+=1;
		return this->current_state;
	}

};
#endif
