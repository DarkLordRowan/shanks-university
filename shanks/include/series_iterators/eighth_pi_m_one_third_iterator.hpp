#ifndef EIGHTH_PI_M_ONE_THIRD_ITERATOR_HPP
#define EIGHTH_PI_M_ONE_THIRD_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file eighth_pi_m_one_third_iterator.hpp
 * @brief Iterator for the series expansion related to pi/8 - 1/3.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x * (pi/8 - 1/3).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (pi/8 - 1/3).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class eighth_pi_m_one_third_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for eighth_pi_m_one_third_iterator.
     * @authors Bolshakov M.P.
     */
	eighth_pi_m_one_third_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * (pi/8 - 1/3)).
     * @authors Bolshakov M.P.
     * @return T The value of x * (pi/8 - 1/3).
     */
	T sum() const override{ return this->x * (utils::cast<T>(std::numbers::pi*0.125) - utils::cast<T>(1)/utils::cast<T>(3)); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term of the specific series expansion
		this->current_state = utils::minus_one_raised_to_power_n<T,K>(this->n) * this->x /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3}) * utils::fma(size_t{2},this->n,size_t{5}));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
