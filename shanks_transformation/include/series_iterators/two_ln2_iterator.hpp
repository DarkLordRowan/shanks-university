#ifndef TWO_LN2_ITERATOR_HPP
#define TWO_LN2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x * 2log(2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class two_ln2_iterator final : public series_base_iterator<T, K>{
public:

	two_ln2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return this->x * utils::log(utils::cast<T>(4));}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
		
		const K n1 = this->n + 1;
		this->current_state = utils::cast<T>(12 * n1 * n1 - 1) * this->x / utils::cast<T>(n1*(4*n1*n1-1)*(4*n1*n1-1));
		this->n+=1;
		return this->current_state;
	}

};

#endif