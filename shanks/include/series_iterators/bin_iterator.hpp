#ifndef BIN_ITERATOR_HPP
#define BIN_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file bin_iterator.hpp
 * @brief Iterator for the binomial series (1+x)^alpha.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the binomial function (1+x)^alpha.
 *
 * This class implements the generalized binomial expansion, which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class bin_iterator final : public series_base_iterator<T, K>{
public:

	T alpha = utils::cast<T>(0.0); /**< The exponent alpha in the binomial expansion. */

    /**
     * @brief Default constructor for bin_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	bin_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series ((1+x)^alpha).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (1+x)^alpha.
     */
	T sum() const override{ return utils::pow(utils::cast<T>(1.0) + this->x, alpha); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the binomial series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {
		// First term is always 1.0, subsequent terms use the binomial recurrence
		if (this->n == 0) this->current_state = utils::cast<T>(1.0);
		else this->current_state *= (this->alpha - utils::cast<T>(this->n - static_cast<K>(1))) * this->x / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
