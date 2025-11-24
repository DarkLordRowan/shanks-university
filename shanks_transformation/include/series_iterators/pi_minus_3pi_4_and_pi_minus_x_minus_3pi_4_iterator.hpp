#ifndef PI_MINUS_3PI_4_AND_PI_MINUS_X_MINUS_3PI_4_ITERATOR_HPP
#define PI_MINUS_3PI_4_AND_PI_MINUS_X_MINUS_3PI_4_ITERATOR_HPP
#pragma once

#include <numbers>
#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of pi/4{x<0}, pi/4-x{x>=0} function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator final : public series_base_iterator<T, K>{
public:

	pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{

		using std::abs;

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= abs(static_cast<T>(0)))
				return static_cast<T>(0.25 * std::numbers::pi);
		} else {
			if (this->x <= static_cast<T>(0))
           		return static_cast<T>(0.25 * std::numbers::pi);
		}

		return static_cast<T>(0.25 * std::numbers::pi) - this->x;
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
			return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x.real()) >= static_cast<T>(std::numbers::pi).real();
		} else {
           	return !isfinite(this->x) || abs(this->x)>=static_cast<T>(std::numbers::pi);
		}

	}

	T next() override {
		using std::sin;
		using std::cos;
	
		const K n1 = this->n+1;

		this->current_state = 
		(static_cast<T>(1) + utils::minus_one_raised_to_power_n<T, K>(this->n)) / (static_cast<T>(std::numbers::pi) * static_cast<T>(n1 * n1)) * cos(static_cast<T>(n1) * this->x) +
		utils::minus_one_raised_to_power_n<T,K>(n1) / static_cast<T>(n1) * sin(static_cast<T>(n1) * this->x);
		this->n += 1;
		return this->current_state;
	}

};

#endif