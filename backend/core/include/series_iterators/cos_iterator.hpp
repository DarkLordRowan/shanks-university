#ifndef COS_ITERATOR_HPP
#define COS_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file cos_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the cosine function cos(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the cosine function cos(x).
 *
 * This class implements the Maclaurin expansion of cos(x), which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class cos_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for cos_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit cos_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (cos(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of cos(x).
     */
    T get_sum() const override { return utils::math<T>::cos(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the cos(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence logic for the alternating even-power terms of the cosine expansion
        if (n == 0)
            state = utils::cast<T>::meta(1, utils::helpers<T>::get_precision(state));
        else
            state *= utils::cast<T>::meta(-1) * this->x * this->x /
                     utils::cast<T>::meta(n * (static_cast<K>(4) * n - static_cast<K>(2)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
