#ifndef M_FACT_1MX_MP1_INVERSE_ITERATOR_HPP
#define M_FACT_1MX_MP1_INVERSE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x*log(13/7)/7 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class m_fact_1mx_mp1_inverse_iterator final : public series_base_iterator<T, K>{
public:

	K m = static_cast<K>(0);

	m_fact_1mx_mp1_inverse_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return utils::cast<T>(utils::fact<K>(m)) / utils::pow(utils::cast<T>(1) - this->x, utils::cast<T>(m+1));}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

	T next() override {

		if (this->n == 0) this->current_state = utils::cast<T>(utils::fact<K>(m));
		else this->current_state *= this->x * utils::cast<T>(m + static_cast<K>(this->n)) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif