#ifndef XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP
#define XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Another power series of (x^2+3)/(x^2+2x)-3 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class xsquareplus3_div_xsquareplus2multix_minus_1_iterator final : public series_base_iterator<T, K>{
public:

	xsquareplus3_div_xsquareplus2multix_minus_1_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return (this->x * this->x + static_cast<T>(3)) / (this->x * this->x + static_cast<T>(2) * this->x) - static_cast<T>(1);
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x - static_cast<T>(1)) >= static_cast<T>(1).real();
        } else {
		    return !isfinite(this->x) || abs(this->x - static_cast<T>(1)) >= static_cast<T>(1);
        }

	}

	T next() override {
		using std::pow;
		this->current_state = static_cast<T>(0.5) * utils::minus_one_raised_to_power_n<T, K>(this->n) * 
		static_cast<T>(pow(3, this->n+2) - 7) * pow(this->x - static_cast<T>(1), static_cast<T>(this->n)) / static_cast<T>(pow(3, this->n+1));
		this->n+=1;
		return this->current_state;
	}

};

#endif