#ifndef SQRT_1_PLUSX_ITERATOR_HPP
#define SQRT_1_PLUSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file sqrt_1plusx_iterator.hpp
 * @brief Iterator for the Taylor series expansion of sqrt(1+x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the square root function f(x) = sqrt(1 + x).
 *
 * This class implements the generalized binomial expansion for sqrt(1+x),
 * which converges for values of x such that |x| <= 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for sqrt_1plusx_iterator.
     * @authors Bolshakov M.P.
     */
	sqrt_1plusx_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (sqrt(1+x)).
     * @authors Bolshakov M.P.
     * @return T The value of sqrt(1 + x).
     */
	T sum() const override{ return utils::sqrt(utils::cast<T>(1) + this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the binomial expansion of sqrt(1+x).
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation for binomial expansion with alpha = 0.5
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= utils::cast<T>(-1) * this->x * (utils::cast<T>(2*this->n)-utils::cast<T>(3)) / utils::cast<T>(2*this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
