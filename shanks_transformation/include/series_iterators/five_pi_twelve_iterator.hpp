#ifndef FIVE_PI_TWELVE_ITERATOR_HPP
#define FIVE_PI_TWELVE_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of x*5pi/12 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class five_pi_twelve_iterator final : public series_base_iterator<T, K>{
public:

	five_pi_twelve_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

        return this->x * static_cast<T>(std::numbers::pi * 5) / static_cast<T>(12);
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

		if (this->n == 0) this->current_state = static_cast<T>(5) * this->x / static_cast<T>(3);
		else this->current_state = static_cast<T>(5) / static_cast<T>(3) * utils::minus_one_raised_to_power_n<T,K>(this->n) * this->x / static_cast<T>(fma(2,this->n,1));

		this->n+=1;
		return this->current_state;
	}

};

#endif