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
	
	T sum() const override{
		using std::pow;

        return static_cast<T>(utils::fact<K>(m)) / pow(static_cast<T>(1) - this->x, static_cast<T>(m+1));
	}
	
	bool check_validity() const override {

		using std::isfinite;
		using std::abs;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag()) || abs(this->x) >= static_cast<T>(1).real();
		} else {
			return !isfinite(this->x) || abs(this->x) >= static_cast<T>(1);
		}

	}

	T next() override {

		if (this->n == 0) this->current_state = static_cast<T>(utils::fact<K>(m));
		else this->current_state *= this->x * static_cast<T>(m + this->n) / static_cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

#endif