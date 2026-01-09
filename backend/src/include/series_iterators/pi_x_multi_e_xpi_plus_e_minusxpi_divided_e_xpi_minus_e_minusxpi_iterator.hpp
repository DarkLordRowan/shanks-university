#ifndef PI_X_MULTI_E_XPI_PLUS_E_MINUSXPI_DIVIDED_E_XPI_MINUS_E_MINUSXPI_ITERATOR_HPP
#define PI_X_MULTI_E_XPI_PLUS_E_MINUSXPI_DIVIDED_E_XPI_MINUS_E_MINUSXPI_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator.hpp
 * @brief Iterator for the partial fraction expansion of pi*x*coth(pi*x) - 1.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Series iterator for the function f(x) = pi*x*coth(pi*x) - 1.
 *
 * This class implements the partial fraction expansion of the hyperbolic cotangent
 * function, shifted by -1. This expansion is related to the values of the Riemann
 * zeta function at even integers.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (pi*x*coth(pi*x) - 1).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of pi*x*coth(pi*x) - 1.
     */
	T get_sum() const override{
		const T pi = utils::cast<T>(std::numbers::pi);
		return pi * this->x * (utils::exp(pi*this->x)+utils::exp(-pi*this->x)) / (utils::exp(pi*this->x) - utils::exp(-pi*this->x)) - utils::cast<T>(1);
	}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the partial fraction expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (2*x^2 / (x^2 + (n+1)^2)).
     */
	T next(K n, T& state) const override {

		// Formula for the terms of the coth partial fraction expansion
		state = utils::cast<T>(2) * this->x * this->x / (this->x * this->x + utils::cast<T>((n+1) * (n+1)));
		return state;
	}

};

}} //namespace shanks
#endif
