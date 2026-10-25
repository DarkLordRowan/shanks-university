#ifndef TWO_LN2_ITERATOR_HPP
#define TWO_LN2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file two_ln2_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * 2 * ln(2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = x * 2 * ln(2).
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x * 2 * ln(2) (or x * ln(4)).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class two_ln2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for two_ln2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit two_ln2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * 2 * ln(2).
     */
    T get_sum() const override {
        return this->x * utils::math<T>::log(utils::cast<T, int>()(4, utils::helpers<T>::get_precision(this->x)));
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
        // Promote before multiplying: n^5 overflows a 32-bit index even
        // for moderate n. Partial fractions telescope to 2*log(2):
        // -1/k + 1/(2k-1) + 1/(2k+1) + 1/(2k-1)^2 - 1/(2k+1)^2.
        // Harmonic/log limit: https://dlmf.nist.gov/5.2#E3
        const T k = utils::cast<T, K>()(n) + utils::cast<T, int>()(1);
        const T square = k * k;
        const T factor = utils::cast<T, int>()(4) * square - utils::cast<T, int>()(1);
        state = (utils::cast<T, int>()(12) * square - utils::cast<T, int>()(1)) * this->x /
                (k * factor * factor);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
