#ifndef COS_SQRT_X_ITERATOR_HPP
#define COS_SQRT_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class cos_sqrt_x_iterator final : public series_base_iterator<T, K>{
public:

	cos_sqrt_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{

		using std::cos;
		using std::sqrt;

		return cos(sqrt(this->x));
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || this->x.real() <= static_cast<T>(0).real();
		} else {
			return !isfinite(this->x) || this->x < static_cast<T>(0);
		}

	}

	T next() override {
		if (this->n == 0) this->current_state = static_cast<T>(1);
		else this->current_state *= static_cast<T>(-1) * this->x  / static_cast<T>(2*this->n*fma(2,this->n-1,1));

		this->n+=1;
		return this->current_state;
	}

};

#endif