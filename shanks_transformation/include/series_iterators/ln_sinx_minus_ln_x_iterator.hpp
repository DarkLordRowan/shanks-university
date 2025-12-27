#ifndef LN_SINX_MINUS_LN_X_ITERATOR_HPP
#define LN_SINX_MINUS_LN_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of log(sin(x))-log(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln_sinx_minus_ln_x_iterator final : public series_base_iterator<T, K>{
public:

	ln_sinx_minus_ln_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::log(utils::sin(this->x)) - utils::log(this->x); }
	
	bool check_validity() const override {
		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() > utils::cast<T>(std::numbers::pi).real() ||  this->x.real() < utils::cast<T>(0).real();
		} else {
			return !utils::isfinite(this->x) || this->x > utils::cast<T>(std::numbers::pi) ||  this->x < utils::cast<T>(0);
		}

	}

	T next() override {
		
		this->current_state = utils::log(utils::cast<T>(1) - this->x * this->x  / 
		(utils::cast<T>((this->n+1)*(this->n+1)) * utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi)));

		this->n+=1;
		return this->current_state;
	}

};

#endif