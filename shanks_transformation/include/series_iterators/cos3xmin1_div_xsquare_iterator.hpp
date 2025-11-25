#ifndef COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP
#define COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of cos(x^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class cos3xmin1_div_xsquare_iterator final : public series_base_iterator<T, K>{
public:

	cos3xmin1_div_xsquare_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::cos;
		return cos(static_cast<T>(3)*this->x-static_cast<T>(1)) / (this->x * this->x);
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || this->x == static_cast<T>(0);
		} else {
			return !isfinite(this->x) || this->x == static_cast<T>(0);
		}

	}

	T next() override {
		if (this->n == 0) this->current_state = static_cast<T>(1) / (this->x * this->x);
		else{
			const T var_3x_1squared = fma(static_cast<T>(3),this->x,static_cast<T>(-1)) * fma(static_cast<T>(3),this->x,static_cast<T>(-1));
			this->current_state *= static_cast<T>(-1) * var_3x_1squared / static_cast<T>(2*this->n*fma(2,this->n-1,1));
		}

		this->n+=1;
		return this->current_state;
	}

};

#endif