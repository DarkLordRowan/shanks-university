#ifndef LAMBERT_W_FUNC_ITERATOR_HPP
#define LAMBERT_W_FUNC_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <gsl/gsl_sf_lambert.h>
#ifndef INC_FPRECISION
    #include "libs/arbitrary_arithmetics/fprecision.h"
#endif

/**
* @brief Maclaurin series of W0(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class lambert_W_func_iterator final : public series_base_iterator<T, K>{
public:

	lambert_W_func_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		if constexpr (std::is_floating_point<T>::value){
			return static_cast<T>(gsl_sf_lambert_W0(static_cast<double>(this->x)));
		}
		#ifdef INC_FPRECISION
		else if constexpr (std::is_same<T, float_precision>::value){
			return lambertW0(this->x);
		}
		#endif
		else return static_cast<T>(0);

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= (static_cast<T>(1) / static_cast<T>(std::numbers::e)).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(1) / static_cast<T>(std::numbers::e);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= static_cast<T>(-1) * this->x / static_cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif