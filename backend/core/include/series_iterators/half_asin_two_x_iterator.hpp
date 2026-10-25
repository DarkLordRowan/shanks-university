#ifndef HALF_ASIN_TWO_X_ITERATOR_HPP
#define HALF_ASIN_TWO_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file half_asin_two_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 0.5 * asin(2x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = 0.5 * asin(2x).
 *
 * This class implements the Maclaurin expansion of 0.5 * asin(2x), which converges
 * for values of x such that |x| <= 0.5.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class half_asin_two_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for half_asin_two_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit half_asin_two_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (0.5 * asin(2x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 0.5 * asin(2x).
     */
    T get_sum() const override {
        return utils::cast<T, double>()(0.5) * utils::math<T>::asin(utils::cast<T, int>()(2) * this->x);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 0.5 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) > utils::cast<float_type, double>()(0.5);
    }

    /**
     * @brief Computes the next term in the 0.5 * asin(2x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // DLMF 4.24.1 under x -> 2x, followed by division by two.
        // https://dlmf.nist.gov/4.24#E1: ratio contains (2n-1)^2.
        if (n == 0)
            state = this->x;
        else {
            const T odd = utils::cast<T, K>()(n) * utils::cast<T, int>()(2) - utils::cast<T, int>()(1);
            state *= utils::cast<T, int>()(2) * this->x * this->x * odd * odd /
                     (utils::cast<T, K>()(n) * (odd + utils::cast<T, int>()(2)));
        }
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
