#ifndef K_X_ITERATOR_HPP
#define K_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file k_x_iterator.hpp
 * @brief Iterator for the series expansion of the complete elliptic integral K(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the complete elliptic integral of the first kind K(x).
 *
 * This class implements the standard power series expansion for K(x), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class k_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for k_x_iterator.
     * @authors Bolshakov M.P.
     */
	k_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (K(x)).
     * @authors Bolshakov M.P.
     * @return T The value of the complete elliptic integral K(x).
     */
	T sum() const override{ return utils::k_x(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the K(x) expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// First term is pi/2, subsequent terms use the squared ratio of odd factorials
		if (this->n == 0) this->current_state = utils::cast<T>(std::numbers::pi * 0.5);
		else this->current_state *= this->x * this->x *
		utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1}) *
		utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(4 * this->n * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
