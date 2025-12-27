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

		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<float_type>(0))
				return utils::cast<T>(0.25 * std::numbers::pi);
		} else {
			if (this->x <= utils::cast<T>(0))
           		return utils::cast<T>(0.25 * std::numbers::pi);
		}

		return utils::cast<T>(0.25 * std::numbers::pi) - this->x;
	}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
	}

	T next() override {
	
		const K n1 = this->n+1;
		this->current_state = 
		utils::cos(utils::cast<T>(n1) * this->x) * (utils::cast<T>(1) + utils::minus_one_raised_to_power_n<T, K>(this->n)) / (utils::cast<T>(std::numbers::pi) * utils::cast<T>(n1 * n1)) + 
		utils::sin(utils::cast<T>(n1) * this->x) * utils::minus_one_raised_to_power_n<T,K>(n1) / utils::cast<T>(n1);
		this->n += 1;
		return this->current_state;
	}

};

#endif