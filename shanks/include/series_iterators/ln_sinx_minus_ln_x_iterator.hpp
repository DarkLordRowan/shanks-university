#ifndef LN_SINX_MINUS_LN_X_ITERATOR_HPP
#define LN_SINX_MINUS_LN_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file ln_sinx_minus_ln_x_iterator.hpp
 * @brief Iterator for the series expansion of ln(sin(x)) - ln(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = ln(sin(x)) - ln(x).
 *
 * This class implements an expansion for ln(sin(x)/x), which resolves the
 * singularity at x=0. It is typically valid for x within the range [0, pi).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ln_sinx_minus_ln_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ln_sinx_minus_ln_x_iterator.
     * @authors Bolshakov M.P.
     */
	ln_sinx_minus_ln_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (ln(sin(x)) - ln(x)).
     * @authors Bolshakov M.P.
     * @return T The value of ln(sin(x)/x).
     */
	T sum() const override{ return utils::log(utils::sin(this->x)) - utils::log(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is outside [0, pi) or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() > utils::cast<float_type>(std::numbers::pi).real() ||  this->x.real() < utils::cast<float_type>(0).real();
		} else {
			return !utils::isfinite(this->x) || this->x > utils::cast<T>(std::numbers::pi) ||  this->x < utils::cast<T>(0);
		}

	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Infinite product based expansion term for ln(sin(x)/x)
		this->current_state = utils::log(utils::cast<T>(1) - this->x * this->x  /
		(utils::cast<T>((this->n+1)*(this->n+1)) * utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi)));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
