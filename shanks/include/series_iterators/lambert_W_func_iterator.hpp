#ifndef LAMBERT_W_FUNC_ITERATOR_HPP
#define LAMBERT_W_FUNC_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file lambert_W_func_iterator.hpp
 * @brief Iterator for the series expansion of the principal branch of the Lambert W function W0(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the principal branch of the Lambert W function, W0(x).
 *
 * This class implements the Maclaurin expansion of W0(x), which converges for
 * values of x such that |x| < 1/e.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class lambert_W_func_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for lambert_W_func_iterator.
     * @authors Bolshakov M.P.
     */
	lambert_W_func_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (W0(x)).
     * @authors Bolshakov M.P.
     * @return T The value of the principal branch of the Lambert W function.
     */
	T sum() const override{ return utils::lambertW0(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > 1/e or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1) / utils::cast<float_type>(std::numbers::e);
	}

    /**
     * @brief Computes the next term in the Lambert W0(x) expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Implementation of the power series for W0(x)
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
