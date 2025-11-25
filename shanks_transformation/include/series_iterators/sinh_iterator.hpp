#ifndef SINH_ITERATOR_HPP
#define SINH_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of sinh(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class sinh_iterator final : public series_base_iterator<T, K>{
public:

	sinh_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::sinh;

		return sinh(this->x);
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
		else this->current_state *= this->x * this->x / static_cast<T>(fma(2,this->n,1) * 2 * this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif