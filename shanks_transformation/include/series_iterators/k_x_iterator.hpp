#ifndef K_X_ITERATOR_HPP
#define K_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of K(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class k_x_iterator final : public series_base_iterator<T, K>{
public:

	k_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		if constexpr (std::is_floating_point<T>::value){
			return std::comp_ellint_1(this->x);
		}
		#ifdef INC_FPRECISION
		else if constexpr (std::is_floating_point<T>::value){
			return float_precision(std::comp_ellint_1(static_cast<double>(this->x)), this->x.precision());
		}
		#endif
		else return static_cast<T>(0);

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(1).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(1);
		}

	}



	T next() override {

		if (this->n == 0) this->current_state = static_cast<T>(std::numbers::pi * 0.5);
		else this->current_state *= this->x * this->x * static_cast<T>(fma(2,this->n-1,1)*fma(2,this->n-1,1))/static_cast<T>(4 * this->n * this->n);
		
		this->n+=1;
		return this->current_state;
	}

};
#endif