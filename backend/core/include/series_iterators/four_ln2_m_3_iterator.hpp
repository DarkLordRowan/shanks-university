#ifndef FOUR_LN2_M_3_ITERATOR_HPP
#define FOUR_LN2_M_3_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file four_ln2_m_3_iterator.hpp
 * @brief Iterator for the series expansion related to 4*ln(2) - 3.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = x * (4 * ln(2) - 3).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (4 * ln(2) - 3).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class four_ln2_m_3_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for four_ln2_m_3_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit four_ln2_m_3_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * (4 * ln(2) - 3)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (4 * ln(2) - 3).
     */
    T get_sum() const override {
        return this->x *
               (utils::cast<T, int>()(4) * utils::math<T>::log(utils::cast<T, int>()(2)) - utils::cast<T, int>()(3));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T, int>()(0);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Exact rational coefficients; promote before products to avoid index overflow.
        // Partial fractions and harmonic sums: https://dlmf.nist.gov/5.9
        const auto precision=utils::helpers<T>::get_precision(this->x);
        const T k=utils::cast<T,K>()(n,precision)+utils::cast<T,int>()(1,precision);
        state=this->x * utils::math<T>::template minus_one_raised_to_power_n<K>(n) * (-utils::cast<T,int>()(1,precision)) / (k*k*(k+utils::cast<T,int>()(1,precision))*(k+utils::cast<T,int>()(1,precision)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
