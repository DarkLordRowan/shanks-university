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

	T sum() const override{
		using std::log;
		using std::sin;

		return log(sin(this->x)) - log(this->x);

	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) 
			|| this->x.real() > static_cast<T>(std::numbers::pi).real() ||  this->x.real() < static_cast<T>(0).real();
		} else {
			return !isfinite(this->x) || this->x > static_cast<T>(std::numbers::pi) ||  this->x < static_cast<T>(0);
		}

	}

	T next() override {
		
		this->current_state = log(static_cast<T>(1) - this->x * this->x  / 
		(static_cast<T>((this->n+1)*(this->n+1)) * static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi)));

		this->n+=1;
		return this->current_state;
	}

};

#endif