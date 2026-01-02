#ifndef TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP
#define TWO_ARCSIN_SQUARE_X_HALFED_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file two_arcsin_square_x_halfed_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 2 * asin(x/2)^2.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = 2 * asin(x/2)^2.
 *
 * This class implements the Maclaurin expansion of the squared inverse sine
 * function, which converges for values of x such that |x| <= 2.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class two_arcsin_square_x_halfed_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for two_arcsin_square_x_halfed_iterator.
     * @authors Bolshakov M.P.
     */
	two_arcsin_square_x_halfed_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (2 * asin(x/2)^2).
     * @authors Bolshakov M.P.
     * @return T The value of the function at point x.
     */
	T sum() const override{ return utils::cast<T>(2) * utils::asin(this->x * utils::cast<T>(0.5)) * utils::asin(this->x * utils::cast<T>(0.5));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > 2 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(2.0);
	}

    /**
     * @brief Computes the next term in the 2 * asin(x/2)^2 Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation for the squared arcsin expansion terms
		if (this->n == 0) this->current_state = this->x * this->x * utils::cast<T>(0.5);
		else this->current_state *= this->x * this->x * utils::cast<T>(this->n * this->n) /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{2}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
