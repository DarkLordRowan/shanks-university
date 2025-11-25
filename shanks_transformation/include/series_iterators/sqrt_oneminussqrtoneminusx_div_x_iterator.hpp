#ifndef SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP
#define SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of sqrt((1-sqrt(1-x))/x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sqrt_oneminussqrtoneminusx_div_x_iterator final : public series_base_iterator<T, K>{
public:

	sqrt_oneminussqrtoneminusx_div_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::sqrt;

		return sqrt((static_cast<T>(1) - sqrt(static_cast<T>(1) - this->x)) / this->x);
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || this->x == static_cast<T>(0) || abs(this->x) >= static_cast<T>(1).real();
        } else {
		    return !isfinite(this->x) || this->x == static_cast<T>(0) || abs(this->x) >= static_cast<T>(1);
        }

	}

	T next() override {

		using std::sqrt;

		if (this->n == 0) this->current_state = static_cast<T>(1) / sqrt(static_cast<T>(2));
		else this->current_state *= this->x * static_cast<T>(fma(4,this->n-1,1) * fma(4,this->n-1,3)) / static_cast<T>(8 * this->n * fma(2,this->n,1));

		this->n+=1;
		return this->current_state;
	}

};

#endif