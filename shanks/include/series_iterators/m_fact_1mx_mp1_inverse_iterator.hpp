#ifndef M_FACT_1MX_MP1_INVERSE_ITERATOR_HPP
#define M_FACT_1MX_MP1_INVERSE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file m_fact_1mx_mp1_inverse_iterator.hpp
 * @brief Iterator for the series expansion of m! / (1 - x)^(m + 1).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = m! / (1 - x)^(m + 1).
 *
 * This class implements the expansion of the (m)-th derivative of the geometric series,
 * which converges for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class m_fact_1mx_mp1_inverse_iterator final : public series_base_iterator<T, K>{
public:

	K m = static_cast<K>(0); /**< The derivative order m. */

    /**
     * @brief Default constructor for m_fact_1mx_mp1_inverse_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	m_fact_1mx_mp1_inverse_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (m! / (1 - x)^(m + 1)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of m! / (1 - x)^(m + 1).
     */
	T sum() const override{ return utils::cast<T>(utils::fact<K>(m)) / utils::pow(utils::cast<T>(1) - this->x, utils::cast<T>(m+1));}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation based on the binomial coefficients for the m-th derivative of 1/(1-x)
		if (this->n == 0) this->current_state = utils::cast<T>(utils::fact<K>(m));
		else this->current_state *= this->x * utils::cast<T>(m + static_cast<K>(this->n)) / utils::cast<T>(this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
