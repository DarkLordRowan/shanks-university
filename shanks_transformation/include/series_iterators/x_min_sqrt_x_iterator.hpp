#ifndef X_MIN_SQRT_X_ITERATOR_HPP
#define X_MIN_SQRT_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x-sqrt(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class x_min_sqrt_x_iterator final : public series_base_iterator<T, K>{
public:

	x_min_sqrt_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::sqrt;

		return this->x - sqrt(this->x);
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x - static_cast<T>(1)) >= static_cast<T>(1).real();
        } else {
		    return !isfinite(this->x) || abs(this->x - static_cast<T>(1)) >= static_cast<T>(1);
        }

	}

	T next() override {

		if (this->n == 0) this->current_state = this->x - static_cast<T>(1);
		else if (this->n == 1) this->current_state = (this->x - static_cast<T>(1)) * static_cast<T>(-0.5);
		else this->current_state *= static_cast<T>(-1) * (this->x - static_cast<T>(1)) * static_cast<T>(0.5) * static_cast<T>(2*this->n - 3) / static_cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif