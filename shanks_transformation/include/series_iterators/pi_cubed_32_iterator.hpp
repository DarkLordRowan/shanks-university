#ifndef PI_CUBED_32_ITERATOR_HPP
#define PI_CUBED_32_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x*pi^3/32 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_cubed_32_iterator final : public series_base_iterator<T, K>{
public:

	pi_cubed_32_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using std::pow;

		return this->x * pow(static_cast<T>(std::numbers::pi), static_cast<T>(3)) / static_cast<T>(32);
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
	
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * this->x / static_cast<T>(fma(2,this->n,1) * fma(2,this->n,1) * fma(2,this->n,1));
		this->n += 1;
		return this->current_state;
	}

};

#endif