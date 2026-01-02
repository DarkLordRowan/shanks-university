#ifndef INVERSE_1MX_ITERATOR_HPP
#define INVERSE_1MX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file inverse_1mx_iterator.hpp
 * @brief Iterator for the geometric series expansion of 1/(1-x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = 1 / (1 - x).
 *
 * This class implements the classic geometric series expansion, which converges
 * for values of x such that |x| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class inverse_1mx_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for inverse_1mx_iterator.
     * @authors Bolshakov M.P.
     */
	inverse_1mx_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (1 / (1 - x)).
     * @authors Bolshakov M.P.
     * @return T The value of 1 / (1 - x).
     */
	T sum() const override{ return utils::cast<T>(1) / (utils::cast<T>(1) - this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the geometric series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series (x^n).
     */
	T next() override {

		// Terms of the geometric series are powers of x
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= this->x;

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
