#ifndef PI_MINUS_X_2_ITERATOR_HPP
#define PI_MINUS_X_2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of (pi-2)/x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_minus_x_2_iterator final : public series_base_iterator<T, K>{
public:

	pi_minus_x_2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return (utils::cast<T>(std::numbers::pi) - this->x) * utils::cast<T>(0.5);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

		if constexpr (isComplexLike<T>::value){
			return !utils::isfinite(this->x) || this->x.real() <= utils::cast<float_type>(0) || this->x.real() >= utils::cast<float_type>(std::numbers::pi);
		} else {
           	return !utils::isfinite(this->x) || this->x <= utils::cast<T>(0) || this->x >= utils::cast<T>(std::numbers::pi);
		}

	}

	T next() override {

		this->current_state = utils::sin(utils::cast<T>(this->n+1) * this->x) / utils::cast<T>(this->n+1);
		this->n += 1;
		return this->current_state;
	}

};

#endif