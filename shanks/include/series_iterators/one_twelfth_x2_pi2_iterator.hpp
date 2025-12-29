#ifndef ONE_TWELFTH_X2_ITERATOR_HPP
#define ONE_TWELFTH_X2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x/12 * (x^2-pi^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_x2_pi2_iterator final : public series_base_iterator<T, K>{
public:

	one_twelfth_x2_pi2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x / utils::cast<T>(12) * (this->x * this->x - utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi));}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
		
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n+1) * 
		utils::sin(utils::cast<T>(this->n+1) * this->x) / utils::cast<T>(utils::pow(this->n+1,size_t{3}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif