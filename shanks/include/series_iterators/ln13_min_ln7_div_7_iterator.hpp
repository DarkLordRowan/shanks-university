#ifndef LN13_MIN_LN7_DIV_7_ITERATOR_HPP
#define LN13_MIN_LN7_DIV_7_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x*log(13/7)/7 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln13_min_ln7_div_7_iterator final : public series_base_iterator<T, K>{
public:

	ln13_min_ln7_div_7_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * utils::log(utils::cast<T>(13)/utils::cast<T>(7)) / utils::cast<T>(7);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
		if (this->n == 0) this->current_state = utils::cast<T>(6) * this->x / utils::cast<T>(49);
		else this->current_state *= utils::cast<T>(-1) * utils::cast<T>(6 * this->n) / utils::cast<T>(utils::fma(size_t{7},this->n,size_t{7}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif