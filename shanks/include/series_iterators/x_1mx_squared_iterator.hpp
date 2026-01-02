#ifndef X_1MX_SQUARED_ITERATOR_HPP
#define X_1MX_SQUARED_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file x_1mx_squared_iterator.hpp
 * @brief Iterator for the series expansion of x / (1 - x)^2.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the rational function f(x) = x / (1 - x)^2.
 *
 * This class implements the Maclaurin expansion of the derivative of the
 * geometric series, which converges for values of x such that |x| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class x_1mx_squared_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for x_1mx_squared_iterator.
     * @authors Bolshakov M.P.
     */
	x_1mx_squared_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x / (1 - x)^2).
     * @authors Bolshakov M.P.
     * @return T The value of x / (1 - x)^2.
     */
	T sum() const override{ return this->x / ((utils::cast<T>(1) - this->x) * (utils::cast<T>(1) - this->x));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the power series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series ( (n+1) * x^(n+1) ).
     */
	T next() override {

		// Recurrence relation: each term is (n+1) * x^(n+1)
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * utils::cast<T>(this->n + 1) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
