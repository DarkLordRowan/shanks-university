#ifndef LN2_ITERATOR_HPP
#define LN2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file ln2_iterator.hpp
 * @brief Iterator for the series expansion related to x * ln(2).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x * ln(2).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * ln(2).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ln2_iterator.
     * @authors Bolshakov M.P.
     */
	ln2_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * ln(2)).
     * @authors Bolshakov M.P.
     * @return T The value of x * ln(2).
     */
	T sum() const override{ return this->x * utils::log(utils::cast<T>(2));}

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

		// Recurrence relation for the specific series whose sum is ln(2) * x
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1)  * utils::cast<T>(this->n) / utils::cast<T>(this->n + 1);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
