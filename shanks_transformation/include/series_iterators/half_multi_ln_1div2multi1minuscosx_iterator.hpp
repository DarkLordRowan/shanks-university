#ifndef HALF_MULTI_LN_1DIV2MULTI1MISUSCOSX_ITERATOR_HPP
#define HALF_MULTI_LN_1DIV2MULTI1MISUSCOSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of -0.5 log(2-2cos(x))function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class half_multi_ln_1div2multi1minuscosx_iterator final : public series_base_iterator<T, K>{
public:

	half_multi_ln_1div2multi1minuscosx_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(-0.5) * utils::log(utils::cast<T>(2) - utils::cast<T>(2) * utils::cos(this->x)); }

	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
	}

	T next() override {

		this->current_state = utils::cos(utils::cast<T>(this->n+1) * this->x) / utils::cast<T>(this->n+1);
		this->n+=1;

		return this->current_state;
	}

};
#endif