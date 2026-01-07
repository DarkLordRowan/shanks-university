#ifndef ONE_THIRD_PI_SQUARED_M_NINE_ITERATOR_HPP
#define ONE_THIRD_PI_SQUARED_M_NINE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file one_third_pi_squared_m_nine_iterator.hpp
 * @brief Iterator for the series expansion related to x * (pi^2 - 9) / 3.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = x * (pi^2 - 9) / 3.
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (pi^2 - 9) / 3.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_third_pi_squared_m_nine_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for one_third_pi_squared_m_nine_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	one_third_pi_squared_m_nine_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * (pi^2 - 9) / 3).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (pi^2 - 9) / 3.
     */
	T sum() const override{ return this->x * (utils::cast<T>(std::numbers::pi) * utils::cast<T>(std::numbers::pi) - utils::cast<T>(9)) / utils::cast<T>(3);}

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

		// Formula for the n-th term of the specific expansion
		this->current_state = this->x / utils::cast<T>((this->n+1)*(this->n+1)*(this->n+2)*(this->n+2));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
