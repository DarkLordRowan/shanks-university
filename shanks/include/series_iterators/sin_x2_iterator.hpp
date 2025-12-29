#ifndef SIN_X2_ITERATOR_HPP
#define SIN_X2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of sin(x^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sin_x2_iterator final : public series_base_iterator<T, K>{
public:

	sin_x2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::sin(this->x * this->x);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x;
		else this->current_state *= utils::cast<T>(-1) * utils::pow(this->x, utils::cast<T>(4)) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 2 * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif