#ifndef LN1PX4_ITERATOR_HPP
#define LN1PX4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file ln1px4_iterator.hpp
 * @brief Iterator for the Taylor series expansion of ln(1 + x^4).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = ln(1 + x^4).
 *
 * This class implements the Maclaurin expansion of ln(1 + x^4), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln1px4_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ln1px4_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	ln1px4_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (ln(1 + x^4)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of ln(1 + x^4).
     */
	T sum() const override{ return utils::log(utils::cast<T>(1) + utils::pow(this->x, utils::cast<T>(4)));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the ln(1+x^4) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation based on the ln(1+u) expansion with u = x^4
		if (this->n == 0) this->current_state = utils::pow(this->x, utils::cast<T>(4));
		else this->current_state *= utils::cast<T>(-1) *  utils::pow(this->x, utils::cast<T>(4))  * utils::cast<T>(this->n) / utils::cast<T>(this->n + 1);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
