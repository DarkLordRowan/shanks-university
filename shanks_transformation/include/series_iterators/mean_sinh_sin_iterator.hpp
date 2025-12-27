#ifndef MEAN_SINH_SIN_ITERATOR_HPP
#define MEAN_SINH_SIN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 0.5*(sin(x)+sinh(x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class mean_sinh_sin_iterator final : public series_base_iterator<T, K>{
public:

	mean_sinh_sin_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return (utils::sinh(this->x) + utils::sin(this->x)) * utils::cast<T>(0.5);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::pow(this->x, utils::cast<T>(4)) / 
		utils::cast<T>(
			utils::fma(size_t{4},this->n-1,size_t{5})*utils::fma(size_t{4},this->n-1,size_t{4}) * 
			utils::fma(size_t{4},this->n-1,size_t{3})*utils::fma(size_t{4},this->n-1,size_t{2})
		);

		this->n+=1;
		return this->current_state;
	}

};

#endif