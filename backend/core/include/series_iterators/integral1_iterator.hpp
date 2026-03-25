#ifndef INTEGRAL1_ITERATOR_HPP
#define INTEGRAL1_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file integral1_iterator.hpp
 * @brief Iterator for the integral int_{0}^{1}ln{x}/(1-x)dx = pi^2/12
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Integral int_{0}^{1}ln{x}/(1-x)dx = pi^2/12 by trapezoid method
 *
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class integral1_iterator final : public series_base_succ<T, K> {
public:
    const K steps;

    /**
     * @brief Default constructor for integral1_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit integral1_iterator(T x, K steps) : series_base_succ<T, K>(x), steps(steps) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    explicit integral1_iterator(T x, T steps) = delete;

    /**
     * @brief Retrieves the analytic sum of the series (1 / (1 - x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 1 / (1 - x).
     */
    T get_sum() const override {
        const std::size_t precision = utils::helpers<T>::get_precision(this->x);
        return utils::cast<T, double>()(-std::numbers::pi, precision) *
               utils::cast<T, double>()(std::numbers::pi, precision) / utils::cast<T, int>()(12);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes integral int_{0}^{1}ln{x}/(1-x)dx = pi^2/12 by trapezoid method (next term in series)
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (x^n).
     */
    T next(K n, [[maybe_unused]] T& state) const override {
        if (n + 1 >= steps) return utils::cast<T, int>()(0);

        const std::size_t precision = utils::helpers<T>::get_precision(this->x);
        const T step = utils::cast<T, int>()(1, precision) / utils::cast<T, K>()(steps);

        constexpr auto f = [](const T& x) {
            if (x == utils::cast<T, int>()(0)) return utils::cast<T, int>()(0);
            return utils::math<T>::log(x) / (utils::cast<T, int>()(1) + x);
        };

        const T a = step * utils::cast<T, int>()(n);
        const T b = a + step;

        return step * (f(a) + f(b)) * utils::cast<T, double>()(0.5);
    }
};

}  // namespace series
}  // namespace shanks
#endif
