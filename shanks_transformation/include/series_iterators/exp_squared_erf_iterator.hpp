#ifndef EXP_SQUARED_ERF_ITERATOR_HPP
#define EXP_SQUARED_ERF_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of e^x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class exp_squared_erf_iterator final : public series_base_iterator<T, K>{
public:

	exp_squared_erf_iterator() : series_base_iterator<T, K>() {}
	T sum() const override{ return utils::erf(this->x) * utils::exp(this->x * this->x);}
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(2) * this->x / utils::sqrt(utils::cast<T>(std::numbers::pi));
		else this->current_state *= utils::cast<T>(2) * this->x * this->x / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

#endif