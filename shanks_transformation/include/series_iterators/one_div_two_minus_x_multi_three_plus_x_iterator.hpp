#ifndef ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#define ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 1/((2-x)(3+x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_div_two_minus_x_multi_three_plus_x_iterator final : public series_base_iterator<T, K>{
public:

	one_div_two_minus_x_multi_three_plus_x_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return static_cast<T>(1) / ((static_cast<T>(2) - this->x) * (static_cast<T>(3) + this->x));
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(2).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(2);
		}

	}

	T next() override {
		using std::pow;

		this->current_state = pow(this->x, static_cast<T>(this->n)) * 
		static_cast<T>(0.2) * (utils::minus_one_raised_to_power_n<T, K>(this->n) * static_cast<T>(pow(2, this->n + 1)) + 
		static_cast<T>(pow(3, this->n + 1))) / static_cast<T>(pow(6, this->n+1));

		this->n += 1;
		return this->current_state;
	}

};

#endif