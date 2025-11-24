#ifndef ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#define ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 1/sqrt(2)*sin(x/sqrt(2)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_div_sqrt2_sin_xdivsqrt2_iterator final : public series_base_iterator<T, K>{
public:

	one_div_sqrt2_sin_xdivsqrt2_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using std::sin;
        using std::sqrt;

		return  static_cast<T>(1) / sqrt(static_cast<T>(2)) * sin(this->x * static_cast<T>(1) / sqrt(static_cast<T>(2)));
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

		if (this->n == 0) this->current_state = this->x * static_cast<T>(0.5);
		else this->current_state *= static_cast<T>(-1) * this->x * this->x / static_cast<T>(4 * this->n * fma(2,this->n,1));

		this->n += 1;
		return this->current_state;
	}

};

#endif