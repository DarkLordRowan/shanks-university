#ifndef RIEMANN_ZETA_FUNC_ITERATOR_HPP
#define RIEMANN_ZETA_FUNC_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of dzeta(z) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_iterator final : public series_base_iterator<T, K>{
public:

	riemann_zeta_func_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		if constexpr (std::is_floating_point<T>::value){
			return std::riemann_zeta(this->x);
		}
		#ifdef INC_FPRECISION
		if constexpr (std::is_same<T, float_precision>::value){
			return abs(zeta(this->x));
		}
		#endif 
		else return static_cast<T>(0);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || this->x.real() < static_cast<T>(1).real();
		} else {
			return !isfinite(this->x) || this->x < static_cast<T>(1);
		}

	}

	T next() override {
		using std::pow;

		this->current_state = pow(static_cast<T>(this->n+1), static_cast<T>(-1) * this->x);
		this->n += 1;
		return this->current_state;
	}

};

#endif