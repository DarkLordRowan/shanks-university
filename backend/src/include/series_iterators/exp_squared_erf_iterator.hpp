#ifndef EXP_SQUARED_ERF_ITERATOR_HPP
#define EXP_SQUARED_ERF_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"
#include <numbers>

/**
 * @file exp_squared_erf_iterator.hpp
 * @brief Iterator for the Taylor series expansion of exp(x^2)*erf(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = exp(x^2) * erf(x).
 *
 * This class implements the power series expansion of the product of the
 * Gaussian-like exponential and the error function.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class exp_squared_erf_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for exp_squared_erf_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit exp_squared_erf_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (exp(x^2) * erf(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the product exp(x^2) * erf(x).
     */
	T get_sum() const override{ return utils::erf(this->x) * utils::exp(this->x * this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the exp(x^2) * erf(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Recurrence relation for the power series expansion of exp(x^2)*erf(x)
		if (n == 0) state = utils::cast<T>(2) * this->x / utils::sqrt(utils::cast<T>(std::numbers::pi));
		else state *= utils::cast<T>(2) * this->x * this->x / utils::cast<T>(utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(1)));
		return state;
	}

};

}} //namespace shanks
#endif
