#ifndef LN_COSX_ITERATOR_HPP
#define LN_COSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of log(cos(x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class ln_cosx_iterator final : public series_base_iterator<T, K>{
public:

	ln_cosx_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::log;
		using std::cos;

		return log(cos(this->x));

	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(std::numbers::pi * 0.5).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(std::numbers::pi * 0.5);
		}

	}

	T next() override {
		using std::sin;

		if (this->n == 0) this->current_state = sin(this->x) * sin(this->x) * static_cast<T>(-0.5);
		else this->current_state *= sin(this->x) * sin(this->x) * static_cast<T>(this->n) / static_cast<T>(this->n+1);

		this->n+=1;
		return this->current_state;
	}

};

#endif