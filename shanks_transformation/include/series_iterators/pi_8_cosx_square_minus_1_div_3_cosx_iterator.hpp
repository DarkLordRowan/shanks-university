#ifndef PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#define PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of pi/8*cos(x)^2-cos(x)/3 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_8_cosx_square_minus_1_div_3_cosx_iterator final : public series_base_iterator<T, K>{
public:

	pi_8_cosx_square_minus_1_div_3_cosx_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using std::cos;

		return static_cast<T>(std::numbers::pi * 0.125) * cos(this->x) * cos(this->x) - cos(this->x) / static_cast<T>(3);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) > static_cast<T>(std::numbers::pi * 0.5).real();
        } else {
		    return !isfinite(this->x) || abs(this->x) > static_cast<T>(std::numbers::pi * 0.5);
        }

	}

	T next() override {
	
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * cos(static_cast<T>(fma(2,this->n,3)) * this->x) / 
		static_cast<T>(fma(2,this->n,1) * fma(2,this->n,3) * fma(2,this->n,5));
		this->n += 1;
		return this->current_state;
	}

};

#endif