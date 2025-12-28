#ifndef LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#define LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of log(1+sqrt(1+x^2)) - log(2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln_1plussqrt1plusxsquare_minus_ln_2_iterator final : public series_base_iterator<T, K>{
public:

	ln_1plussqrt1plusxsquare_minus_ln_2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::log(utils::cast<T>(1)+utils::sqrt(utils::cast<T>(1) + this->x * this->x)) - utils::log(utils::cast<T>(2)); }
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x * utils::cast<T>(0.25);
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x * 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * this->n) / utils::cast<T>(2 * (this->n+1)*(this->n+1));
		
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif