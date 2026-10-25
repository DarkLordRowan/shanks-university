#ifndef PI_3_ITERATOR_HPP
#define PI_3_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_3_iterator.hpp
 * @brief Iterator for a series expansion related to x * pi / 3.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi / 3.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x * pi / 3.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_3_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_3_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_3_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * pi / 3).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi / 3.
     */
    T get_sum() const override {
        return this->x * utils::cast<T, double>()(std::numbers::pi) / utils::cast<T, int>()(3);
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
        state=this->x * (utils::cast<T,int>()(1,precision)) / (k*(utils::cast<T,int>()(2,precision)*k-utils::cast<T,int>()(1,precision))*(utils::cast<T,int>()(4,precision)*k-utils::cast<T,int>()(3,precision)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
