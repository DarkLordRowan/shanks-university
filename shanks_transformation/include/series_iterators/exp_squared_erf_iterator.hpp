#ifndef EXP_SQUARED_ERF_ITERATOR_HPP
#define EXP_SQUARED_ERF_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of e^x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class exp_squared_erf_iterator final : public series_base_iterator<T, K>{
public:

	exp_squared_erf_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

        using std::erf;
		using std::exp;

		if constexpr(isComplexLike<T>::value){
			return static_cast<T>(0);
		} else {
			return erf(this->x) * exp(this->x * this->x);
		}
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag());
		} else {
			return !isfinite(this->x);
		}

	}

	T next() override {
		using std::sqrt;

		if (this->n == 0) this->current_state = static_cast<T>(2) * this->x / sqrt(static_cast<T>(std::numbers::pi));
		else this->current_state *= static_cast<T>(2) * this->x * this->x / static_cast<T>(fma(2,this->n,1));

		this->n+=1;
		return this->current_state;
	}

};

#endif