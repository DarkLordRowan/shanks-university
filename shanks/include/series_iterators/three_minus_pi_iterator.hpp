#ifndef THREE_MINUS_PI_ITERATOR_HPP
#define THREE_MINUS_PI_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file three_minus_pi_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * (3 - pi).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * (3 - pi).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (3 - pi).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class three_minus_pi_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for three_minus_pi_iterator.
     * @authors Bolshakov M.P.
     */
	three_minus_pi_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of x * (3 - pi).
     */
	T sum() const override{ return this->x * (utils::cast<T>(3) - utils::cast<T>(std::numbers::pi));}

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

		// Alternating term formula: (-1)^(n+1) * x / ((n+1)*(n+2)*(2n+3))
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n+1) * this->x /
		utils::cast<T>((this->n + 1)*(this->n+2)*utils::fma(size_t{2},this->n,size_t{3}));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
