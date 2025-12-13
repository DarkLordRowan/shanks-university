#ifndef ONE_TWELFTH_X2_ITERATOR_HPP
#define ONE_TWELFTH_X2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x/12 * (x^2-pi^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_x2_pi2_iterator final : public series_base_iterator<T, K>{
public:

	one_twelfth_x2_pi2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return this->x / static_cast<T>(12) * (this->x * this->x - static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi));
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
		using std::sin;
		using std::pow;
		
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n+1) * sin(static_cast<T>(this->n+1) * this->x) / static_cast<T>(pow(this->n+1,3));
		this->n += 1;
		return this->current_state;
	}

};

#endif