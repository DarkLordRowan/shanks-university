#ifndef PI_4_ITERATOR_HPP
#define PI_4_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"
#include <numbers>

/**
 * @file pi_4_iterator.hpp
 * @brief Iterator for the Gregory-Leibniz-like series related to x * pi / 4.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = x * pi / 4.
 *
 * This class implements an alternating series expansion whose sum is x * pi / 4.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_4_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for pi_4_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit pi_4_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (x * pi / 4).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi / 4.
     */
	T get_sum() const override{ return this->x * utils::cast<T>(std::numbers::pi * 0.25); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the alternating series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ((-1)^n * x / (2n+1)).
     */
	T next(K n, T& state) const override {

		// Standard alternating series term for the atan(1) expansion scaled by x
		state = utils::minus_one_raised_to_power_n<T , K>(n) * this->x / utils::cast<T>(utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(1)));
		return state;
	}

};

}} //namespace shanks
#endif
