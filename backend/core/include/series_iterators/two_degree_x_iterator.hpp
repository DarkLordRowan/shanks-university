#ifndef TWO_DEGREE_X_ITERATOR_HPP
#define TWO_DEGREE_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file two_degree_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 2^x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the exponential function f(x) = 2^x.
 *
 * This class implements the Maclaurin expansion of 2^x, which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class two_degree_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for two_degree_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit two_degree_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (2^x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 2^x.
     */
    T get_sum() const override { return utils::pow(utils::cast<T>::meta(2), this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the 2^x Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Each term is (x * ln(2))^n / n!, computed recursively
        if (n == 0)
            state = utils::cast<T>::meta(1, utils::helpers<T>::get_precision(state));
        else
            state *= this->x * utils::math<T>::log(utils::cast<T>::meta(2)) / utils::cast<T>::meta(n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
