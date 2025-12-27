#ifndef MINUS_ONE_N_FACT_IN_N_ITERATOR_HPP
#define MINUS_ONE_N_FACT_IN_N_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of -x*0.65583160 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class minus_one_n_fact_n_in_n_iterator final : public series_base_iterator<T, K>{
public:

	minus_one_n_fact_n_in_n_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * utils::cast<T>(-0.65583160);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n + 1) * 
		utils::cast<T>(utils::fact<K>(this->n+1)) * this->x / utils::cast<T>(utils::pow(this->n+1, this->n+1));
		this->n+=1;
		return this->current_state;
	}

};

#endif