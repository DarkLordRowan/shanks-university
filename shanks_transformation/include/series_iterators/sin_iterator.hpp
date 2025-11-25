#ifndef SIN_ITERATOR_HPP
#define SIN_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of sin(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sin_iterator final : public series_base_iterator<T, K>{
public:

	sin_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::sin;

		return sin(this->x);
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
		else this->current_state *= static_cast<T>(-1) * this->x * this->x / static_cast<T>(fma(2,this->n,1) * 2 * this->n);
		
		this->n+=1;
		return this->current_state;
	}

};

#endif