#ifndef X_1MX_SQUARED_ITERATOR_HPP
#define X_1MX_SQUARED_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x/((1-x)^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class x_1mx_squared_iterator final : public series_base_iterator<T, K>{
public:

	x_1mx_squared_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return this->x / ((utils::cast<T>(1) - this->x) * (utils::cast<T>(1) - this->x));}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * utils::cast<T>(this->n + 1) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif