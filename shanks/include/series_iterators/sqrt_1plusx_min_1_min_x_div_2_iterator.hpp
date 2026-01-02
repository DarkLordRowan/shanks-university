#ifndef SQRT_1PLUSX_MIN_1_MIN_X_DIV_2_ITERATOR_HPP
#define SQRT_1PLUSX_MIN_1_MIN_X_DIV_2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file sqrt_1plusx_min_1_min_x_div_2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of sqrt(1+x) - 1 - x/2.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = sqrt(1 + x) - 1 - x/2.
 *
 * This class implements the Maclaurin expansion of sqrt(1+x) starting from the
 * quadratic term, which converges for values of x such that |x| <= 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_min_1_min_x_div_2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for sqrt_1plusx_min_1_min_x_div_2_iterator.
     * @authors Bolshakov M.P.
     */
	sqrt_1plusx_min_1_min_x_div_2_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (sqrt(1+x) - 1 - x/2).
     * @authors Bolshakov M.P.
     * @return T The value of the truncated square root expansion at x.
     */
	T sum() const override{ return utils::sqrt(utils::cast<T>(1) + this->x) - utils::cast<T>(1) - this->x * utils::cast<T>(0.5);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		if (this->n == 0) this->current_state = this->x * this->x * utils::cast<T>(-0.125);
		else this->current_state *= utils::cast<T>(-1) * this->x *
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1})) / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{2}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
