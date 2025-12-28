#ifndef SQRT_1_PLUSX_ITERATOR_HPP
#define SQRT_1_PLUSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of sqrt(1+x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_iterator final : public series_base_iterator<T, K>{
public:

	sqrt_1plusx_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::sqrt(utils::cast<T>(1) + this->x);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= utils::cast<T>(-1) * this->x * (utils::cast<T>(2*this->n)-utils::cast<T>(3)) / utils::cast<T>(2*this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif