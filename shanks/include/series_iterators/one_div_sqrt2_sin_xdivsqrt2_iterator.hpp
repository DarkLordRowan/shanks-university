#ifndef ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#define ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of 1/sqrt(2)*sin(x/sqrt(2)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_div_sqrt2_sin_xdivsqrt2_iterator final : public series_base_iterator<T, K>{
public:

	one_div_sqrt2_sin_xdivsqrt2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return utils::cast<T>(1) / utils::sqrt(utils::cast<T>(2)) * utils::sin(this->x * utils::cast<T>(1) / utils::sqrt(utils::cast<T>(2)));}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = this->x * utils::cast<T>(0.5);
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x / utils::cast<T>(4 * this->n * utils::fma(size_t{2},this->n,size_t{1}));

		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif