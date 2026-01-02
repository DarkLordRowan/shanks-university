#ifndef ONE_TWELFTH_X2_PI2_ITERATOR_HPP
#define ONE_TWELFTH_X2_PI2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file one_twelfth_x2_pi2_iterator.hpp
 * @brief Iterator for the Fourier series expansion of x*(x^2 - pi^2)/12.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier series iterator for the cubic function f(x) = x * (x^2 - pi^2) / 12.
 * 
 * This class implements the Fourier expansion for the given odd cubic function,
 * which converges for values of x within the range [-pi, pi].
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_x2_pi2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for one_twelfth_x2_pi2_iterator.
     * @authors Bolshakov M.P.
     */
	one_twelfth_x2_pi2_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series (x * (x^2 - pi^2) / 12).
     * @authors Bolshakov M.P.
     * @return T The value of the cubic function at point x.
     */
	T sum() const override{ return this->x / utils::cast<T>(12) * (this->x * this->x - utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi));}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the Fourier expansion of the cubic function.
     * @authors Bolshakov M.P.
     * @return T The next term of the series ((-1)^(n+1) * sin((n+1)x) / (n+1)^3).
     */
	T next() override {
		
		// Formula for the n-th Fourier harmonic
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n+1) * 
		utils::sin(utils::cast<T>(this->n+1) * this->x) / utils::cast<T>(utils::pow(this->n+1,size_t{3}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif