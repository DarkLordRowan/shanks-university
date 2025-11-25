#ifndef THREE_MINUS_PI_ITERATOR_HPP
#define THREE_MINUS_PI_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x * (3 - pi) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class three_minus_pi_iterator final : public series_base_iterator<T, K>{
public:

	three_minus_pi_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		return this->x * (static_cast<T>(3) - static_cast<T>(std::numbers::pi));
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

		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n+1) * this->x / static_cast<T>((this->n + 1)*(this->n+2)*fma(2,this->n,3));
		this->n+=1;
		return this->current_state;
	}

};

#endif