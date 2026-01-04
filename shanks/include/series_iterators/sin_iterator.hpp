#ifndef SIN_ITERATOR_HPP
#define SIN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file sin_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the sine function sin(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the sine function sin(x).
 *
 * This class implements the Maclaurin expansion of sin(x), which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class sin_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for sin_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	sin_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (sin(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of sin(x).
     */
	T sum() const override{ return utils::sin(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the sin(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence logic for the alternating odd-power terms of the sine expansion
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1.0) * this->x * this->x /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 2 * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
