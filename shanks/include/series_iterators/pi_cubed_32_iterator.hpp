#ifndef PI_CUBED_32_ITERATOR_HPP
#define PI_CUBED_32_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x*pi^3/32 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_cubed_32_iterator final : public series_base_iterator<T, K>{
public:

	pi_cubed_32_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * utils::pow(utils::cast<T>(std::numbers::pi), utils::cast<T>(3)) / utils::cast<T>(32);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
	
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * this->x / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 
		utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{1}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif