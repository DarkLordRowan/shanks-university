#ifndef LN2_ITERATOR_HPP
#define LN2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln2_iterator.hpp
 * @brief Iterator for the series expansion related to x * ln(2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Series iterator for the function f(x) = x * ln(2).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * ln(2).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln2_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for ln2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit ln2_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (x * ln(2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * ln(2).
     */
	T get_sum() const override{ return this->x * utils::log(utils::cast<T>(2));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x);}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Recurrence relation for the specific series whose sum is ln(2) * x
		if (n == 0) state = this->x;
		else state *= utils::cast<T>(-1)  * utils::cast<T>(n) / utils::cast<T>(n + 1);
		return state;
	}

};

}} //namespace shanks
#endif
