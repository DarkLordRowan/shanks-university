#ifndef HALF_ASIN_TWO_X_ITERATOR_HPP
#define HALF_ASIN_TWO_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of 0.5asin(2x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class half_asin_two_x_iterator final : public series_base_iterator<T, K>{
public:

	half_asin_two_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(0.5) * utils::asin(utils::cast<T>(2) * this->x); }
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(0.5);
	}

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x * utils::cast<T>(2 * this->n * utils::fma(size_t{2},this->n-1,size_t{1}) * 
		utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(this->n * this->n * utils::fma(size_t{2},this->n,size_t{1}));
		
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
