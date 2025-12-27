#ifndef ONE_TWELFTH_3X2_PI2_ITERATOR_HPP
#define ONE_TWELFTH_3X2_PI2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 1/12*(3*x^2-pi^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_3x2_pi2_iterator final : public series_base_iterator<T, K>{
public:

	one_twelfth_3x2_pi2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return utils::cast<T>(1) / utils::cast<T>(12) * (utils::cast<T>(3) * this->x * this->x - utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi));
	}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(std::numbers::pi);
	}

	T next() override {
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n+1) * 
		utils::cos(utils::cast<T>(this->n+1) * this->x) / utils::cast<T>((this->n + 1) * (this->n + 1));
		this->n += 1;
		return this->current_state;
	}

};

#endif