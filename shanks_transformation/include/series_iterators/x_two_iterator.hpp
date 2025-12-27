#ifndef X_TWO_ITERATOR_HPP
#define X_TWO_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclauren series of x/2 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class x_two_iterator final : public series_base_iterator<T, K>{
public:

	x_two_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * utils::cast<T>(0.5);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		this->current_state = this->x / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3}));
		this->n+=1;
		return this->current_state;
	}

};

#endif