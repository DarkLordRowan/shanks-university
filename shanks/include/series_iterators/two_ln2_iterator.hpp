#ifndef TWO_LN2_ITERATOR_HPP
#define TWO_LN2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file two_ln2_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * 2 * ln(2).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x * 2 * ln(2).
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x * 2 * ln(2) (or x * ln(4)).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class two_ln2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for two_ln2_iterator.
     * @authors Bolshakov M.P.
     */
	two_ln2_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of x * 2 * ln(2).
     */
	T sum() const override{ return this->x * utils::log(utils::cast<T>(4));}

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

		const K n1 = this->n + 1;
		// Formula for the n-th term of the specific series expansion
		this->current_state = utils::cast<T>(12 * n1 * n1 - 1) * this->x / utils::cast<T>(n1*(4*n1*n1-1)*(4*n1*n1-1));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
