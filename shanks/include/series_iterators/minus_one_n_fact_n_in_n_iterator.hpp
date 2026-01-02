#ifndef MINUS_ONE_N_FACT_IN_N_ITERATOR_HPP
#define MINUS_ONE_N_FACT_IN_N_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file minus_one_n_fact_n_in_n_iterator.hpp
 * @brief Iterator for the series expansion related to (-1)^n * n! / n^n.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for a specific combinatorial series.
 *
 * This class implements a specific series expansion whose sum is approximately
 * -0.65583160 * x.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class minus_one_n_fact_n_in_n_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for minus_one_n_fact_n_in_n_iterator.
     * @authors Bolshakov M.P.
     */
	minus_one_n_fact_n_in_n_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the approximate sum of the series.
     * @authors Bolshakov M.P.
     * @return T The analytic sum.
     */
	T sum() const override{ return this->x * utils::cast<T>(-0.65583160);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the combinatorial series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term: (-1)^(n+1) * (n+1)! * x / (n+1)^(n+1)
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n + 1) *
		utils::cast<T>(utils::fact<K>(this->n+1)) * this->x / utils::cast<T>(utils::pow(this->n+1, this->n+1));
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
