#ifndef FOUR_LN2_M_3_ITERATOR_HPP
#define FOUR_LN2_M_3_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x*(4*ln(2)-3) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class four_ln2_m_3_iterator final : public series_base_iterator<T, K>{
public:

	four_ln2_m_3_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		using std::log;

		return this->x * (static_cast<T>(4) * log(static_cast<T>(2)) - static_cast<T>(3));
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
		
		this->current_state = utils::minus_one_raised_to_power_n<T,K>(this->n+1) * this->x / static_cast<T>((this->n+1)*(this->n+1)*(this->n+2)*(this->n+2));
		this->n+=1;
		return this->current_state;
	}

};
#endif