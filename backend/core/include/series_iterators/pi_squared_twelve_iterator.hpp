#ifndef PI_SQUARED_TWELVE_ITERATOR_HPP
#define PI_SQUARED_TWELVE_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_squared_twelve_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * pi^2 / 12.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi^2 / 12.
 *
 * This class implements an alternating series expansion related to the Dirichlet
 * eta function eta(2) whose sum is x * pi^2 / 12.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_squared_twelve_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_squared_twelve_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_squared_twelve_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi^2 / 12.
     */
    T get_sum() const override {
        return this->x * utils::cast<T, double>()(std::numbers::pi) * utils::cast<T, double>()(std::numbers::pi) /
               utils::cast<T, int>()(12);
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
     * @brief Computes the next term in the alternating series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ((-1)^n * x / (n+1)^2).
     */
    T next(K n, T& state) const override {
        // Alternating term with quadratic denominator
        state = this->x * utils::math<T>::template minus_one_raised_to_power_n<K>(n) / utils::cast<T, K>()((n + 1) * (n + 1));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
