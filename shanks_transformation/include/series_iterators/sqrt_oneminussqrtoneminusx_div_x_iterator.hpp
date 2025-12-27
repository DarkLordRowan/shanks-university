#ifndef SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP
#define SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <cmath>

/**
* @brief Maclaurin series of sqrt((1-sqrt(1-x))/x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sqrt_oneminussqrtoneminusx_div_x_iterator final : public series_base_iterator<T, K>{
public:

	sqrt_oneminussqrtoneminusx_div_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::sqrt((utils::cast<T>(1) - utils::sqrt(utils::cast<T>(1) - this->x)) / this->x);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || this->x == utils::cast<T>(0) || abs(this->x) >= utils::cast<float_type>(1);
	}

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(1) / utils::sqrt(utils::cast<T>(2));
		else this->current_state *= this->x * 
		utils::cast<T>(utils::fma(size_t{4},this->n-1,size_t{1}) * utils::fma(size_t{4},this->n-1,size_t{3})) /
		utils::cast<T>(8 * this->n * utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

#endif