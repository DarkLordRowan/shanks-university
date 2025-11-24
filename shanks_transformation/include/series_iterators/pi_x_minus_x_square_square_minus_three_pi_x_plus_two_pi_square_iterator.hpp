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
		using std::abs;

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= static_cast<T>(std::numbers::pi).real())
				return static_cast<T>(std::numbers::pi) * this->x - this->x * this->x;
		} else {
			if (this->x <= static_cast<T>(0))
           		return static_cast<T>(std::numbers::pi) * this->x - this->x * this->x;
		}

		return this->x * this->x - static_cast<T>(3.0 * std::numbers::pi) * this->x + static_cast<T>(2.0 * std::numbers::pi) * static_cast<T>(std::numbers::pi);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag())
			|| this->x.real() < static_cast<T>(0).real() || this->x.real() > static_cast<T>(2.0 * std::numbers::pi).real();
		} else {
			return !isfinite(this->x) || this->x < static_cast<T>(0) || this->x > static_cast<T>(2.0 * std::numbers::pi);
		}

	}

	T next() override {

		this->current_state = static_cast<T>(8) * sin(static_cast<T>(fma(2,this->n,1)) * this->x) / 
		(static_cast<T>(std::numbers::pi) * static_cast<T>(fma(2,this->n,1)*fma(2,this->n,1)*fma(2,this->n,1)));
		this->n += 1;
		return this->current_state;
	}

};

#endif