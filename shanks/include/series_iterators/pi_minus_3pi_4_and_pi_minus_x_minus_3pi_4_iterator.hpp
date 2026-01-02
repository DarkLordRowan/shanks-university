#ifndef PI_MINUS_3PI_4_AND_PI_MINUS_X_MINUS_3PI_4_ITERATOR_HPP
#define PI_MINUS_3PI_4_AND_PI_MINUS_X_MINUS_3PI_4_ITERATOR_HPP
#pragma once

#include <numbers>
#include "series_base_iterator.hpp"

/**
 * @file pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator.hpp
 * @brief Iterator for the Fourier series expansion of a specific piecewise linear function.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier series iterator for the piecewise function f(x) = pi/4 if x < 0, else pi/4 - x if x >= 0.
 * 
 * This class implements the Fourier expansion for a specific piecewise linear 
 * function, which converges for values of x such that |x| < pi.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator.
     * @authors Bolshakov M.P.
     */
	pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series (the piecewise function value).
     * @authors Bolshakov M.P.
     * @return T The value of the piecewise function at current point x.
     */
	T sum() const override{

		using float_type = GetUnderlyingType<T>::value;

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<float_type>(0))
				return utils::cast<T>(0.25 * std::numbers::pi);
		} else {
			if (this->x <= utils::cast<T>(0))
           		return utils::cast<T>(0.25 * std::numbers::pi);
		}

		return utils::cast<T>(0.25 * std::numbers::pi) - this->x;
	}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= pi or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
	}

    /**
     * @brief Computes the next term in the Fourier expansion of the piecewise function.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {
	
		// General Fourier term formula involving alternating cosine and sine components
		const K n1 = this->n+1;
		this->current_state = 
		utils::cos(utils::cast<T>(n1) * this->x) * (utils::cast<T>(1) + utils::minus_one_raised_to_power_n<T, K>(this->n)) / (utils::cast<T>(std::numbers::pi) * utils::cast<T>(n1 * n1)) + 
		utils::sin(utils::cast<T>(n1) * this->x) * utils::minus_one_raised_to_power_n<T,K>(n1) / utils::cast<T>(n1);
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif