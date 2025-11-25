#ifndef X_ITERATOR_HPP
#define X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Fourier series of x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class x_iterator final : public series_base_iterator<T, K>{
public:

	x_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return this->x;
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(std::numbers::pi).real();
        } else {
		    return !isfinite(this->x) || abs(this->x) >= static_cast<T>(std::numbers::pi);
        }

	}

	T next() override {

		this->current_state = static_cast<T>(2) * utils::minus_one_raised_to_power_n<T, K>(this->n) / static_cast<T>(this->n+1) * 
		sin(static_cast<T>(this->n+1) * this->x);
		this->n+=1;
		return this->current_state;
	}

};

#endif