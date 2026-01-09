#ifndef LN1MX_ITERATOR_HPP
#define LN1MX_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln1mx_iterator.hpp
 * @brief Iterator for the Taylor series expansion of -ln(1-x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = -ln(1 - x).
 *
 * This class implements the Maclaurin expansion of -ln(1 - x), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln1mx_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for ln1mx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	ln1mx_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (-ln(1 - x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of -ln(1 - x).
     */
	T get_sum() const override{ return utils::cast<T>(-1) * utils::log(utils::cast<T>(1) - this->x);}

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
     * @brief Computes the next term in the -ln(1-x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Standard log expansion: Sum x^n / n for n >= 1
		if (n == 0) state = this->x;
		else state *= this->x  * utils::cast<T>(n) / utils::cast<T>(n + 1);
		return state;
	}

};

}} //namespace shanks
#endif
