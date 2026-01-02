#ifndef PI_3_ITERATOR_HPP
#define PI_3_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file pi_3_iterator.hpp
 * @brief Iterator for a series expansion related to x * pi / 3.
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi / 3.
 * 
 * This class implements a specific series expansion whose analytic sum equals 
 * x * pi / 3.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_3_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_3_iterator.
     * @authors Bolshakov M.P.
     */
	pi_3_iterator() : series_base_iterator<T, K>() {}
	
    /**
     * @brief Retrieves the analytic sum of the series (x * pi / 3).
     * @authors Bolshakov M.P.
     * @return T The value of x * pi / 3.
     */
	T sum() const override{ return this->x * utils::cast<T>(std::numbers::pi) / utils::cast<T>(3);}
	
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
	
		// Formula for the n-th term of the specific series expansion
		this->current_state = this->x / utils::cast<T>((this->n+1)*
		utils::fma(size_t{2},this->n,size_t{1})*utils::fma(size_t{4},this->n,size_t{1}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif