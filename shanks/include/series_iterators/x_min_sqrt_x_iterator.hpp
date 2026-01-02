#ifndef X_MIN_SQRT_X_ITERATOR_HPP
#define X_MIN_SQRT_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file x_min_sqrt_x_iterator.hpp
 * @brief Iterator for the series expansion of x - sqrt(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x - sqrt(x).
 *
 * This class implements a specific series expansion for x - sqrt(x), typically
 * expanded around x=1, which converges for values of x such that |x - 1| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class x_min_sqrt_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for x_min_sqrt_x_iterator.
     * @authors Bolshakov M.P.
     */
	x_min_sqrt_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x - sqrt(x)).
     * @authors Bolshakov M.P.
     * @return T The value of x - sqrt(x).
     */
	T sum() const override{ return this->x - utils::sqrt(this->x); }

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
     * @brief Computes the next term in the series expansion of x - sqrt(x).
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		if (this->n == 0) this->current_state = this->x - utils::cast<T>(1);
		else if (this->n == 1) this->current_state = (this->x - utils::cast<T>(1)) * utils::cast<T>(-0.5);
		else this->current_state *= utils::cast<T>(-1) * (this->x - utils::cast<T>(1)) * utils::cast<T>(0.5) * utils::cast<T>(2*this->n - 3) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
