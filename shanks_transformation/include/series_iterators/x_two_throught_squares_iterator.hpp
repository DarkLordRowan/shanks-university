#ifndef X_TWO_THROUGHT_SQUARES_ITERATOR_HPP
#define X_TWO_THROUGHT_SQUARES_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Another power series of x/2 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class x_two_throught_squares_iterator final : public series_base_iterator<T, K>{
public:

	x_two_throught_squares_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return this->x * static_cast<T>(0.5);
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
		using std::pow;
		this->current_state = static_cast<T>(2 * pow(this->n + 1, 2) - 1) * this->x / static_cast<T>(4 * pow(this->n + 1, 4) + 1);
		this->n+=1;
		return this->current_state;
	}

};

#endif