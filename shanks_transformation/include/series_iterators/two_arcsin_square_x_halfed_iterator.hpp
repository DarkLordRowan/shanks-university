#ifndef TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP
#define TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 2*asin(x/2)^2 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class two_arcsin_square_x_halfed_iterator final : public series_base_iterator<T, K>{
public:

	two_arcsin_square_x_halfed_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::asin;

		return static_cast<T>(2) * asin(this->x * static_cast<T>(0.5)) * asin(this->x * static_cast<T>(0.5));
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) > static_cast<T>(2).real();
        } else {
		    return !isfinite(this->x) || abs(this->x) > static_cast<T>(2);
        }

	}

	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x * static_cast<T>(0.5);
		else this->current_state *= this->x * this->x * static_cast<T>(this->n * this->n) / static_cast<T>(fma(2,this->n,1) * fma(2,this->n,2));

		this->n+=1;
		return this->current_state;
	}

};

#endif