#ifndef INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#define INCOMPLETE_GAMMA_FUNC_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file incomplete_Gamma_func_iterator.hpp
 * @brief Iterator for the series expansion of the incomplete Gamma function gamma(alpha, x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the lower incomplete Gamma function gamma(alpha, x).
 *
 * This class implements the power series expansion for the lower incomplete
 * Gamma function, which converges for all finite x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class incomplete_Gamma_func_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for incomplete_Gamma_func_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit incomplete_Gamma_func_iterator(T x, T alpha) : series_base_succ<T, K>(x), alpha(alpha) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    explicit incomplete_Gamma_func_iterator(T x, K alpha) = delete;

    T alpha; /**< The shape parameter alpha of the incomplete Gamma function. */

    /**
     * @brief Retrieves the analytic sum of the series (gamma(alpha, x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the lower incomplete Gamma function.
     */
    T get_sum() const override { 
        if constexpr (typename utils::math<T>::has_inc_gamma{})
        return utils::math<T>::inc_gamma(this->x, alpha); 
        else
        #ifndef DEBUG
        static_assert(dependent_false<T>::value, "utils::math<T>::inc_gamma is not implemented for this type");
        #else
            return utils::helpers<T>::get_nan();
        #endif

    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the incomplete Gamma function expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence relation for the series terms of gamma(alpha, x)
        if (n == 0)
            state = utils::math<T>::pow(this->x, alpha) / alpha;
        else
            state *= utils::cast<T, int>()(-1) * this->x * (alpha + utils::cast<T, K>()(n - 1)) /
                     ((alpha + utils::cast<T, K>()(n)) * utils::cast<T, K>()(n));

        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
