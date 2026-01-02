#ifndef INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#define INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file incomplete_Gamma_func_iterator.hpp
 * @brief Iterator for the series expansion of the incomplete Gamma function gamma(alpha, x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the lower incomplete Gamma function gamma(alpha, x).
 *
 * This class implements the power series expansion for the lower incomplete
 * Gamma function, which converges for all finite x.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class incomplete_Gamma_func_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for incomplete_Gamma_func_iterator.
     * @authors Bolshakov M.P.
     */
	incomplete_Gamma_func_iterator() : series_base_iterator<T, K>() {}

	T alpha; /**< The shape parameter alpha of the incomplete Gamma function. */

    /**
     * @brief Retrieves the analytic sum of the series (gamma(alpha, x)).
     * @authors Bolshakov M.P.
     * @return T The value of the lower incomplete Gamma function.
     */
	T sum() const override{ return utils::inc_gamma(this->x, alpha);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the incomplete Gamma function expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation for the series terms of gamma(alpha, x)
		if (this->n == 0) this->current_state = utils::pow(this->x, alpha) / alpha;
		else this->current_state *= utils::cast<T>(-1) * this->x * (alpha + utils::cast<T>(this->n-1)) /
		((alpha + utils::cast<T>(this->n)) * utils::cast<T>(this->n));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
