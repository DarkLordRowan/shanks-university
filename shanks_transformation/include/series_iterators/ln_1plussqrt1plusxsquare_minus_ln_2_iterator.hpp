#ifndef LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#define LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of log(1+sqrt(1+x^2)) - log(2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln_1plussqrt1plusxsquare_minus_ln_2_iterator final : public series_base_iterator<T, K>{
public:

	ln_1plussqrt1plusxsquare_minus_ln_2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::log;
		using std::sqrt;

		return log(static_cast<T>(1)+sqrt(static_cast<T>(1) + this->x * this->x)) - log(static_cast<T>(2));

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(1).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(1);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x * static_cast<T>(0.25);
		else this->current_state *= static_cast<T>(-1) * this->x * this->x * static_cast<T>(fma(2,this->n,1) * this->n) / static_cast<T>(2 * (this->n+1)*(this->n+1));
		
		this->n+=1;
		return this->current_state;
	}

};
#endif