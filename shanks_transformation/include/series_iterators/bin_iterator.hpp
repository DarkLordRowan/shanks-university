#ifndef BIN_ITERATOR_HPP
#define BIN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of arcsin(x^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class bin_iterator final : public series_base_iterator<T, K>{
public:

	T alpha = utils::cast<T>(0.0);

	bin_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::pow(utils::cast<T>(1.0) + this->x, alpha); }
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

	T next() override {
		if (this->n == 0) this->current_state = utils::cast<T>(1.0);
		else this->current_state *= (this->alpha - utils::cast<T>(this->n - static_cast<K>(1))) * this->x / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif