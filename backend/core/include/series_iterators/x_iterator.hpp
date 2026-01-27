#ifndef X_ITERATOR_HPP
#define X_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file x_iterator.hpp
 * @brief Iterator for the Fourier series expansion of the identity function f(x) = x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the identity function f(x) = x.
 *
 * This class implements the Fourier sine expansion for the identity function,
 * which converges for values of x such that |x| < pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x.
     */
    T get_sum() const override { return this->x; }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= pi or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
    }

    /**
     * @brief Computes the next term in the Fourier expansion of x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (2 * (-1)^n * sin((n+1)x) / (n+1)).
     */
    T next(K n, T& state) const override {
        // Standard Fourier sine expansion for the sawtooth wave (identity on (-pi, pi))
        state = utils::cast<T>(2) * utils::minus_one_raised_to_power_n<T, K>(n) / utils::cast<T>(n + 1) *
                utils::sin(utils::cast<T>(n + 1) * this->x);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
