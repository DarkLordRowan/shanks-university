#ifndef FOUR_ARCTAN_ITERATOR_HPP
#define FOUR_ARCTAN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file four_arctan_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 4*arctan(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = 4 * arctan(x).
 *
 * This class implements the Maclaurin expansion of 4 * arctan(x), which converges
 * for values of x such that |x| <= 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class four_arctan_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for four_arctan_iterator.
     * @authors Bolshakov M.P.
     */
	four_arctan_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (4 * arctan(x)).
     * @authors Bolshakov M.P.
     * @return T The value of 4 * arctan(x).
     */
	T sum() const override{ return utils::cast<T>(4) * utils::atan(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}


    /**
     * @brief Computes the next term in the 4 * arctan(x) Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence logic for the alternating odd-power terms of the arctan expansion
		if (this->n == 0) this->current_state = utils::cast<T>(4) * this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x * utils::cast<T>(2 * this->n - 1) /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
