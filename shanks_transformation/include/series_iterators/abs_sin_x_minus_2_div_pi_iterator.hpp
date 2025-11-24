#ifndef ABS_SIN_X_MINUS_2_DIV_PI_ITERATOR_HPP
#define ABS_SIN_X_MINUS_2_DIV_PI_ITERATOR_HPP
#pragma once

#include <numbers>
#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of pi/4{x<0}, pi/4-x{x>=0} function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class abs_sin_x_minus_2_div_pi_iterator final : public series_base_iterator<T, K>{
public:

	abs_sin_x_minus_2_div_pi_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		using std::sin;

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= static_cast<T>(std::numbers::pi).real())
				return sin(this->x) - static_cast<T>(2) / static_cast<T>(std::numbers::pi);
		} else {
			if (this->x <= static_cast<T>(std::numbers::pi))
           		return sin(this->x) - static_cast<T>(2) / static_cast<T>(std::numbers::pi);
		}

		return static_cast<T>(-1)*sin(this->x) - static_cast<T>(2) / static_cast<T>(std::numbers::pi);
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
		using std::cos;

		this->current_state = static_cast<T>(-4) * cos(static_cast<T>(fma(2,this->n,2))*this->x) / 
		static_cast<T>(fma(2,this->n,1)*fma(2,this->n,3)) / static_cast<T>(std::numbers::pi);
	
		this->n += 1;
		return this->current_state;
	}

};

#endif