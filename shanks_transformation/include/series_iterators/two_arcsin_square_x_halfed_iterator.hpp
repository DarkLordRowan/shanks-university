#ifndef TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP
#define TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 2*asin(x/2)^2 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class two_arcsin_square_x_halfed_iterator final : public series_base_iterator<T, K>{
public:

	two_arcsin_square_x_halfed_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(2) * utils::asin(this->x * utils::cast<T>(0.5)) * utils::asin(this->x * utils::cast<T>(0.5));}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(2.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x * utils::cast<T>(0.5);
		else this->current_state *= this->x * this->x * utils::cast<T>(this->n * this->n) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{2}));

		this->n+=1;
		return this->current_state;
	}

};

#endif