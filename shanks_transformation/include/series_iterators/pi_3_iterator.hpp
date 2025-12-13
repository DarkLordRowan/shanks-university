#ifndef PI_3_ITERATOR_HPP
#define PI_3_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x*pi/3 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_3_iterator final : public series_base_iterator<T, K>{
public:

	pi_3_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return this->x * static_cast<T>(std::numbers::pi) / static_cast<T>(3);
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
	
		this->current_state = this->x / static_cast<T>((this->n+1)*fma(2,this->n,1)*fma(4,this->n,1));
		this->n += 1;
		return this->current_state;
	}

};

#endif