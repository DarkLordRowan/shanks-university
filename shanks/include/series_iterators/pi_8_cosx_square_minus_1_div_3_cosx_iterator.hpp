#ifndef PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#define PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file pi_8_cosx_square_minus_1_div_3_cosx_iterator.hpp
 * @brief Iterator for the series expansion related to (pi/8)*cos^2(x) - cos(x)/3.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier-like series iterator for the trigonometric function f(x) = (pi/8)*cos^2(x) - cos(x)/3.
 * 
 * This class implements a specific expansion whose sum involves squared and linear 
 * cosine terms, typically valid for x in [-pi/2, pi/2].
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_8_cosx_square_minus_1_div_3_cosx_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_8_cosx_square_minus_1_div_3_cosx_iterator.
     * @authors Bolshakov M.P.
     */
	pi_8_cosx_square_minus_1_div_3_cosx_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of (pi/8)*cos^2(x) - cos(x)/3.
     */
	T sum() const override{
		return utils::cast<T>(std::numbers::pi * 0.125) * utils::cos(this->x) * utils::cos(this->x) - utils::cos(this->x) / utils::cast<T>(3);
	}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > pi/2 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(std::numbers::pi * 0.5);
	}

    /**
     * @brief Computes the next term in the trigonometric series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {
	
		// General term formula involving alternating higher-order cosine harmonics
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * utils::cos(utils::cast<T>(utils::fma(size_t{2},this->n,size_t{3})) * this->x) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3}) * utils::fma(size_t{2},this->n,size_t{5}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif