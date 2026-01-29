#ifndef XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP
#define XSQUAREPLUS3_DIV_XSQUAREPLUS2MULTIX_MINUS_1_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file xsquareplus3_div_xsquareplus2multix_minus_1_iterator.hpp
 * @brief Iterator for the series expansion of (x^2 + 3) / (x^2 + 2x) - 1.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the rational function f(x) = (x^2 + 3) / (x^2 + 2x) - 1.
 *
 * This class implements a specific series expansion for the given rational function,
 * typically expanded around x=1, which converges for values of x such that |x - 1| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class xsquareplus3_div_xsquareplus2multix_minus_1_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for xsquareplus3_div_xsquareplus2multix_minus_1_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit xsquareplus3_div_xsquareplus2multix_minus_1_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the rational function at current point x.
     */
    T get_sum() const override {
        return (this->x * this->x + utils::cast<T, int>()(3)) / (this->x * this->x + utils::cast<T, int>()(2) * this->x) -
               utils::cast<T, int>()(1);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x - 1| >= 1 or x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x - utils::cast<T, int>()(1)) >= utils::cast<float_type, int>()(1);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Specific term formula for the expansion of the rational function around x=1
        state = utils::cast<T, double>()(0.5) * utils::math<T>::template minus_one_raised_to_power_n<K>(n) *
                utils::cast<T, size_t>()(utils::math<size_t>::pow(size_t{3}, static_cast<size_t>(n + 2)) - 7) *
                utils::math<T>::pow(this->x - utils::cast<T, int>()(1), utils::cast<T, K>()(n)) /
                utils::cast<T, size_t>()(utils::math<size_t>::pow(size_t{3}, static_cast<size_t>(n + 1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
