#ifndef COSH_ITERATOR_HPP
#define COSH_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file cosh_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the hyperbolic cosine cosh(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the hyperbolic cosine function cosh(x).
 *
 * This class implements the Maclaurin expansion of cosh(x), which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class cosh_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for cosh_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit cosh_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (cosh(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of cosh(x).
     */
    T get_sum() const override { return utils::cosh(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the cosh(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence logic for the even-power terms of the cosh expansion
        if (n == 0)
            state = utils::cast<T>(1, utils::get_precision(state));
        else
            state *= this->x * this->x / utils::cast<T>(n * (size_t{4} * n - size_t{2}));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
