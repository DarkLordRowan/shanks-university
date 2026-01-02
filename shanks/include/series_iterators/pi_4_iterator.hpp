#ifndef PI_4_ITERATOR_HPP
#define PI_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file pi_4_iterator.hpp
 * @brief Iterator for the Gregory-Leibniz-like series related to x * pi / 4.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = x * pi / 4.
 * 
 * This class implements an alternating series expansion whose sum is x * pi / 4.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_4_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_4_iterator.
     * @authors Bolshakov M.P.
     */
	pi_4_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series (x * pi / 4).
     * @authors Bolshakov M.P.
     * @return T The value of x * pi / 4.
     */
	T sum() const override{ return this->x * utils::cast<T>(std::numbers::pi * 0.25); }
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the alternating series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series ((-1)^n * x / (2n+1)).
     */
	T next() override {
	
		// Standard alternating series term for the atan(1) expansion scaled by x
		this->current_state = utils::minus_one_raised_to_power_n<T , K>(this->n) * this->x / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif