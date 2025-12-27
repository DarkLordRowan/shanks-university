#ifndef ERF_ITERATOR_HPP
#define ERF_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of x*(pi/8-1/3) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class erf_iterator final : public series_base_iterator<T, K>{
public:

	erf_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{return utils::sqrt(utils::cast<T>(std::numbers::pi)) * utils::erf(this->x) * utils::cast<T>(0.5);}
	
	bool check_validity() const override { return !utils::isfinite(this->x);}

	T next() override {
	
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x * 
		utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1})) / 
		utils::cast<T>(this->n * utils::fma(size_t{2},this->n,size_t{1}));
		
		this->n+=1;
		return this->current_state;
	}

};

#endif
