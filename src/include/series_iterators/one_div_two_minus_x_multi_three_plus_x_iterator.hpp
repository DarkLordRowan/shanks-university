#ifndef ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#define ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file one_div_two_minus_x_multi_three_plus_x_iterator.hpp
 * @brief Iterator for the series expansion of 1 / ((2 - x) * (3 + x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the rational function f(x) = 1 / ((2 - x) * (3 + x)).
 *
 * This class implements the power series expansion of the given rational function,
 * obtained via partial fraction decomposition. It converges for values of x
 * such that |x| < 2.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class one_div_two_minus_x_multi_three_plus_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for one_div_two_minus_x_multi_three_plus_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	one_div_two_minus_x_multi_three_plus_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (1 / ((2 - x) * (3 + x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the rational function at x.
     */
	T sum() const override{
		return utils::cast<T>(1) / ((utils::cast<T>(2) - this->x) * (utils::cast<T>(3) + this->x));
	}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 2 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(2.0);
	}

    /**
     * @brief Computes the next term in the power series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term based on partial fraction decomposition components
		this->current_state = utils::pow(this->x, utils::cast<T>(this->n)) *
		utils::cast<T>(0.2) * (utils::minus_one_raised_to_power_n<T, K>(this->n) * utils::cast<T>(utils::pow(size_t{2}, this->n + 1)) +
		utils::cast<T>(utils::pow(size_t{3}, this->n + 1))) / utils::cast<T>(utils::pow(size_t{6}, this->n+1));

		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
