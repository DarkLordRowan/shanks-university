#ifndef LN_COSX_ITERATOR_HPP
#define LN_COSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file ln_cosx_iterator.hpp
 * @brief Iterator for the series expansion of ln(cos(x)).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the natural logarithm of the cosine function ln(cos(x)).
 *
 * This class implements a specific series expansion for ln(cos(x)), which converges
 * for values of x such that |x| < pi/2.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln_cosx_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ln_cosx_iterator.
     * @authors Bolshakov M.P.
     */
	ln_cosx_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (ln(cos(x))).
     * @authors Bolshakov M.P.
     * @return T The value of ln(cos(x)).
     */
	T sum() const override{ return utils::log(utils::cos(this->x));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= pi/2 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi * 0.5);
	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence logic for the expansion terms involving sin(x)^2
		if (this->n == 0) this->current_state = utils::sin(this->x) * utils::sin(this->x) * utils::cast<T>(-0.5);
		else this->current_state *= utils::sin(this->x) * utils::sin(this->x) * utils::cast<T>(this->n) / utils::cast<T>(this->n+1);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
