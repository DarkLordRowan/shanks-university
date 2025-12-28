#ifndef MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#define MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of -x{-pi<=x<0}, 0{pi>=x>=0} function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_x_minus_pi_4_or_minus_pi_4_iterator final : public series_base_iterator<T, K>{
public:

	minus_x_minus_pi_4_or_minus_pi_4_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<float_type>(0))
				return utils::cast<T>(-1) * this->x;
		} else {
			if (this->x <= utils::cast<T>(0))
           		return utils::cast<T>(-1) * this->x;
		}

		return utils::cast<T>(0);
	}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
	}

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(std::numbers::pi * 0.25);
		else {
			const T piDiv3 = utils::cast<T>(std::numbers::pi) / utils::cast<T>(3);
			const T cos_coef = (utils::minus_one_raised_to_power_n<T, K>(this->n) - utils::cast<T>(1)) / (utils::cast<T>(std::numbers::pi)*utils::cast<T>(this->n * this->n));
			const T sin_coef = utils::minus_one_raised_to_power_n<T, K>(this->n) / utils::cast<T>(this->n);
			this->current_state = cos_coef * utils::cos(utils::cast<T>(this->n) * this->x) + sin_coef * utils::sin(utils::cast<T>(this->n) * this->x);
		}

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif