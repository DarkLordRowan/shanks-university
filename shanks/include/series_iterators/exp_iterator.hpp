#ifndef EXP_ITERATOR_HPP
#define EXP_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file exp_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the exponential function e^x.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the exponential function exp(x).
 *
 * This class implements the Maclaurin expansion of e^x, which converges
 * for all finite values of x.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class exp_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for exp_iterator.
     * @authors Bolshakov M.P.
     */
	exp_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (exp(x)).
     * @authors Bolshakov M.P.
     * @return T The value of exp(x).
     */
	T sum() const override{ return utils::exp(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the exp(x) Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Each term is x^n / n!, computed recursively from the previous term
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= this->x / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
