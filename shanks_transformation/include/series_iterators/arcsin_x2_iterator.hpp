#ifndef ARCSIN_X2_ITERATOR_HPP
#define ARCSIN_X2_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of arcsin(x^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class arcsin_x2_iterator final : public series_base_iterator<T, K>{
public:

	arcsin_x2_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		using std::asin;

        return asin(this->x * this->x);
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
		using std::pow;

		if (this->n == 0) this->current_state = this->x * this->x;
		else this->current_state *= pow(this->x, static_cast<T>(4)) * 
		static_cast<T>(fma(2,this->n-1,1)*fma(2,this->n-1,1)) / static_cast<T>(2*this->n*fma(2,this->n,1));

		this->n+=1;
		return this->current_state;
	}

};

#endif