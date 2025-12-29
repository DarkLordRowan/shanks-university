#ifndef SERIES_WITH_LN_NUMBER2_ITERATOR_HPP
#define SERIES_WITH_LN_NUMBER2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of ln_number_2(x * 5.71617784132) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number2_iterator final : public series_base_iterator<T, K>{
public:

	series_with_ln_number2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(5.71617784132) * this->x;}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		const T n1 = utils::cast<T>(this->n + 1);
		const T n1_2 = n1 * n1;

		this->current_state = this->x / utils::pow(utils::log(utils::cast<T>(this->n+2)), utils::log(utils::cast<T>(this->n+2)));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif