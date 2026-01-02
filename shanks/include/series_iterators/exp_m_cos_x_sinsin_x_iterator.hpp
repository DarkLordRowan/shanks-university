#ifndef EXP_M_COS_X_SINSINX_ITERATOR_HPP
#define EXP_M_COS_X_SINSINX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file exp_m_cos_x_sinsin_x_iterator.hpp
 * @brief Iterator for the series expansion of exp(-cos(x))*sin(sin(x)).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the complex composite function f(x) = exp(-cos(x)) * sin(sin(x)).
 *
 * This class implements a specific series expansion for this composite
 * trigonometric/exponential function.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class exp_m_cos_x_sinsin_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for exp_m_cos_x_sinsin_x_iterator.
     * @authors Bolshakov M.P.
     */
	exp_m_cos_x_sinsin_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (exp(-cos(x)) * sin(sin(x))).
     * @authors Bolshakov M.P.
     * @return T The value of the function at the current point x.
     */
	T sum() const override{ return utils::exp(utils::cast<T>(-1)*utils::cos(this->x))*utils::sin(utils::sin(this->x));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x);}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {
		// General formula for the n-th term of the specific expansion
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) *
		utils::sin(utils::cast<T>(this->n + 1) * this->x) / utils::cast<T>(utils::fact<K>(this->n + 1));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
