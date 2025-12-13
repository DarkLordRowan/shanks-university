#ifndef MINUS_ONE_QUARTER_ITERATOR_HPP
#define MINUS_ONE_QUARTER_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of -0.25 * x function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_one_quarter_iterator final : public series_base_iterator<T, K>{
public:

	minus_one_quarter_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{

		return this->x * static_cast<T>(-0.25);
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

		
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n + 1) * this->x / static_cast<T>((this->n+1) * (this->n+3));
		this->n+=1;
		return this->current_state;
	}

};

#endif