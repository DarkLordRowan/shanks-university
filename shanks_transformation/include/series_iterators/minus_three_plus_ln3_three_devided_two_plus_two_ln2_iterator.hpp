#ifndef MINUS_THREE_PLUS_LN3_THREE_DEVIDED_TWO_PLUS_TWO_LN2_ITERATOR_HPP
#define MINUS_THREE_PLUS_LN3_THREE_DEVIDED_TWO_PLUS_TWO_LN2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x*(-3+ln(3)/3+2ln(2)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator final : public series_base_iterator<T, K>{
public:

	minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using std::log;

		return this->x * (log(static_cast<T>(4)) + static_cast<T>(1.5) * log(static_cast<T>(3)) - static_cast<T>(3));
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag());
		} else {
			return !isfinite(this->x);
		}

	}



	T next() override {

		this->current_state = this->x / static_cast<T>((this->n+1) * (36 * (this->n+1)*(this->n+1) - 1));
		this->n += 1;
		return this->current_state;
	}

};

#endif