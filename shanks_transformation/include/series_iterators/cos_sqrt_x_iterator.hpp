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

	T sum() const override{ return utils::cos(utils::sqrt(this->x)); }
	
	bool check_validity() const override {

		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() <= utils::cast<T>(0).real();
		} else {
			return !utils::isfinite(this->x) || this->x < utils::cast<T>(0);
		}

	}

	T next() override {
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= utils::cast<T>(-1) * this->x  / utils::cast<T>(2*this->n*utils::fma(size_t{2},this->n-1,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

#endif