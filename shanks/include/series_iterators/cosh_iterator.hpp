#ifndef COSH_ITERATOR_HPP
#define COSH_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file cosh_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the hyperbolic cosine cosh(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the hyperbolic cosine function cosh(x).
 *
 * This class implements the Maclaurin expansion of cosh(x), which converges
 * for all finite values of x.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class cosh_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for cosh_iterator.
     * @authors Bolshakov M.P.
     */
	cosh_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (cosh(x)).
     * @authors Bolshakov M.P.
     * @return T The value of cosh(x).
     */
	T sum() const override{ return utils::cosh(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the cosh(x) Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence logic for the even-power terms of the cosh expansion
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= this->x * this->x / utils::cast<T>(this->n * (size_t{4} * this->n - size_t{2}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
