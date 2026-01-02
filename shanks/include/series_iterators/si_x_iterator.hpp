#ifndef SI_X_ITERATOR_HPP
#define SI_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file si_x_iterator.hpp
 * @brief Iterator for the series expansion of the Sine Integral Si(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the Sine Integral function Si(x).
 *
 * This class implements the Maclaurin expansion of Si(x), which converges
 * for all finite values of x.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class si_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for si_x_iterator.
     * @authors Bolshakov M.P.
     */
	si_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (Si(x)).
     * @authors Bolshakov M.P.
     * @return T The value of the Sine Integral function at point x.
     */
	T sum() const override{ return utils::si_x(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the Si(x) expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term involving iterated logarithms and powers
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x *
		utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1})) /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 2 * utils::fma(size_t{2},this->n,size_t{1}) * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
