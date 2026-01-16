#ifndef COS_X2_ITERATOR_HPP
#define COS_X2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file cos_x2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of cos(x^2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = cos(x^2).
 *
 * This class implements the Maclaurin expansion of cos(x^2), which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class cos_x2_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for cos_x2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit cos_x2_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (cos(x^2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of cos(x^2).
     */
	T get_sum() const override{ return utils::cos(this->x * this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the cos(x^2) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Recurrence relation for cos(x^2) terms derived from the cos Taylor series
		if (n == 0) state = utils::cast<T>(1, utils::get_precision(state));
		else state *= utils::cast<T>(-1) * utils::pow(this->x, utils::cast<T>(4)) /
		utils::cast<T>(n * (size_t{4} * n - size_t{2}));
		return state;
	}

};

}} //namespace shanks
#endif
