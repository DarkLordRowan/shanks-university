#ifndef PI_ITERATOR_HPP
#define PI_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file pi_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * pi.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x * pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	pi_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * pi).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi.
     */
	T sum() const override{ return this->x * utils::cast<T>(std::numbers::pi);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion for x * pi.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term of the specific expansion
		if (this->n == 0) this->current_state = this->x * utils::sqrt(utils::cast<T>(12));
		else this->current_state *= utils::cast<T>(-1) * utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1})) /
		utils::cast<T>(3 * utils::fma(size_t{2},this->n,size_t{1}));

		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
