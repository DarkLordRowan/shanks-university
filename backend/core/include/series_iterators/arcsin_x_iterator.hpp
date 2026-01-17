#ifndef ARCSIN_X_ITERATOR_HPP
#define ARCSIN_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"


/**
 * @file arcsin_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of arcsin(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the inverse sine function arcsin(x).
 *
 * This class implements the Maclaurin expansion of arcsin(x), which converges
 * for values of x such that |x| <= 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class arcsin_x_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for arcsin_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit arcsin_x_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (arcsin(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arcsin(x).
     */
	T get_sum() const override{ return utils::asin(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
	bool is_invalid() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the arcsin(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Standard Taylor expansion recursion: each term is derived from the previous one
		if (n == 0) state = this->x;
		else state *= this->x * this->x *
		utils::cast<T>(utils::fma(static_cast<size_t>(2),static_cast<size_t>(n-1),static_cast<size_t>(1)) * utils::fma(static_cast<size_t>(2),static_cast<size_t>(n-1),static_cast<size_t>(1))) /
		utils::cast<T>(2 * n * utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(1)));
		return state;
	}

};

}} //namespace shanks
#endif
