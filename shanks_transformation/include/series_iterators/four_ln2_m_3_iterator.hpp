#ifndef FOUR_LN2_M_3_ITERATOR_HPP
#define FOUR_LN2_M_3_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of x*(4*ln(2)-3) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class four_ln2_m_3_iterator final : public series_base_iterator<T, K>{
public:

	four_ln2_m_3_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return this->x * (utils::cast<T>(4) * utils::log(utils::cast<T>(2)) - utils::cast<T>(3));}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {
		
		this->current_state = utils::minus_one_raised_to_power_n<T,K>(this->n+1) * this->x / utils::cast<T>((this->n+1)*(this->n+1)*(this->n+2)*(this->n+2));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif