#ifndef EIGHTH_PI_M_ONE_THIRD_ITERATOR_HPP
#define EIGHTH_PI_M_ONE_THIRD_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of x*(pi/8-1/3) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class eighth_pi_m_one_third_iterator final : public series_base_iterator<T, K>{
public:

	eighth_pi_m_one_third_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		return this->x * (static_cast<T>(std::numbers::pi*0.125) - static_cast<T>(1)/static_cast<T>(3));
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

		this->current_state = utils::minus_one_raised_to_power_n<T,K>(this->n) * this->x / static_cast<T>(fma(2,this->n,1) * fma(2,this->n,3) * fma(2,this->n,5));
		this->n+=1;
		return this->current_state;
	}

};

#endif