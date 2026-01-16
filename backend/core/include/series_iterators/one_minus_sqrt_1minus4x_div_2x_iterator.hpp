#ifndef ONE_MINUS_SQRT_1MINUS4X_DIV_2X_ITERATOR_HPP
#define ONE_MINUS_SQRT_1MINUS4X_DIV_2X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file one_minus_sqrt_1minus4x_div_2x_iterator.hpp
 * @brief Iterator for the series expansion of (1 - sqrt(1 - 4x)) / (2x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = (1 - sqrt(1 - 4x)) / (2x).
 *
 * This class implements the Maclaurin expansion of the generating function for
 * Catalan numbers, which converges for values of x such that |x| <= 0.25 (excluding x=0).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_minus_sqrt_1minus4x_div_2x_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for one_minus_sqrt_1minus4x_div_2x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit one_minus_sqrt_1minus4x_div_2x_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series ((1 - sqrt(1 - 4x)) / (2x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the generating function at x.
     */
	T get_sum() const override{ return (utils::cast<T>(1) - utils::sqrt(utils::cast<T>(1) - utils::cast<T>(4) * this->x)) / (utils::cast<T>(2) * this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 0.25, x is zero, or x is non-finite; false otherwise.
     */
	bool is_invalid() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(0.25) || this->x == utils::cast<T>(0);
	}

    /**
     * @brief Computes the next term in the Catalan generating function expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (Catalan_n * x^n).
     */
	T next(K n, T& state) const override {

		// Catalan numbers generating function terms: C_n * x^n
		if (n == 0) state += utils::cast<T>(1);
		else if (n == 1) state = this->x;
		else state *= this->x * utils::cast<T>(2*utils::fma(static_cast<size_t>(2),static_cast<size_t>(n-1),static_cast<size_t>(1))) / utils::cast<T>(n+1);

		return state;
	}

};

}} //namespace shanks
#endif
