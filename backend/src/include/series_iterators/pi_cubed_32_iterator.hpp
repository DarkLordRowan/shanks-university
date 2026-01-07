#ifndef PI_CUBED_32_ITERATOR_HPP
#define PI_CUBED_32_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file pi_cubed_32_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * pi^3 / 32.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi^3 / 32.
 *
 * This class implements an alternating series expansion whose analytic sum equals
 * x * pi^3 / 32.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_cubed_32_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for pi_cubed_32_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	pi_cubed_32_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (x * pi^3 / 32).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi^3 / 32.
     */
	T sum() const override{ return this->x * utils::pow(utils::cast<T>(std::numbers::pi), utils::cast<T>(3)) / utils::cast<T>(32);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the alternating series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ((-1)^n * x / (2n+1)^3).
     */
	T next() override {

		// Alternating term with cubic denominator
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * this->x /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) *
		utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{1}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
