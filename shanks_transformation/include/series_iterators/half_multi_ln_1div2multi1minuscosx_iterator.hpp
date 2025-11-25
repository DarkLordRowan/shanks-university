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

	T sum() const override{

		using std::log;
		using std::cos;

		return static_cast<T>(-0.5) * log(static_cast<T>(2) - static_cast<T>(2) * cos(this->x));

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= abs(static_cast<T>(std::numbers::pi));
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(std::numbers::pi);
		}

	}

	T next() override {
		using std::cos;
		
		this->current_state = cos(static_cast<T>(this->n+1) * this->x) / static_cast<T>(this->n+1);
		this->n+=1;

		return this->current_state;
	}

};
#endif