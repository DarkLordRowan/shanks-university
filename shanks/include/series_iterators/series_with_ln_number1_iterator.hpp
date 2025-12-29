#ifndef SERIES_WITH_LN_NUMBER1_ITERATOR_HPP
#define SERIES_WITH_LN_NUMBER1_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of ln_number_1(x * 0.599195688977) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number1_iterator final : public series_base_iterator<T, K>{
public:

	series_with_ln_number1_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(0.599195688977) * this->x;}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
		const T n1 = utils::cast<T>(this->n + 1);
		const T n1_2 = n1 * n1;

		this->current_state = utils::log(utils::cast<T>(1) + 
		utils::pow(n1, n1_2 + n1 * utils::cast<T>(0.5)) / (utils::pow(utils::cast<T>(utils::fact<K>(this->n+1)), n1) * utils::exp(n1_2))) * this->x;
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif