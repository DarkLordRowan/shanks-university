#ifndef MEAN_SINH_SIN_ITERATOR_HPP
#define MEAN_SINH_SIN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of 0.5*(sin(x)+sinh(x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class mean_sinh_sin_iterator final : public series_base_iterator<T, K>{
public:

	mean_sinh_sin_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		using std::sinh;
        using std::sin;

		return (sinh(this->x) + sin(this->x)) * static_cast<T>(0.5);
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
		else this->current_state *= pow(this->x, static_cast<T>(4)) / 
		static_cast<T>(fma(4,this->n-1,5)*fma(4,this->n-1,4)*fma(4,this->n-1,3)*fma(4,this->n-1,2));

		this->n+=1;
		return this->current_state;
	}

};

#endif