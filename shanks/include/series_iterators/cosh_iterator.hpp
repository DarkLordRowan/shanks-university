#ifndef COSH_ITERATOR_HPP
#define COSH_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class cosh_iterator final : public series_base_iterator<T, K>{
public:

	cosh_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cosh(this->x);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= this->x * this->x / utils::cast<T>(this->n * (size_t{4} * this->n - size_t{2}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif