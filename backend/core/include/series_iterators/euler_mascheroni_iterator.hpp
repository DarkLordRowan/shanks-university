#ifndef EULER_MASHERONI_ITERATOR_HPP
#define EULER_MASHERONI_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file euler_mascheroni_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * gamma.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @cite https://www.mi.sanu.ac.rs/~gvm/radovi/GVM-PROC-MICOPAM2022.pdf
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * gamma.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x * gamma.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class euler_mascheroni_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for euler_mascheroni_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit euler_mascheroni_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * gamma).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi.
     */
    T get_sum() const override { return this->x * utils::cast<T, long double>()(std::numbers::egamma_v<long double>); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T, int>()(0);
    }

    /**
     * @brief Computes the next term in the series expansion for x * gamma.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Formula for the n-th term of the specific expansion
        const std::size_t precision = utils::helpers<T>::get_precision(this->x);

        const T n_plus_one_inv = utils::cast<T, int>()(1, precision) / utils::cast<T, K>()(n + 1);

        state = n_plus_one_inv - utils::math<T>::log(utils::cast<T, int>()(1, precision) + n_plus_one_inv);

        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
