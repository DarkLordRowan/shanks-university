#ifndef INVERSE_SQRT_1M4X_ITERATOR_HPP
#define INVERSE_SQRT_1M4X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of 1/sqrt(1-4x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class inverse_sqrt_1m4x_iterator final : public series_base_iterator<T, K>{
public:

	inverse_sqrt_1m4x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(1) / utils::sqrt(utils::cast<T>(1) - utils::cast<T>(4) * this->x);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(0.25);
	}

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= this->x * utils::cast<T>(2 * utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif