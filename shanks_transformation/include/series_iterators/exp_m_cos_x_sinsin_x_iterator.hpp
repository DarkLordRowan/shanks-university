#ifndef EXP_M_COS_X_SINSINX_ITERATOR_HPP
#define EXP_M_COS_X_SINSINX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of e^(-cos(x))*sin(sin(x)) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class exp_m_cos_x_sinsin_x_iterator final : public series_base_iterator<T, K>{
public:

	exp_m_cos_x_sinsin_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::exp(utils::cast<T>(-1)*utils::cos(this->x))*utils::sin(utils::sin(this->x));}
	
	bool check_validity() const override { return !utils::isfinite(this->x);}

	T next() override {
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * 
		utils::sin(utils::cast<T>(this->n + 1) * this->x) / utils::cast<T>(utils::fact<K>(this->n + 1));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif