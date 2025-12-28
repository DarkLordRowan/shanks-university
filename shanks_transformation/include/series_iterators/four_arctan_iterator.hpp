#ifndef FOUR_ARCTAN_ITERATOR_HPP
#define FOUR_ARCTAN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of 4atan(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class four_arctan_iterator final : public series_base_iterator<T, K>{
public:

	four_arctan_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cast<T>(4) * utils::atan(this->x);}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}


	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(4) * this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x * utils::cast<T>(2 * this->n - 1) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif