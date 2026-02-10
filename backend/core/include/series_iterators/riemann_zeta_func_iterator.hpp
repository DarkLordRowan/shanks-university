#ifndef RIEMANN_ZETA_FUNC_ITERATOR_HPP
#define RIEMANN_ZETA_FUNC_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file riemann_zeta_func_iterator.hpp
 * @brief Iterator for the Dirichlet series defining the Riemann zeta function zeta(s).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Dirichlet series iterator for the Riemann zeta function zeta(s).
 *
 * This class implements the standard Dirichlet series representation of zeta(s),
 * which converges for complex values of s such that Re(s) > 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for riemann_zeta_func_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit riemann_zeta_func_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (zeta(s)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the Riemann zeta function at point x (s).
     */
    T get_sum() const override {
        if constexpr (typename utils::math<T>::has_zeta{})
            return utils::math<T>::zeta(this->x);
        else
#ifndef DEBUG
            assert(false);
#else
            return utils::helpers<T>::get_nan();
#endif
    }

    /**
     * @brief Validates the current evaluation point x (s).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if Re(x) <= 1 or x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;

        if constexpr (isComplexLike<T>::value) {
            return !utils::helpers<T>::isfinite(this->x) || this->x.real() <= utils::cast<float_type, int>()(1);
        } else {
            return !utils::helpers<T>::isfinite(this->x) || this->x <= utils::cast<T, int>()(1);
        }
    }

    /**
     * @brief Computes the next term in the Dirichlet series for zeta(s).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ((n+1)^(-s)).
     */
    T next(K n, T& state) const override {
        // Term formula: 1 / (n+1)^s
        state = utils::math<T>::pow(utils::cast<T, K>()(n + 1, utils::helpers<T>::get_precision(state)),
                                    utils::cast<T, int>()(-1) * this->x);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
