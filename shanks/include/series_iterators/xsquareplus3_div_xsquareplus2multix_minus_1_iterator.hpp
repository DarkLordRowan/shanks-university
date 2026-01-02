#ifndef XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP
#define XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file xsquareplus3_div_xsquareplus2multix_minus_1_iterator.hpp
 * @brief Iterator for the series expansion of (x^2 + 3) / (x^2 + 2x) - 1.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the rational function f(x) = (x^2 + 3) / (x^2 + 2x) - 1.
 *
 * This class implements a specific series expansion for the given rational function,
 * typically expanded around x=1, which converges for values of x such that |x - 1| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class xsquareplus3_div_xsquareplus2multix_minus_1_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for xsquareplus3_div_xsquareplus2multix_minus_1_iterator.
     * @authors Bolshakov M.P.
     */
	xsquareplus3_div_xsquareplus2multix_minus_1_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of the rational function at current point x.
     */
	T sum() const override{ return (this->x * this->x + utils::cast<T>(3)) / (this->x * this->x + utils::cast<T>(2) * this->x) - utils::cast<T>(1);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x - 1| >= 1 or x is non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x - utils::cast<T>(1)) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Specific term formula for the expansion of the rational function around x=1
		this->current_state = utils::cast<T>(0.5) * utils::minus_one_raised_to_power_n<T, K>(this->n) *
		utils::cast<T>(utils::pow(size_t{3}, this->n+2) - 7) *
		utils::pow(this->x - utils::cast<T>(1), utils::cast<T>(this->n)) / utils::cast<T>(utils::pow(size_t{3}, this->n+1));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
