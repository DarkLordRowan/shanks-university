#ifndef TWO_LN2_ITERATOR_HPP
#define TWO_LN2_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x * 2log(2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class two_ln2_iterator final : public series_base_iterator<T, K>{
public:

	two_ln2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::log;

		return this->x * log(static_cast<T>(4));
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
		
		const K n1 = this->n + 1;
		this->current_state = static_cast<T>(12 * n1 * n1 - 1) * this->x / static_cast<T>(n1*(4*n1*n1-1)*(4*n1*n1-1));
		this->n+=1;
		return this->current_state;
	}

};

#endif