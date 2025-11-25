#ifndef TEN_MINUS_X_ITERATOR_HPP
#define TEN_MINUS_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Fourier series of 10-x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ten_minus_x_iterator final : public series_base_iterator<T, K>{
public:

	ten_minus_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		return static_cast<T>(10) - this->x;
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x - static_cast<T>(10)) > static_cast<T>(5).real();
        } else {
		    return !isfinite(this->x) || abs(this->x - static_cast<T>(10)) > static_cast<T>(5);
        }

	}

	T next() override {

		using std::sin;
		
		this->current_state = static_cast<T>(10) * utils::minus_one_raised_to_power_n<T, K>(this->n+1) / 
		(static_cast<T>(this->n+1) * static_cast<T>(std::numbers::pi)) * sin(static_cast<T>(this->n+1) * static_cast<T>(std::numbers::pi * 0.2) * this->x);
		this->n+=1;
		return this->current_state;
	}

};

#endif