#ifndef ABS_SIN_X_MINUS_2_DIV_PI_ITERATOR_HPP
#define ABS_SIN_X_MINUS_2_DIV_PI_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of pi/4{x<0}, pi/4-x{x>=0} function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class abs_sin_x_minus_2_div_pi_iterator final : public series_base_iterator<T, K>{
public:

	abs_sin_x_minus_2_div_pi_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<T>(std::numbers::pi).real())
				return utils::sin(this->x) - utils::cast<T>(2) / utils::cast<T>(std::numbers::pi);
		} else {
			if (this->x <= utils::cast<T>(std::numbers::pi))
           		return utils::sin(this->x) - utils::cast<T>(2) / utils::cast<T>(std::numbers::pi);
		}

		return utils::cast<T>(-1) * utils::sin(this->x) - utils::cast<T>(2) / utils::cast<T>(std::numbers::pi);
	}
	
	bool check_validity() const override {

		if constexpr (isComplexLike<T>::value){
			return !utils::isfinite(this->x) || this->x.real() < utils::cast<T>(0).real() || this->x.real() > utils::cast<T>(2.0 * std::numbers::pi).real();
		} else {
           	return !utils::isfinite(this->x) || this->x < utils::cast<T>(0) || this->x > utils::cast<T>(2.0 * std::numbers::pi);
		}

	}

	T next() override {
	
		this->current_state = utils::cast<T>(-4) * utils::cos(utils::cast<T>(utils::fma(size_t{2},this->n,size_t{2}))*this->x) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3})) / utils::cast<T>(std::numbers::pi);
	
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif