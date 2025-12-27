#ifndef HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#define HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of 0.5asin(2x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class half_minus_sinx_multi_pi_4_iterator final : public series_base_iterator<T, K>{
public:

	half_minus_sinx_multi_pi_4_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(0.5) - utils::cast<T>(std::numbers::pi * 0.25) * utils::sin(this->x); }
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() < utils::cast<float_type>(0) || this->x.real() > utils::cast<float_type>(0.5 * std::numbers::pi);
		} else {
			return !utils::isfinite(this->x) || this->x < utils::cast<T>(0) || this->x > utils::cast<T>(0.5 * std::numbers::pi);
		}
	}

	T next() override {
		this->current_state = utils::cos(utils::cast<T>(utils::fma(size_t{2},this->n,size_t{2})) * this->x) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3}));
		
		this->n+=1;
		return this->current_state;
	}

};
#endif
