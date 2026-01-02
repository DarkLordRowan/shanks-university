#ifndef MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#define MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file minus_3_div_4_or_x_minus_3_div_4_iterator.hpp
 * @brief Iterator for the Fourier series expansion of a piecewise linear function.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier series iterator for the piecewise function f(x) = -0.75 if x < 0, else x - 0.75.
 *
 * This class implements the Fourier expansion for a specific saw-tooth like
 * piecewise linear function.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class minus_3_div_4_or_x_minus_3_div_4_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for minus_3_div_4_or_x_minus_3_div_4_iterator.
     * @authors Bolshakov M.P.
     */
	minus_3_div_4_or_x_minus_3_div_4_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (the value of the piecewise function).
     * @authors Bolshakov M.P.
     * @return T The analytic sum.
     */
	T sum() const override{
		using float_type = GetUnderlyingType<T>::value;
		if constexpr (isComplexLike<T>::value){
			if (this->x.real() <= utils::cast<float_type>(0))
				return utils::cast<T>(-0.75);
		} else {
			if (this->x <= utils::cast<T>(0))
           		return utils::cast<T>(-0.75);
		}

		return this->x - utils::cast<T>(0.75);
	}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= 3 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(3.0);
	}

    /**
     * @brief Computes the next term in the Fourier expansion of the piecewise function.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		const T piDiv3 = utils::cast<T>(std::numbers::pi) / utils::cast<T>(3);
		const T shifted_x = utils::cast<T>(this->n+1)*piDiv3*this->x;

		// Fourier series term formula involving both sine and cosine components
		this->current_state = utils::cast<T>(-2) /
		(piDiv3 * piDiv3 * utils::cast<T>(3 * utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{1}))) *
		utils::cos(shifted_x) + utils::minus_one_raised_to_power_n<T, K>(this->n) / (piDiv3 * utils::cast<T>(this->n+1)) * utils::sin(shifted_x);
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
