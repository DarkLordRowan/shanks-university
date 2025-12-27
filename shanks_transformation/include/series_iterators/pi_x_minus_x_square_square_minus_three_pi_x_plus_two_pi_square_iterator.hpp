#ifndef PI_X_MINUS_X_SQUARE_SQUARE_MINUS_THREE_PI_X_PLUS_TWO_PI_SQUARE_ITERATOR_HPP
#define PI_X_MINUS_X_SQUARE_SQUARE_MINUS_THREE_PI_X_PLUS_TWO_PI_SQUARE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of pi*x-x^2{x<=pi}, x^2-3pi*x+2pi^2 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator final : public series_base_iterator<T, K>{
public:

	pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<float_type>(std::numbers::pi))
				return utils::cast<T>(std::numbers::pi) * this->x - this->x * this->x;
		} else {
			if (this->x <= utils::cast<T>(0))
           		return utils::cast<T>(std::numbers::pi) * this->x - this->x * this->x;
		}

		return this->x * this->x - utils::cast<T>(3.0 * std::numbers::pi) * this->x + utils::cast<T>(2.0 * std::numbers::pi) * utils::cast<T>(std::numbers::pi);
	}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() < utils::cast<float_type>(0) || this->x.real() > utils::cast<float_type>(2.0 * std::numbers::pi);
		} else {
			return !utils::isfinite(this->x) || this->x < utils::cast<T>(0) || this->x > utils::cast<T>(2.0 * std::numbers::pi);
		}

	}

	T next() override {

		this->current_state = utils::cast<T>(8) * utils::sin(utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1})) * this->x) / 
		(utils::cast<T>(std::numbers::pi) * utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) *
		utils::fma(size_t{2},this->n,size_t{1})*utils::fma(size_t{2},this->n,size_t{1})));
		this->n += 1;
		return this->current_state;
	}

};

#endif