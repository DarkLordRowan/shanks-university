#ifndef LN_X_PLUS_ONE_X_MINUS_ONE_HALFED_ITERATOR_HPP
#define LN_X_PLUS_ONE_X_MINUS_ONE_HALFED_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln_x_plus_one_x_minus_one_halfed_iterator.hpp
 * @brief Iterator for the series expansion related to ln(cos(x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = ln(cos(x)).
 *
 * This class implements a Maclaurin expansion for ln(cos(x)).
 * Note: The class name and file name suggest a different function, but the implementation
 * currently computes ln(cos(x)).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln_x_plus_one_x_minus_one_halfed_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for ln_x_plus_one_x_minus_one_halfed_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	ln_x_plus_one_x_minus_one_halfed_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series (ln(cos(x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of ln(cos(x)).
     */
	T get_sum() const override{	return utils::log(utils::cos(this->x));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > pi/2 or non-finite, false otherwise.
     */
	bool is_invalid() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(std::numbers::pi * 0.5);
	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// Expansion terms based on sin(x)^2 for ln(cos(x))
		if (n == 0) state = utils::sin(this->x) * utils::sin(this->x) * utils::cast<T>(-0.5);
		else state *= utils::sin(this->x) * utils::sin(this->x) * utils::cast<T>(n) / utils::cast<T>(n+1);
		return state;
	}

};

}} //namespace shanks
#endif
