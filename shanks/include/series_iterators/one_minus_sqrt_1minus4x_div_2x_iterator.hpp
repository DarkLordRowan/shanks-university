#ifndef ONE_MINUS_SQRT_1MINUS4X_DIV_2X_ITERATOR_HPP
#define ONE_MINUS_SQRT_1MINUS4X_DIV_2X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file one_minus_sqrt_1minus4x_div_2x_iterator.hpp
 * @brief Iterator for the series expansion of (1 - sqrt(1 - 4x)) / (2x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = (1 - sqrt(1 - 4x)) / (2x).
 * 
 * This class implements the Maclaurin expansion of the generating function for 
 * Catalan numbers, which converges for values of x such that |x| <= 0.25 (excluding x=0).
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_minus_sqrt_1minus4x_div_2x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for one_minus_sqrt_1minus4x_div_2x_iterator.
     * @authors Bolshakov M.P.
     */
	one_minus_sqrt_1minus4x_div_2x_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series ((1 - sqrt(1 - 4x)) / (2x)).
     * @authors Bolshakov M.P.
     * @return T The value of the generating function at x.
     */
	T sum() const override{ return (utils::cast<T>(1) - utils::sqrt(utils::cast<T>(1) - utils::cast<T>(4) * this->x)) / (utils::cast<T>(2) * this->x);}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > 0.25, x is zero, or x is non-finite; false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(0.25) || this->x == utils::cast<T>(0);
	}

    /**
     * @brief Computes the next term in the Catalan generating function expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series (Catalan_n * x^n).
     */
	T next() override {

		// Catalan numbers generating function terms: C_n * x^n
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else if (this->n == 1) this->current_state = this->x;
		else this->current_state *= this->x * utils::cast<T>(2*utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(this->n+1);

		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif