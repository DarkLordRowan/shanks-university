#ifndef SINH_ITERATOR_HPP
#define SINH_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of sinh(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sinh_iterator final : public series_base_iterator<T, K>{
public:

	sinh_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::sinh(this->x);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 2 * this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif