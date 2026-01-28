#ifndef PI_CUBED_32_ITERATOR_HPP
#define PI_CUBED_32_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_cubed_32_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * pi^3 / 32.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi^3 / 32.
 *
 * This class implements an alternating series expansion whose analytic sum equals
 * x * pi^3 / 32.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_cubed_32_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_cubed_32_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_cubed_32_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * pi^3 / 32).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi^3 / 32.
     */
    T get_sum() const override {
        return this->x * utils::pow(utils::cast<T>::meta(std::numbers::pi), utils::cast<T>::meta(3)) / utils::cast<T>::meta(32);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T>::meta(0); }

    /**
     * @brief Computes the next term in the alternating series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ((-1)^n * x / (2n+1)^3).
     */
    T next(K n, T& state) const override {
        // Alternating term with cubic denominator
        state = utils::minus_one_raised_to_power_n<T, K>(n) * this->x /
                utils::cast<T>::meta(
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
