#ifndef X_TWO_ITERATOR_HPP
#define X_TWO_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file x_two_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x/2.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled linear function f(x) = x / 2.
 *
 * This class implements a specific series expansion whose analytic sum is x / 2.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class x_two_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for x_two_iterator.
     * @authors Bolshakov M.P.
     */
	x_two_iterator() : series_base_iterator<T, K>() {}

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
     * @return T The next term of the series (x / ((2n+1)(2n+3))).
     */
	T next() override {

		// Formula for the n-th term of the expansion: x / ((2n+1)(2n+3))
		this->current_state = this->x /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3}));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
