#ifndef E_X_ITERATOR_HPP
#define E_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of e_x(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class e_x_iterator final : public series_base_iterator<T, K>{
public:

	e_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::e_x(this->x);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(std::numbers::pi * 0.5);
		else this->current_state *= this->x * this->x * (utils::cast<T>((this->n-1)*(this->n-1))-utils::cast<T>(0.25))/utils::cast<T>(this->n*this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif