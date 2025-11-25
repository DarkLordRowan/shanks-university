#ifndef SQRT_1PLUSX_MIN_1_MIN_X_DIV_2_ITERATOR_HPP
#define SQRT_1PLUSX_MIN_1_MIN_X_DIV_2_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of sqrt(1+x)-1-1x/2 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_min_1_min_x_div_2_iterator final : public series_base_iterator<T, K>{
public:

	sqrt_1plusx_min_1_min_x_div_2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::sqrt;

		return sqrt(static_cast<T>(1) + this->x) - static_cast<T>(1) - this->x * static_cast<T>(0.5);
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) > static_cast<T>(1).real();
		} else {
			return !isfinite(this->x) || abs(this->x) > static_cast<T>(1);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x * static_cast<T>(-0.125);
		else this->current_state *= static_cast<T>(-1) * this->x * static_cast<T>(fma(2,this->n,1)) / static_cast<T>(fma(2,this->n,2));

		this->n+=1;
		return this->current_state;
	}

};

#endif