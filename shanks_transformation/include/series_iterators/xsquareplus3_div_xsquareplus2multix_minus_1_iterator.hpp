#ifndef XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP
#define XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Another power series of (x^2+3)/(x^2+2x)-3 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class xsquareplus3_div_xsquareplus2multix_minus_1_iterator final : public series_base_iterator<T, K>{
public:

	xsquareplus3_div_xsquareplus2multix_minus_1_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return (this->x * this->x + utils::cast<T>(3)) / (this->x * this->x + utils::cast<T>(2) * this->x) - utils::cast<T>(1);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x- utils::cast<T>(1)) >= utils::cast<float_type>(1.0);
	}

	T next() override {
		
		this->current_state = utils::cast<T>(0.5) * utils::minus_one_raised_to_power_n<T, K>(this->n) * 
		utils::cast<T>(utils::pow(size_t{3}, this->n+2) - 7) * 
		utils::pow(this->x - utils::cast<T>(1), utils::cast<T>(this->n)) / utils::cast<T>(utils::pow(size_t{3}, this->n+1));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif