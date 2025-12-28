#ifndef LN1MX_ITERATOR_HPP
#define LN1MX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of -log(1-x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln1mx_iterator final : public series_base_iterator<T, K>{
public:

	ln1mx_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return utils::cast<T>(-1) * utils::log(utils::cast<T>(1) - this->x);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

	T next() override {
		
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x  * utils::cast<T>(this->n) / utils::cast<T>(this->n + 1);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif