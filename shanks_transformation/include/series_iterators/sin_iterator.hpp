#ifndef SIN_ITERATOR_HPP
#define SIN_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of sin(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sin_iterator final : public series_base_iterator<T, K>{
public:

	sin_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::sin(this->x);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1.0) * this->x * this->x / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 2 * this->n);
		
		this->n+=1;
		return this->current_state;
	}

};

#endif