#ifndef FIVE_PI_TWELVE_ITERATOR_HPP
#define FIVE_PI_TWELVE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file five_pi_twelve_iterator.hpp
 * @brief Iterator for the series expansion related to 5*pi*x/12.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x * 5pi / 12.
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * 5pi / 12.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class five_pi_twelve_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for five_pi_twelve_iterator.
     * @authors Bolshakov M.P.
     */
	five_pi_twelve_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * 5pi / 12).
     * @authors Bolshakov M.P.
     * @return T The value of x * 5pi / 12.
     */
	T sum() const override{ return this->x * utils::cast<T>(std::numbers::pi * 5) / utils::cast<T>(12);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Terms of the series expansion contributing to the 5pi/12 * x result
		if (this->n == 0) this->current_state = utils::cast<T>(5) * this->x / utils::cast<T>(3);
		else this->current_state = utils::cast<T>(5) / utils::cast<T>(3) *
		utils::minus_one_raised_to_power_n<T,K>(this->n) * this->x / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
