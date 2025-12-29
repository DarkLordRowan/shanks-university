#ifndef MINUS_THREE_PLUS_LN3_THREE_DEVIDED_TWO_PLUS_TWO_LN2_ITERATOR_HPP
#define MINUS_THREE_PLUS_LN3_THREE_DEVIDED_TWO_PLUS_TWO_LN2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x*(-3+ln(3)/3+2ln(2)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator final : public series_base_iterator<T, K>{
public:

	minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * (utils::log(utils::cast<T>(4)) + utils::cast<T>(1.5) * utils::log(utils::cast<T>(3)) - utils::cast<T>(3));}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }



	T next() override {

		this->current_state = this->x / utils::cast<T>((this->n+1) * (36 * (this->n+1)*(this->n+1) - 1));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif