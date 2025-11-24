#ifndef TWO_DEGREE_X_ITERATOR_HPP
#define TWO_DEGREE_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 2^x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class two_degree_x_iterator final : public series_base_iterator<T, K>{
public:

	two_degree_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::pow;

		return pow(static_cast<T>(2), this->x);
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
		using std::log;

		if (this->n == 0) this->current_state = static_cast<T>(1);
		else this->current_state *= this->x * log(static_cast<T>(2)) / static_cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif