#ifndef EXP_ITERATOR_HPP
#define EXP_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file exp_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the exponential function e^x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the exponential function exp(x).
 *
 * This class implements the Maclaurin expansion of e^x, which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class exp_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for exp_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	exp_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (exp(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of exp(x).
     */
	T get_sum() const override{ return utils::exp(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the exp(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Each term is x^n / n!, computed recursively from the previous term
		if (n == 0) state = utils::cast<T>(1);
		else state *= this->x / utils::cast<T>(n);
		return state;
	}

};

}} //namespace shanks
#endif
