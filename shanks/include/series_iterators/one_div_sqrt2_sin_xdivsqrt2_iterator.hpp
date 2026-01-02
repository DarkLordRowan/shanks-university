#ifndef ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#define ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file one_div_sqrt2_sin_xdivsqrt2_iterator.hpp
 * @brief Iterator for the series expansion of (1/sqrt(2)) * sin(x/sqrt(2)).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = (1/sqrt(2)) * sin(x/sqrt(2)).
 * 
 * This class implements the Maclaurin expansion of the scaled sine function, 
 * which converges for all finite values of x.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_div_sqrt2_sin_xdivsqrt2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for one_div_sqrt2_sin_xdivsqrt2_iterator.
     * @authors Bolshakov M.P.
     */
	one_div_sqrt2_sin_xdivsqrt2_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series ((1/sqrt(2)) * sin(x/sqrt(2))).
     * @authors Bolshakov M.P.
     * @return T The value of (1/sqrt(2)) * sin(x/sqrt(2)).
     */
	T sum() const override{ return utils::cast<T>(1) / utils::sqrt(utils::cast<T>(2)) * utils::sin(this->x * utils::cast<T>(1) / utils::sqrt(utils::cast<T>(2)));}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the scaled sine Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation derived from the sin(u) expansion with u = x/sqrt(2)
		if (this->n == 0) this->current_state = this->x * utils::cast<T>(0.5);
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x / utils::cast<T>(4 * this->n * utils::fma(size_t{2},this->n,size_t{1}));

		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif