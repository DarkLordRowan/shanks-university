#ifndef X_1MX_SQUARED_ITERATOR_HPP
#define X_1MX_SQUARED_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file x_1mx_squared_iterator.hpp
 * @brief Iterator for the series expansion of x / (1 - x)^2.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the rational function f(x) = x / (1 - x)^2.
 *
 * This class implements the Maclaurin expansion of the derivative of the
 * geometric series, which converges for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class x_1mx_squared_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for x_1mx_squared_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit x_1mx_squared_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (x / (1 - x)^2).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x / (1 - x)^2.
     */
	T get_sum() const override{ return this->x / ((utils::cast<T>(1) - this->x) * (utils::cast<T>(1) - this->x));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool is_invalid() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the power series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ( (n+1) * x^(n+1) ).
     */
	T next(K n, T& state) const override {

		// Recurrence relation: each term is (n+1) * x^(n+1)
		if (n == 0) state = this->x;
		else state *= this->x * utils::cast<T>(n + 1) / utils::cast<T>(n);
		return state;
	}

};

}} //namespace shanks
#endif
