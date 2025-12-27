#ifndef CI_X_ITERATOR_HPP
#define CI_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of ci_x(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ci_x_iterator final : public series_base_iterator<T, K>{
public:

	ci_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::ci_x(this->x);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(std::numbers::egamma);
		else if (this->n == 1) this->current_state = utils::log(this->x);
		else if (this->n == 2) this->current_state = utils::cast<T>(-0.25) * this->x * this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x / utils::cast<T>(2*(this->n-1)*(this->n-1)*utils::fma(size_t{2},this->n-2,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

#endif