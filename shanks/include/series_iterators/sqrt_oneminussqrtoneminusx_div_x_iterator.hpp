#ifndef SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP
#define SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file sqrt_oneminussqrtoneminusx_div_x_iterator.hpp
 * @brief Iterator for the series expansion of sqrt((1 - sqrt(1 - x)) / x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the composite function f(x) = sqrt((1 - sqrt(1 - x)) / x).
 *
 * This class implements the Maclaurin expansion of the given nested radical function,
 * which converges for values of x such that |x| < 1 (excluding x=0).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class sqrt_oneminussqrtoneminusx_div_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for sqrt_oneminussqrtoneminusx_div_x_iterator.
     * @authors Bolshakov M.P.
     */
	sqrt_oneminussqrtoneminusx_div_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of sqrt((1 - sqrt(1 - x)) / x).
     */
	T sum() const override{ return utils::sqrt((utils::cast<T>(1) - utils::sqrt(utils::cast<T>(1) - this->x)) / this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= 1, x is zero, or x is non-finite; false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || this->x == utils::cast<T>(0) || utils::abs(this->x) >= utils::cast<float_type>(1);
	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// First term corresponds to the limit at x -> 0, subsequent terms are calculated recursively
		if (this->n == 0) this->current_state = utils::cast<T>(1) / utils::sqrt(utils::cast<T>(2));
		else this->current_state *= this->x *
		utils::cast<T>(utils::fma(size_t{4},this->n-1,size_t{1}) * utils::fma(size_t{4},this->n-1,size_t{3})) /
		utils::cast<T>(8 * this->n * utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
