#ifndef MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#define MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of -x{-pi<=x<0}, 0{pi>=x>=0} function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_x_minus_pi_4_or_minus_pi_4_iterator final : public series_base_iterator<T, K>{
public:

	minus_x_minus_pi_4_or_minus_pi_4_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= static_cast<T>(0).real())
				return static_cast<T>(-1) * this->x;
		} else {
			if (this->x <= static_cast<T>(0))
           		return static_cast<T>(-1) * this->x;
		}

		return static_cast<T>(0);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(std::numbers::pi).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(std::numbers::pi);
		}

	}

	T next() override {
		using std::sin;
		using std::cos;

		if (this->n == 0) this->current_state = static_cast<T>(std::numbers::pi * 0.25);
		else {
			const T piDiv3 = static_cast<T>(std::numbers::pi) / static_cast<T>(3);
			this->current_state = 
			(utils::minus_one_raised_to_power_n<T, K>(this->n) - static_cast<T>(1)) / (static_cast<T>(std::numbers::pi)*static_cast<T>(this->n * this->n))*cos(static_cast<T>(this->n) * this->x) + 
			utils::minus_one_raised_to_power_n<T, K>(this->n) / static_cast<T>(this->n) * sin(static_cast<T>(this->n) * this->x);
		}

		this->n+=1;
		return this->current_state;
	}

};

#endif