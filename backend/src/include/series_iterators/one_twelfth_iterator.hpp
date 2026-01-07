#ifndef ONE_TWELFTH_ITERATOR_HPP
#define ONE_TWELFTH_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file one_twelfth_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x/12.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled linear function f(x) = x / 12.
 *
 * This class implements a specific series expansion whose analytic sum is x / 12.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for one_twelfth_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	one_twelfth_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x / 12).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x / 12.
     */
	T sum() const override{ return this->x / utils::cast<T>(12);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term of the expansion: x / ((2n+1)(2n+3)(2n+5))
		this->current_state = this->x / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) *
		utils::fma(size_t{2},this->n,size_t{3}) * utils::fma(size_t{2},this->n,size_t{5}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
