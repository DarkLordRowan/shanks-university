#ifndef ERF_ITERATOR_HPP
#define ERF_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file erf_iterator.hpp
 * @brief Iterator for the series expansion of the Error Function erf(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the Error Function erf(x).
 *
 * This class implements the Maclaurin expansion of erf(x) (normalized by sqrt(pi)/2),
 * which converges for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class erf_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for erf_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit erf_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (normalized erf(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (sqrt(pi)/2) * erf(x).
     */
    T get_sum() const override {
        if constexpr (typename utils::math<T>::has_erf{})
            return utils::math<T>::sqrt(utils::cast<T, double>()(std::numbers::pi)) * utils::math<T>::erf(this->x) *
                   utils::cast<T, double>()(0.5);

        assert(false);
        return utils::helpers<T>::get_nan();
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the erf(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Standard recurrence relation for the Error Function power series
        if (n == 0)
            state = this->x;
        else
            state *=
                utils::cast<T, int>()(-1) * this->x * this->x *
                utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                                  static_cast<size_t>(1))) /
                utils::cast<T, size_t>()(n * utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                      static_cast<size_t>(1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
