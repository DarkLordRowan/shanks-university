#ifndef ERF_ITERATOR_HPP
#define ERF_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of x*(pi/8-1/3) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class erf_iterator final : public series_base_iterator<T, K>{
public:

	erf_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::erf;

		if constexpr(isComplexLike<T>::value){
			return static_cast<T>(0);
		} else {
			return sqrt(static_cast<T>(std::numbers::pi)) * erf(this->x) * static_cast<T>(0.5);
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
	
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= static_cast<T>(-1) * this->x * this->x * static_cast<T>(fma(2,this->n-1,1)) / static_cast<T>(this->n * fma(2,this->n,1));
		
		this->n+=1;
		return this->current_state;
	}

};

#endif
