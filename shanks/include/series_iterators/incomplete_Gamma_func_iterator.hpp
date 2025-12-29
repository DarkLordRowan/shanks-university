#ifndef INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#define INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of -0.5 log(2-2cos(x))function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class incomplete_Gamma_func_iterator final : public series_base_iterator<T, K>{
public:

	incomplete_Gamma_func_iterator() : series_base_iterator<T, K>() {}

	T alpha;

	T sum() const override{ return utils::inc_gamma(this->x, alpha);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = utils::pow(this->x, alpha) / alpha;
		else this->current_state *= utils::cast<T>(-1) * this->x * (alpha + utils::cast<T>(this->n-1)) /
		((alpha + utils::cast<T>(this->n)) * utils::cast<T>(this->n));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif