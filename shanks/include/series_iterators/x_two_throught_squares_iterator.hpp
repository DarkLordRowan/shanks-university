#ifndef X_TWO_THROUGHT_SQUARES_ITERATOR_HPP
#define X_TWO_THROUGHT_SQUARES_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file x_two_throught_squares_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x/2.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x / 2 using a specific expansion.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x / 2, with terms involving higher-order powers in the denominator.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class x_two_throught_squares_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for x_two_throught_squares_iterator.
     * @authors Bolshakov M.P.
     */
	x_two_throught_squares_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x / 2).
     * @authors Bolshakov M.P.
     * @return T The value of x / 2.
     */
	T sum() const override{ return this->x * utils::cast<T>(0.5);}

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

		// Specific term formula for this variant of the x/2 expansion
		this->current_state = utils::cast<T>(2 *(this->n + 1)*(this->n + 1) - 1) * this->x / utils::cast<T>(4 * utils::pow(this->n + 1, size_t{4}) + 1);
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
