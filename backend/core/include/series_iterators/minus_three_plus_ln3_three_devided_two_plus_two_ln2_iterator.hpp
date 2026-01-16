#ifndef MINUS_THREE_PLUS_LN3_THREE_DEVIDED_TWO_PLUS_TWO_LN2_ITERATOR_HPP
#define MINUS_THREE_PLUS_LN3_THREE_DEVIDED_TWO_PLUS_TWO_LN2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator.hpp
 * @brief Iterator for the series expansion related to -3 + ln(3)/3 + 2*ln(2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Series iterator for the function f(x) = x * (ln(4) + 1.5*ln(3) - 3).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (ln(4) + 1.5*ln(3) - 3).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The analytic sum.
     */
	T get_sum() const override{ return this->x * (utils::log(utils::cast<T>(4)) + utils::cast<T>(1.5) * utils::log(utils::cast<T>(3)) - utils::cast<T>(3));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Specific term formula for the expansion: x / ((n+1) * (36*(n+1)^2 - 1))
		state = this->x / utils::cast<T>((n+1) * (36 * (n+1)*(n+1) - 1));
		return state;
	}

};

}} //namespace shanks
#endif
