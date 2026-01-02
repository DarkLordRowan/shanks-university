#ifndef LN13_MIN_LN7_DIV_7_ITERATOR_HPP
#define LN13_MIN_LN7_DIV_7_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file ln13_min_ln7_div_7_iterator.hpp
 * @brief Iterator for the series expansion related to x * (ln(13) - ln(7)) / 7.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x * ln(13/7) / 7.
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * ln(13/7) / 7.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln13_min_ln7_div_7_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ln13_min_ln7_div_7_iterator.
     * @authors Bolshakov M.P.
     */
	ln13_min_ln7_div_7_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * ln(13/7) / 7).
     * @authors Bolshakov M.P.
     * @return T The value of x * ln(13/7) / 7.
     */
	T sum() const override{ return this->x * utils::log(utils::cast<T>(13)/utils::cast<T>(7)) / utils::cast<T>(7);}

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
		// Specific term recurrence for the ln(13/7) related series
		if (this->n == 0) this->current_state = utils::cast<T>(6) * this->x / utils::cast<T>(49);
		else this->current_state *= utils::cast<T>(-1) * utils::cast<T>(6 * this->n) / utils::cast<T>(utils::fma(size_t{7},this->n,size_t{7}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
