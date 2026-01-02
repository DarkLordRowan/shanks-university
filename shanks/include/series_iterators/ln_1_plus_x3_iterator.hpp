#ifndef LN_1_PLUS_X3_ITERATOR_HPP
#define LN_1_PLUS_X3_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file ln_1_plus_x3_iterator.hpp
 * @brief Iterator for the Taylor series expansion of ln(1 + x^3).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = ln(1 + x^3).
 *
 * This class implements the Maclaurin expansion of ln(1 + x^3), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln_1_plus_x3_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ln_1_plus_x3_iterator.
     * @authors Bolshakov M.P.
     */
	ln_1_plus_x3_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (ln(1 + x^3)).
     * @authors Bolshakov M.P.
     * @return T The value of ln(1 + x^3).
     */
	T sum() const override{ return utils::log(utils::cast<T>(1) + utils::pow(this->x, utils::cast<T>(3))); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the ln(1+x^3) Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation based on the ln(1+u) expansion where u = x^3
		if (this->n == 0) this->current_state = utils::pow(this->x, utils::cast<T>(3.0));
		else this->current_state *= utils::cast<T>(-1.0) * utils::pow(this->x, utils::cast<T>(3.0)) *
		utils::cast<T>(static_cast<double>(this->n)) / utils::cast<T>(static_cast<double>(this->n + 1));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
