#ifndef ONE_ITERATOR_HPP
#define ONE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file one_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the identity function f(x) = x.
 * 
 * This class implements a specific series expansion whose analytic sum is 
 * simply the value x.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for one_iterator.
     * @authors Bolshakov M.P.
     */
	one_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series (x).
     * @authors Bolshakov M.P.
     * @return T The value of x.
     */
	T sum() const override{ return this->x;}
	
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
		
		// Formula for the n-th term of the expansion: x / ((n+1) * (n+2))
		this->current_state = this->x / utils::cast<T>((this->n+1)*(this->n+2));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif