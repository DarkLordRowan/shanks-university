#ifndef THREE_MINUS_PI_ITERATOR_HPP
#define THREE_MINUS_PI_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x * (3 - pi) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class three_minus_pi_iterator final : public series_base_iterator<T, K>{
public:

	three_minus_pi_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return this->x * (utils::cast<T>(3) - utils::cast<T>(std::numbers::pi));}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n+1) * this->x / 
		utils::cast<T>((this->n + 1)*(this->n+2)*utils::fma(size_t{2},this->n,size_t{3}));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif