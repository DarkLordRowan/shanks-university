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

	T sum() const override{ return utils::sqrt(utils::cast<T>(1) + this->x) - utils::cast<T>(1) - this->x * utils::cast<T>(0.5);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x * utils::cast<T>(-0.125);
		else this->current_state *= utils::cast<T>(-1) * this->x * 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1})) / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{2}));

		this->n+=1;
		return this->current_state;
	}

};

#endif