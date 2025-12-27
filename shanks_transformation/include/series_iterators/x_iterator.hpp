#ifndef X_ITERATOR_HPP
#define X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Fourier series of x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class x_iterator final : public series_base_iterator<T, K>{
public:

	x_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x;}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
	}

	T next() override {

		this->current_state = utils::cast<T>(2) * utils::minus_one_raised_to_power_n<T, K>(this->n) / utils::cast<T>(this->n+1) * 
		utils::sin(utils::cast<T>(this->n+1) * this->x);
		this->n+=1;
		return this->current_state;
	}

};

#endif