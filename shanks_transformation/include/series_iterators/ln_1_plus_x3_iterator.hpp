#ifndef LN_1_PLUS_X3_ITERATOR_HPP
#define LN_1_PLUS_X3_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of log(1+x^3) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln_1_plus_x3_iterator final : public series_base_iterator<T, K>{
public:

	ln_1_plus_x3_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::log;
        using std::pow;

		return log(static_cast<T>(1) + pow(this->x, static_cast<T>(3)));

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
		using std::pow;
		
		if (this->n == 0) this->current_state = pow(this->x, static_cast<T>(3));
		else this->current_state *= static_cast<T>(-1) * pow(this->x, static_cast<T>(3)) * static_cast<T>(this->n) / static_cast<T>(this->n + 1);

		this->n+=1;
		return this->current_state;
	}

};
#endif