#ifndef SERIES_WITH_LN_NUMBER2_ITERATOR_HPP
#define SERIES_WITH_LN_NUMBER2_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file series_with_ln_number2_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * 5.71617784132.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for a complex expansion summing to x * 5.71617784132.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * approximately 5.71617784132 * x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number2_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for series_with_ln_number2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	series_with_ln_number2_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the approximate analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 5.71617784132 * x.
     */
	T sum() const override{ return utils::cast<T>(5.71617784132) * this->x;}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the specific expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term involving iterated logarithms and powers
		const T n1 = utils::cast<T>(this->n + 1);
		const T n1_2 = n1 * n1;

		this->current_state = this->x / utils::pow(utils::log(utils::cast<T>(this->n+2)), utils::log(utils::cast<T>(this->n+2)));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
