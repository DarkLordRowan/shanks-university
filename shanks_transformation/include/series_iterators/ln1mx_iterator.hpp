#ifndef LN1MX_ITERATOR_HPP
#define LN1MX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of -log(1-x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln1mx_iterator final : public series_base_iterator<T, K>{
public:

	ln1mx_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using std::log;

		return static_cast<T>(-1) * log(static_cast<T>(1) - this->x);
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(1).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(1);
		}

	}

	T next() override {
		
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x  * static_cast<T>(this->n) / static_cast<T>(this->n + 1);

		this->n+=1;
		return this->current_state;
	}

};

#endif