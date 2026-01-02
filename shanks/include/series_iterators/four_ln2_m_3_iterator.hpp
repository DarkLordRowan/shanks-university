#ifndef FOUR_LN2_M_3_ITERATOR_HPP
#define FOUR_LN2_M_3_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file four_ln2_m_3_iterator.hpp
 * @brief Iterator for the series expansion related to 4*ln(2) - 3.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x * (4 * ln(2) - 3).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (4 * ln(2) - 3).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class four_ln2_m_3_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for four_ln2_m_3_iterator.
     * @authors Bolshakov M.P.
     */
	four_ln2_m_3_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * (4 * ln(2) - 3)).
     * @authors Bolshakov M.P.
     * @return T The value of x * (4 * ln(2) - 3).
     */
	T sum() const override{ return this->x * (utils::cast<T>(4) * utils::log(utils::cast<T>(2)) - utils::cast<T>(3));}

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

		// Term formula based on the specific power series expansion for 4*ln(2)-3
		this->current_state = utils::minus_one_raised_to_power_n<T,K>(this->n+1) * this->x / utils::cast<T>((this->n+1)*(this->n+1)*(this->n+2)*(this->n+2));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
