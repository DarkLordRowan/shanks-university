#ifndef MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#define MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of -0.75{x<0},x-0.75{x>=0} function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_3_div_4_or_x_minus_3_div_4_iterator final : public series_base_iterator<T, K>{
public:

	minus_3_div_4_or_x_minus_3_div_4_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= abs(static_cast<T>(0)))
				return static_cast<T>(-0.75);
		} else {
			if (this->x <= static_cast<T>(0))
           		return static_cast<T>(-0.75);
		}

		return this->x - static_cast<T>(0.75);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(3).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(3);
		}

	}

	T next() override {
		using std::pow;
		using std::sin;
		using std::cos;

		const T piDiv3 = static_cast<T>(std::numbers::pi) / static_cast<T>(3);
		
		this->current_state = static_cast<T>(-2) / 
		(piDiv3 * piDiv3 * static_cast<T>(3 * fma(2,this->n,1)*fma(2,this->n,1)))*cos(static_cast<T>(this->n+1)*piDiv3*this->x)+
		utils::minus_one_raised_to_power_n<T, K>(this->n) / (piDiv3 * static_cast<T>(this->n+1)) * sin(static_cast<T>(this->n+1) * piDiv3 * this->x);
		this->n+=1;
		return this->current_state;
	}

};

#endif