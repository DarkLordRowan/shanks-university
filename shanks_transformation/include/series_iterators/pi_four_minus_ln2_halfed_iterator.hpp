#ifndef PI_FOUR_MINUS_LN2_HALFED_ITERATOR_HPP
#define PI_FOUR_MINUS_LN2_HALFED_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x*(pi/4-log(2)/2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_four_minus_ln2_halfed_iterator final : public series_base_iterator<T, K>{
public:

	pi_four_minus_ln2_halfed_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * (utils::cast<T>(std::numbers::pi * 0.25) - utils::log(utils::cast<T>(2)) * utils::cast<T>(0.5)); }
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
	
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * this->x * 
		(utils::cast<T>(1) / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1})) - utils::cast<T>(1) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{2})));
		this->n += 1;
		return this->current_state;
	}

};

#endif