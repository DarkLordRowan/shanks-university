#ifndef ONE_ITERATOR_HPP
#define ONE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_iterator final : public series_base_iterator<T, K>{
public:

	one_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x;}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
		
		this->current_state = this->x / utils::cast<T>((this->n+1)*(this->n+2));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif