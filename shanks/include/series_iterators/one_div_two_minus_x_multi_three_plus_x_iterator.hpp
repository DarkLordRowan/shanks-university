#ifndef ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#define ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of 1/((2-x)(3+x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_div_two_minus_x_multi_three_plus_x_iterator final : public series_base_iterator<T, K>{
public:

	one_div_two_minus_x_multi_three_plus_x_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return utils::cast<T>(1) / ((utils::cast<T>(2) - this->x) * (utils::cast<T>(3) + this->x));
	}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(2.0);
	}

	T next() override {
		
		this->current_state = utils::pow(this->x, utils::cast<T>(this->n)) * 
		utils::cast<T>(0.2) * (utils::minus_one_raised_to_power_n<T, K>(this->n) * utils::cast<T>(utils::pow(size_t{2}, this->n + 1)) + 
		utils::cast<T>(utils::pow(size_t{3}, this->n + 1))) / utils::cast<T>(utils::pow(size_t{6}, this->n+1));

		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif