#ifndef RIEMANN_ZETA_FUNC_XMIN1_DIV_RIEMANN_ZETA_FUNC_X_ITERATOR_HPP
#define RIEMANN_ZETA_FUNC_XMIN1_DIV_RIEMANN_ZETA_FUNC_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator.hpp
 * @brief Iterator for the series representation of zeta(s-1) / zeta(s).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Dirichlet series iterator for the ratio of Riemann zeta functions zeta(s-1) / zeta(s).
 *
 * This class implements the Dirichlet series expansion for the ratio of zeta functions,
 * which involves the Euler totient function phi(n). It converges for values of s such
 * that Re(s) > 2.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (zeta(s-1) / zeta(s)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the ratio zeta(s-1) / zeta(s).
     */
    T get_sum() const override { return utils::math<T>::zeta(this->x - utils::cast<T, int>()(1)) / utils::math<T>::zeta(this->x); }

    /**
     * @brief Validates the current evaluation point x (s).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if Re(x) <= 2 or x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;

        if constexpr (isComplexLike<T>::value) {
            return !utils::helpers<T>::isfinite(this->x) || this->x.real() <= utils::cast<float_type, int>()(2);
        } else {
            return !utils::helpers<T>::isfinite(this->x) || this->x <= utils::cast<T, int>()(2);
        }
    }

    /**
     * @brief Computes the next term in the Dirichlet series for zeta(s-1)/zeta(s).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (phi(n+1) / (n+1)^s).
     */
    T next(K n, T& state) const override {
        // Dirichlet series term involving Euler's totient function
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        state = utils::cast<T, K>()(utils::math<K>::phi(n + 1), precision) /
                utils::math<T>::pow(utils::cast<T, K>()(n + 1, precision), this->x);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
