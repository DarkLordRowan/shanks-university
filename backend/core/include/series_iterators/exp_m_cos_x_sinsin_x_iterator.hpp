#ifndef EXP_M_COS_X_SINSINX_ITERATOR_HPP
#define EXP_M_COS_X_SINSINX_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file exp_m_cos_x_sinsin_x_iterator.hpp
 * @brief Iterator for the series expansion of exp(-cos(x))*sin(sin(x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the complex composite function f(x) = exp(-cos(x)) * sin(sin(x)).
 *
 * This class implements a specific series expansion for this composite
 * trigonometric/exponential function.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class exp_m_cos_x_sinsin_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for exp_m_cos_x_sinsin_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit exp_m_cos_x_sinsin_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (exp(-cos(x)) * sin(sin(x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the function at the current point x.
     */
    T get_sum() const override {
        return utils::math<T>::exp(utils::cast<T, int>()(-1) * utils::math<T>::cos(this->x)) *
               utils::math<T>::sin(utils::math<T>::sin(this->x));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // General formula for the n-th term of the specific expansion
        state = utils::math<T>::template minus_one_raised_to_power_n<K>(n) *
                utils::math<T>::sin(utils::cast<T, K>()(n + 1) * this->x) /
                utils::cast<T, K>()(utils::math<K>::fact(n + 1));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
