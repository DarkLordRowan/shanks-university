#ifndef MEAN_SINH_SIN_ITERATOR_HPP
#define MEAN_SINH_SIN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file mean_sinh_sin_iterator.hpp
 * @brief Iterator for the series expansion of 0.5 * (sinh(x) + sin(x)).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = 0.5 * (sinh(x) + sin(x)).
 *
 * This class implements the Maclaurin expansion of the mean of hyperbolic and
 * standard sine functions, which converges for all finite values of x.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class mean_sinh_sin_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for mean_sinh_sin_iterator.
     * @authors Bolshakov M.P.
     */
	mean_sinh_sin_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (0.5 * (sinh(x) + sin(x))).
     * @authors Bolshakov M.P.
     * @return T The value of 0.5 * (sinh(x) + sin(x)).
     */
	T sum() const override{ return (utils::sinh(this->x) + utils::sin(this->x)) * utils::cast<T>(0.5);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the mean(sinh, sin) expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation for the combined expansion terms
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::pow(this->x, utils::cast<T>(4)) /
		utils::cast<T>(
			utils::fma(size_t{4},this->n-1,size_t{5})*utils::fma(size_t{4},this->n-1,size_t{4}) *
			utils::fma(size_t{4},this->n-1,size_t{3})*utils::fma(size_t{4},this->n-1,size_t{2})
		);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
