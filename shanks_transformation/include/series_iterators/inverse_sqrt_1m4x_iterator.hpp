#ifndef INVERSE_SQRT_1M4X_ITERATOR_HPP
#define INVERSE_SQRT_1M4X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 1/sqrt(1-4x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class inverse_sqrt_1m4x_iterator final : public series_base_iterator<T, K>{
public:

	inverse_sqrt_1m4x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		using std::sqrt;

		return static_cast<T>(1) / sqrt(static_cast<T>(1) - static_cast<T>(4) * this->x);

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(0.25).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(0.25);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = static_cast<T>(1);
		else this->current_state *= this->x * static_cast<T>(2 * fma(2,this->n-1,1)) / static_cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};
#endif