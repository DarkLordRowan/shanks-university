#ifndef ARCTANH_X_ITERATOR_HPP
#define ARCTANH_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"

/**
 * @file arctanh_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of arctanh(x).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the inverse hyperbolic tangent function arctanh(x).
 *
 * This class implements the Maclaurin expansion of arctanh(x), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class arctanh_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for arctanh_x_iterator.
     * @authors Bolshakov M.P.
     */
	arctanh_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (arctanh(x)).
     * @authors Bolshakov M.P.
     * @return T The value of arctanh(x).
     */
	T sum() const override{ return utils::atanh(this->x);}

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
     * @brief Computes the next term in the arctanh(x) Taylor expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
	T next() override {

		// Standard Taylor expansion for inverse hyperbolic tangent
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x *
		utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
