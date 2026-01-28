#ifndef BIN_ITERATOR_HPP
#define BIN_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file bin_iterator.hpp
 * @brief Iterator for the binomial series (1+x)^alpha.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

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
template <AcceptedLike T, UnsignedIntLike K>
class bin_iterator final : public series_base_succ<T, K> {
public:
    T alpha = utils::cast<T>::meta(0.0); /**< The exponent alpha in the binomial expansion. */

    /**
     * @brief Default constructor for bin_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit bin_iterator(T x, T alpha) : series_base_succ<T, K>(x), alpha(alpha) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    explicit bin_iterator(T x, K alpha) = delete;

    /**
     * @brief Retrieves the analytic sum of the series ((1+x)^alpha).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (1+x)^alpha.
     */
    T get_sum() const override { return utils::pow(utils::cast<T>::meta(1.0) + this->x, alpha); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x) > utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the binomial series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // First term is always 1.0, subsequent terms use the binomial recurrence
        if (n == 0)
            state = utils::cast<T>::meta(1.0, utils::helpers<T>::get_precision(state));
        else
            state *= (this->alpha - utils::cast<T>::meta(n - static_cast<K>(1))) * this->x / utils::cast<T>::meta(n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
