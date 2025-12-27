#ifndef X_MIN_SQRT_X_ITERATOR_HPP
#define X_MIN_SQRT_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x-sqrt(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class x_min_sqrt_x_iterator final : public series_base_iterator<T, K>{
public:

	x_min_sqrt_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return this->x - utils::sqrt(this->x); }
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x - utils::cast<T>(1)) >= utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = this->x - utils::cast<T>(1);
		else if (this->n == 1) this->current_state = (this->x - utils::cast<T>(1)) * utils::cast<T>(-0.5);
		else this->current_state *= utils::cast<T>(-1) * (this->x - utils::cast<T>(1)) * utils::cast<T>(0.5) * utils::cast<T>(2*this->n - 3) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif