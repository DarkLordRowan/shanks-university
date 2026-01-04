#ifndef SIN_X2_ITERATOR_HPP
#define SIN_X2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file sin_x2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of sin(x^2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = sin(x^2).
 *
 * This class implements the Maclaurin expansion of sin(x^2), which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class sin_x2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for sin_x2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	sin_x2_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (sin(x^2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of sin(x^2).
     */
	T sum() const override{ return utils::sin(this->x * this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the sin(x^2) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation derived from the sin Taylor expansion with substitution x -> x^2
		if (this->n == 0) this->current_state = this->x * this->x;
		else this->current_state *= utils::cast<T>(-1) * utils::pow(this->x, utils::cast<T>(4)) /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 2 * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
