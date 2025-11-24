#ifndef EXP_M_COS_X_SINSINX_ITERATOR_HPP
#define EXP_M_COS_X_SINSINX_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of e^(-cos(x))*sin(sin(x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class exp_m_cos_x_sinsin_x_iterator final : public series_base_iterator<T, K>{
public:

	exp_m_cos_x_sinsin_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{
		using std::cos;
        using std::sin;
        using std::exp;

		return exp(static_cast<T>(-1)*cos(this->x))*sin(sin(this->x));
	}
	
	bool check_validity() const override {

		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(this->x.real()) || !isfinite(this->x.imag());
		} else {
			return !isfinite(this->x);
		}

	}

	T next() override {
		using std::sin;
		
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * 
		sin(static_cast<T>(this->n + 1) * this->x) / static_cast<T>(utils::fact<K>(this->n + 1));
		this->n+=1;
		return this->current_state;
	}

};

#endif