#ifndef LN2_ITERATOR_HPP
#define LN2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x*log(2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln2_iterator final : public series_base_iterator<T, K>{
public:

	ln2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using std::log;

		return this->x * log(static_cast<T>(2));
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

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= static_cast<T>(-1)  * static_cast<T>(this->n) / static_cast<T>(this->n + 1);

		this->n+=1;
		return this->current_state;
	}

};

#endif