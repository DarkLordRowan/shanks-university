#ifndef MEAN_SINH_SIN_ITERATOR_HPP
#define MEAN_SINH_SIN_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file mean_sinh_sin_iterator.hpp
 * @brief Iterator for the series expansion of 0.5 * (sinh(x) + sin(x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = 0.5 * (sinh(x) + sin(x)).
 *
 * This class implements the Maclaurin expansion of the mean of hyperbolic and
 * standard sine functions, which converges for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class mean_sinh_sin_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for mean_sinh_sin_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	mean_sinh_sin_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (0.5 * (sinh(x) + sin(x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 0.5 * (sinh(x) + sin(x)).
     */
	T get_sum() const override{ return (utils::sinh(this->x) + utils::sin(this->x)) * utils::cast<T>(0.5);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the mean(sinh, sin) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Recurrence relation for the combined expansion terms
		if (n == 0) state = this->x;
		else state *= utils::pow(this->x, utils::cast<T>(4)) /
		utils::cast<T>(
			utils::fma(static_cast<size_t>(4),static_cast<size_t>(n-1),static_cast<size_t>(5))*utils::fma(static_cast<size_t>(4),static_cast<size_t>(n-1),static_cast<size_t>(4)) *
			utils::fma(static_cast<size_t>(4),static_cast<size_t>(n-1),static_cast<size_t>(3))*utils::fma(static_cast<size_t>(4),static_cast<size_t>(n-1),static_cast<size_t>(2))
		);
		return state;
	}

};

}} //namespace shanks
#endif
