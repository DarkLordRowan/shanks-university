#ifndef FIVE_PI_TWELVE_ITERATOR_HPP
#define FIVE_PI_TWELVE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of x*5pi/12 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class five_pi_twelve_iterator final : public series_base_iterator<T, K>{
public:

	five_pi_twelve_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return this->x * utils::cast<T>(std::numbers::pi * 5) / utils::cast<T>(12);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(5) * this->x / utils::cast<T>(3);
		else this->current_state = utils::cast<T>(5) / utils::cast<T>(3) * 
		utils::minus_one_raised_to_power_n<T,K>(this->n) * this->x / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

#endif