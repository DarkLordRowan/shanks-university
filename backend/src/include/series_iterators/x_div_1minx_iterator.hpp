#ifndef X_DIV_1MINX_ITERATOR_HPP
#define X_DIV_1MINX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file x_div_1minx_iterator.hpp
 * @brief Iterator for the series expansion of x / sqrt(1 - x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = x / sqrt(1 - x).
 *
 * This class implements the Maclaurin expansion of the given algebraic function,
 * which converges for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class x_div_1minx_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for x_div_1minx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	x_div_1minx_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x / sqrt(1 - x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x / sqrt(1 - x).
     */
	T sum() const override{ return this->x / utils::sqrt(utils::cast<T>(1.0) - this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the power series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation based on the generalized binomial theorem for (1-x)^(-1/2)
		if (this->n == 0) this->current_state = this->x;
		else  this->current_state *= this->x * utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(2 * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
