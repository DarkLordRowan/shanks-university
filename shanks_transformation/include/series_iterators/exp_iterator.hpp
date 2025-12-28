#ifndef EXP_ITERATOR_HPP
#define EXP_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of e^x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class exp_iterator final : public series_base_iterator<T, K>{
public:

	exp_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::exp(this->x); }
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= this->x / utils::cast<T>(this->n);
		
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif