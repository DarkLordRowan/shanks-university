#ifndef LN_1PLUS_DIV_1PLUSX2_ITERATOR_HPP
#define LN_1PLUS_DIV_1PLUSX2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of log(1+x)/(1+x^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln_1plusx_div_1plusx2_iterator final : public series_base_iterator<T, K>{
public:

	ln_1plusx_div_1plusx2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::log(utils::cast<T>(1) + this->x) / (utils::cast<T>(1) + this->x * this->x); }
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}
	
	T next() override {

		if (this->n == 0) this->current_state = this->x  / (utils::cast<T>(1) + this->x * this->x);
		else this->current_state *= utils::cast<T>(-1) * this->x * utils::cast<T>(this->n) / (utils::cast<T>(this->n+1));
		
		this->n+=1;
		return this->current_state;
	}

};
#endif