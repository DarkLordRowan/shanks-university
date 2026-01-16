#ifndef LAMBERT_W_FUNC_ITERATOR_HPP
#define LAMBERT_W_FUNC_ITERATOR_HPP

#pragma once

#include "../series_base.hpp"
#include <numbers>

/**
 * @file lambert_W_func_iterator.hpp
 * @brief Iterator for the series expansion of the principal branch of the Lambert W function W0(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the principal branch of the Lambert W function, W0(x).
 *
 * This class implements the Maclaurin expansion of W0(x), which converges for
 * values of x such that |x| < 1/e.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class lambert_W_func_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for lambert_W_func_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit lambert_W_func_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (W0(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the principal branch of the Lambert W function.
     */
	T get_sum() const override{ return utils::lambertW0(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 1/e or non-finite, false otherwise.
     */
	bool is_invalid() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1) / utils::cast<float_type>(std::numbers::e);
	}

    /**
     * @brief Computes the next term in the Lambert W0(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Implementation of the power series for W0(x)
		if (n == 0) state = this->x;
		else state *= utils::cast<T>(-1) * this->x / utils::cast<T>(n);
		return state;
	}

};

}} //namespace shanks
#endif
