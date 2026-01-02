#ifndef HALF_ASIN_TWO_X_ITERATOR_HPP
#define HALF_ASIN_TWO_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file half_asin_two_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 0.5 * asin(2x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = 0.5 * asin(2x).
 *
 * This class implements the Maclaurin expansion of 0.5 * asin(2x), which converges
 * for values of x such that |x| <= 0.5.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class half_asin_two_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for half_asin_two_x_iterator.
     * @authors Bolshakov M.P.
     */
	half_asin_two_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (0.5 * asin(2x)).
     * @authors Bolshakov M.P.
     * @return T The value of 0.5 * asin(2x).
     */
	T sum() const override{ return utils::cast<T>(0.5) * utils::asin(utils::cast<T>(2) * this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > 0.5 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(0.5);
	}

    /**
     * @brief Computes the next term in the 0.5 * asin(2x) Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation derived from the arcsin expansion with substitution x -> 2x and scaling factor 0.5
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x * utils::cast<T>(2 * this->n * utils::fma(size_t{2},this->n-1,size_t{1}) *
		utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(this->n * this->n * utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
