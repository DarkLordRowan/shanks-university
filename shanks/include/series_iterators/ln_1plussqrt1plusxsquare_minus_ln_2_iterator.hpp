#ifndef LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#define LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file ln_1plussqrt1plusxsquare_minus_ln_2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of ln(1 + sqrt(1 + x^2)) - ln(2).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = ln(1 + sqrt(1 + x^2)) - ln(2).
 *
 * This class implements the Maclaurin expansion of the given composite logarithmic
 * function, which converges for values of x such that |x| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln_1plussqrt1plusxsquare_minus_ln_2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ln_1plussqrt1plusxsquare_minus_ln_2_iterator.
     * @authors Bolshakov M.P.
     */
	ln_1plussqrt1plusxsquare_minus_ln_2_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (ln(1 + sqrt(1 + x^2)) - ln(2)).
     * @authors Bolshakov M.P.
     * @return T The value of the function at the current point x.
     */
	T sum() const override{ return utils::log(utils::cast<T>(1)+utils::sqrt(utils::cast<T>(1) + this->x * this->x)) - utils::log(utils::cast<T>(2)); }

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
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation for the Taylor expansion of ln(1 + sqrt(1 + x^2)) - ln(2)
		if (this->n == 0) this->current_state = this->x * this->x * utils::cast<T>(0.25);
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x *
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * this->n) / utils::cast<T>(2 * (this->n+1)*(this->n+1));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
