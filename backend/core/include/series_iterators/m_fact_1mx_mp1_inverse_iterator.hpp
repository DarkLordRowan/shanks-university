#ifndef M_FACT_1MX_MP1_INVERSE_ITERATOR_HPP
#define M_FACT_1MX_MP1_INVERSE_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file m_fact_1mx_mp1_inverse_iterator.hpp
 * @brief Iterator for the series expansion of m! / (1 - x)^(m + 1).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

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
template <AcceptedLike T, UnsignedIntLike K>
class m_fact_1mx_mp1_inverse_iterator final : public series_base_succ<T, K> {
public:
    K m = static_cast<K>(0); /**< The derivative order m. */

    /**
     * @brief Default constructor for m_fact_1mx_mp1_inverse_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit m_fact_1mx_mp1_inverse_iterator(T x, K m) : series_base_succ<T, K>(x), m(m) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    explicit m_fact_1mx_mp1_inverse_iterator(T x, T m) = delete;

    /**
     * @brief Retrieves the analytic sum of the series (m! / (1 - x)^(m + 1)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of m! / (1 - x)^(m + 1).
     */
    T get_sum() const override {
        return utils::cast<T, K>()(utils::math<K>::fact(m)) /
               utils::math<T>::pow(utils::cast<T, int>()(1) - this->x, utils::cast<T, K>()(m + 1));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x) >= utils::cast<float_type, int>()(1);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence relation based on the binomial coefficients for the m-th derivative of 1/(1-x)
        if (n == 0)
            state = utils::cast<T, K>()(utils::math<K>::fact(m), utils::helpers<T>::get_precision(state));
        else
            state *= this->x * utils::cast<T, K>()(m + static_cast<K>(n)) / utils::cast<T, K>()(n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
