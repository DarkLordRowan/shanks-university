#ifndef INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#define INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file incomplete_Gamma_func_iterator.hpp
 * @brief Iterator for the series expansion of the incomplete Gamma function gamma(alpha, x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Series iterator for the lower incomplete Gamma function gamma(alpha, x).
 *
 * This class implements the power series expansion for the lower incomplete
 * Gamma function, which converges for all finite x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class incomplete_Gamma_func_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for incomplete_Gamma_func_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	incomplete_Gamma_func_iterator(T x, T alpha) : series_base_succ<T, K>(x), alpha(alpha) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

	T alpha; /**< The shape parameter alpha of the incomplete Gamma function. */

    /**
     * @brief Retrieves the analytic sum of the series (gamma(alpha, x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the lower incomplete Gamma function.
     */
	T get_sum() const override{ return utils::inc_gamma(this->x, alpha);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the incomplete Gamma function expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Recurrence relation for the series terms of gamma(alpha, x)
		if (n == 0) state = utils::pow(this->x, alpha) / alpha;
		else state *= utils::cast<T>(-1) * this->x * (alpha + utils::cast<T>(n-1)) /
		((alpha + utils::cast<T>(n)) * utils::cast<T>(n));

		return state;
	}

};

}} //namespace shanks
#endif
