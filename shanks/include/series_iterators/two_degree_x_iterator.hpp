#ifndef TWO_DEGREE_X_ITERATOR_HPP
#define TWO_DEGREE_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file two_degree_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 2^x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the exponential function f(x) = 2^x.
 *
 * This class implements the Maclaurin expansion of 2^x, which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class two_degree_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for two_degree_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	two_degree_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (2^x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 2^x.
     */
	T sum() const override{ return utils::pow(utils::cast<T>(2), this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the 2^x Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Each term is (x * ln(2))^n / n!, computed recursively
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= this->x * utils::log(utils::cast<T>(2)) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
