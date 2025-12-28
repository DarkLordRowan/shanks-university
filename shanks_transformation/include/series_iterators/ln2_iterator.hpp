#ifndef LN2_ITERATOR_HPP
#define LN2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x*log(2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln2_iterator final : public series_base_iterator<T, K>{
public:

	ln2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * utils::log(utils::cast<T>(2));}
	
	bool check_validity() const override { return !utils::isfinite(this->x);}

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1)  * utils::cast<T>(this->n) / utils::cast<T>(this->n + 1);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif