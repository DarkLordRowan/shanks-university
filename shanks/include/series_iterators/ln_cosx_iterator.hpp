#ifndef LN_COSX_ITERATOR_HPP
#define LN_COSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of log(cos(x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln_cosx_iterator final : public series_base_iterator<T, K>{
public:

	ln_cosx_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::log(utils::cos(this->x));}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi * 0.5);
	}

	T next() override {

		if (this->n == 0) this->current_state = utils::sin(this->x) * utils::sin(this->x) * utils::cast<T>(-0.5);
		else this->current_state *= utils::sin(this->x) * utils::sin(this->x) * utils::cast<T>(this->n) / utils::cast<T>(this->n+1);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif