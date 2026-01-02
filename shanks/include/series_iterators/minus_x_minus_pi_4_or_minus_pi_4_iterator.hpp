#ifndef MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#define MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file minus_x_minus_pi_4_or_minus_pi_4_iterator.hpp
 * @brief Iterator for the Fourier series expansion of a specific piecewise function.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier series iterator for the piecewise function f(x) = -x if -pi <= x < 0, else 0 if 0 <= x <= pi.
 *
 * This class implements the Fourier expansion for a half-rectified linear function.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class minus_x_minus_pi_4_or_minus_pi_4_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for minus_x_minus_pi_4_or_minus_pi_4_iterator.
     * @authors Bolshakov M.P.
     */
	minus_x_minus_pi_4_or_minus_pi_4_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (the value of the piecewise function).
     * @authors Bolshakov M.P.
     * @return T The value of the piecewise function at x.
     */
	T sum() const override{
		using float_type = GetUnderlyingType<T>::value;

		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<float_type>(0))
				return utils::cast<T>(-1) * this->x;
		} else {
			if (this->x <= utils::cast<T>(0))
           		return utils::cast<T>(-1) * this->x;
		}

		return utils::cast<T>(0);
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

		// First term is the DC component (constant term)
		if (this->n == 0) this->current_state = utils::cast<T>(std::numbers::pi * 0.25);
		else {
			const T piDiv3 = utils::cast<T>(std::numbers::pi) / utils::cast<T>(3);
			const T cos_coef = (utils::minus_one_raised_to_power_n<T, K>(this->n) - utils::cast<T>(1)) / (utils::cast<T>(std::numbers::pi)*utils::cast<T>(this->n * this->n));
			const T sin_coef = utils::minus_one_raised_to_power_n<T, K>(this->n) / utils::cast<T>(this->n);
			this->current_state = cos_coef * utils::cos(utils::cast<T>(this->n) * this->x) + sin_coef * utils::sin(utils::cast<T>(this->n) * this->x);
		}

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
