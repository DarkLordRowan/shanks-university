#ifndef HALF_ASIN_TWO_X_ITERATOR_HPP
#define HALF_ASIN_TWO_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 0.5asin(2x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class half_asin_two_x_iterator final : public series_base_iterator<T, K>{
public:

	half_asin_two_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		using std::asin;

		return static_cast<T>(0.5) * asin(static_cast<T>(2) * this->x);

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) > abs(static_cast<T>(0.5));
		} else {
			return !isfinite(this->x) || abs(this->x) > static_cast<T>(0.5);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x * static_cast<T>(2 * this->n *fma(2,this->n-1,1)*fma(2,this->n-1,1)) / 
		static_cast<T>(this->n * this->n * fma(2,this->n,1));
		
		this->n+=1;
		return this->current_state;
	}

};
#endif
