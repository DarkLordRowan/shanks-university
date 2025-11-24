#ifndef PI_X_MULTI_E_XPI_PLUS_E_MINUSXPI_DIVIDED_E_XPI_MINUS_E_MINUSXPI_ITERATOR_HPP
#define PI_X_MULTI_E_XPI_PLUS_E_MINUSXPI_DIVIDED_E_XPI_MINUS_E_MINUSXPI_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x * (pi^2/12) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator final : public series_base_iterator<T, K>{
public:

	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		const T pi = static_cast<T>(std::numbers::pi);
		return pi * this->x * (exp(pi*this->x)+exp(-pi*this->x)) / (exp(pi*this->x) - exp(-pi*this->x)) - static_cast<T>(1);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag());
		} else {
			return !isfinite(this->x);
		}

	}

	T next() override {

		this->current_state = static_cast<T>(2) * this->x * this->x / (this->x * this->x + static_cast<T>((this->n+1) * (this->n+1)));
		this->n += 1;
		return this->current_state;
	}

};

#endif