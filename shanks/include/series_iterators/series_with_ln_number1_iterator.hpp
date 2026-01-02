#ifndef SERIES_WITH_LN_NUMBER1_ITERATOR_HPP
#define SERIES_WITH_LN_NUMBER1_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file series_with_ln_number1_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * 0.599195688977.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for a complex expansion summing to x * 0.599195688977.
 * 
 * This class implements a specific series expansion whose analytic sum equals 
 * approximately 0.599195688977 * x.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number1_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for series_with_ln_number1_iterator.
     * @authors Bolshakov M.P.
     */
	series_with_ln_number1_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the approximate analytic sum of the series.
     * @authors Bolshakov M.P.
     * @return T The value of 0.599195688977 * x.
     */
	T sum() const override{ return utils::cast<T>(0.599195688977) * this->x;}
	
    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the specific expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {
		const T n1 = utils::cast<T>(this->n + 1);
		const T n1_2 = n1 * n1;

		// Specific term formula involving factorials, exponentials, and logarithms
		this->current_state = utils::log(utils::cast<T>(1) + 
		utils::pow(n1, n1_2 + n1 * utils::cast<T>(0.5)) / (utils::pow(utils::cast<T>(utils::fact<K>(this->n+1)), n1) * utils::exp(n1_2))) * this->x;
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif