#ifndef PI_SQUARED_6_MINUS_ONE_ITERATOR_HPP
#define PI_SQUARED_6_MINUS_ONE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file pi_squared_6_minus_one_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * (pi^2 / 6 - 1).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * (pi^2 / 6 - 1).
 * 
 * This class implements a specific series expansion related to the Basel problem 
 * (zeta(2)) whose sum is x * (pi^2 / 6 - 1).
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_squared_6_minus_one_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_squared_6_minus_one_iterator.
     * @authors Bolshakov M.P.
     */
	pi_squared_6_minus_one_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of x * (pi^2 / 6 - 1).
     */
	T sum() const override{ return this->x * (utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi)/utils::cast<T>(6) - utils::cast<T>(1));}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series (x / ((n+1)^2 * (n+2))).
     */
	T next() override {
		
		// Term formula derived from the expansion of (zeta(2) - 1) * x
		this->current_state = this->x / utils::cast<T>((this->n+1) * (this->n+1) * (this->n+2));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif