#ifndef SINH_ITERATOR_HPP
#define SINH_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file sinh_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the hyperbolic sine sinh(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the hyperbolic sine function sinh(x).
 *
 * This class implements the Maclaurin expansion of sinh(x), which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class sinh_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for sinh_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	sinh_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (sinh(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of sinh(x).
     */
	T sum() const override{ return utils::sinh(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the sinh(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence logic for the odd-power terms of the sinh expansion
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * 2 * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
