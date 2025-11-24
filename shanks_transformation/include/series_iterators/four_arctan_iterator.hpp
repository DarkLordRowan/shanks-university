#ifndef FOUR_ARCTAN_ITERATOR_HPP
#define FOUR_ARCTAN_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 4atan(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class four_arctan_iterator final : public series_base_iterator<T, K>{
public:

	four_arctan_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::atan;

		return static_cast<T>(4) * atan(this->x);
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) > static_cast<T>(1).real();
		} else {
			return !isfinite(this->x) || abs(this->x) > static_cast<T>(1);
		}

	}


	T next() override {

		if (this->n == 0) this->current_state = static_cast<T>(4) * this->x;
		else this->current_state *= static_cast<T>(-1) * this->x * this->x * static_cast<T>(2 * this->n - 1) / static_cast<T>(fma(2,this->n,1));

		this->n+=1;
		return this->current_state;
	}

};
#endif