#ifndef MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#define MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of -0.75{x<0},x-0.75{x>=0} function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_3_div_4_or_x_minus_3_div_4_iterator final : public series_base_iterator<T, K>{
public:

	minus_3_div_4_or_x_minus_3_div_4_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<float_type>(0))
				return utils::cast<T>(-0.75);
		} else {
			if (this->x <= utils::cast<T>(0))
           		return utils::cast<T>(-0.75);
		}

		return this->x - utils::cast<T>(0.75);
	}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(3.0);
	}

	T next() override {

		const T piDiv3 = utils::cast<T>(std::numbers::pi) / utils::cast<T>(3);
		const T shifted_x = utils::cast<T>(this->n+1)*piDiv3*this->x;

		this->current_state = utils::cast<T>(-2) / 
		(piDiv3 * piDiv3 * utils::cast<T>(3 * utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{1}))) * 
		utils::cos(shifted_x) + utils::minus_one_raised_to_power_n<T, K>(this->n) / (piDiv3 * utils::cast<T>(this->n+1)) * utils::sin(shifted_x);
		this->n+=1;
		return this->current_state;
	}

};

#endif