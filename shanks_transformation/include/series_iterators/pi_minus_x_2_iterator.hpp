#ifndef PI_MINUS_X_2_ITERATOR_HPP
#define PI_MINUS_X_2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
* @brief Maclaurin series of (pi-2)/x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_minus_x_2_iterator final : public series_base_iterator<T, K>{
public:

	pi_minus_x_2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{

		return (static_cast<T>(std::numbers::pi) - this->x) * static_cast<T>(0.5);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
			return !isfinite(this->x.real()) || !isfinite(this->x.imag())
			|| this->x.real() <= abs(static_cast<T>(0)) || this->x.real() >= abs(static_cast<T>(std::numbers::pi));
		} else {
           	return !isfinite(this->x) || this->x <= static_cast<T>(0) || this->x >= static_cast<T>(std::numbers::pi);
		}

	}

	T next() override {

		using std::sin;
	
		this->current_state = sin(static_cast<T>(this->n+1) * this->x) / static_cast<T>(this->n+1);
		this->n += 1;
		return this->current_state;
	}

};

#endif