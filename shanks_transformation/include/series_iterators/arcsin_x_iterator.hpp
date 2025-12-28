#ifndef ARCSIN_X_ITERATOR_HPP
#define ARCSIN_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of arcsin(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class arcsin_x_iterator final : public series_base_iterator<T, K>{
public:

	arcsin_x_iterator() : series_base_iterator<T, K>() {}
	T sum() const override{ return utils::asin(this->x);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x * 
		utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1}) * utils::fma(size_t{2},this->n-1,size_t{1})) / 
		utils::cast<T>(2 * this->n * utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif