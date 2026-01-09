#ifndef TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP
#define TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file two_arcsin_square_x_halfed_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 2 * asin(x/2)^2.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = 2 * asin(x/2)^2.
 *
 * This class implements the Maclaurin expansion of the squared inverse sine
 * function, which converges for values of x such that |x| <= 2.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class two_arcsin_square_x_halfed_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for two_arcsin_square_x_halfed_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	two_arcsin_square_x_halfed_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (2 * asin(x/2)^2).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the function at point x.
     */
	T get_sum() const override{ return utils::cast<T>(2) * utils::asin(this->x * utils::cast<T>(0.5)) * utils::asin(this->x * utils::cast<T>(0.5));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 2 or non-finite, false otherwise.
     */
	bool is_invalid() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(2.0);
	}

    /**
     * @brief Computes the next term in the 2 * asin(x/2)^2 Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Recurrence relation for the squared arcsin expansion terms
		if (n == 0) state = this->x * this->x * utils::cast<T>(0.5);
		else state *= this->x * this->x * utils::cast<T>(n * n) /
		utils::cast<T>(utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(1)) * utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(2)));
		return state;
	}

};

}} //namespace shanks
#endif
