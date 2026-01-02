#ifndef PI_SQUARED_TWELVE_ITERATOR_HPP
#define PI_SQUARED_TWELVE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file pi_squared_twelve_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * pi^2 / 12.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi^2 / 12.
 * 
 * This class implements an alternating series expansion related to the Dirichlet 
 * eta function eta(2) whose sum is x * pi^2 / 12.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_squared_twelve_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_squared_twelve_iterator.
     * @authors Bolshakov M.P.
     */
	pi_squared_twelve_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of x * pi^2 / 12.
     */
	T sum() const override{ return this->x * utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi)/utils::cast<T>(12);}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the alternating series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series ((-1)^n * x / (n+1)^2).
     */
	T next() override {
	
		// Alternating term with quadratic denominator
		this->current_state = this->x * utils::minus_one_raised_to_power_n<T, K>(this->n) / utils::cast<T>((this->n+1) * (this->n+1));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif