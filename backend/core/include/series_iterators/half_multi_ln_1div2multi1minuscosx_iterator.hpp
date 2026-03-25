#ifndef HALF_MULTI_LN_1DIV2MULTI1MISUSCOSX_ITERATOR_HPP
#define HALF_MULTI_LN_1DIV2MULTI1MISUSCOSX_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file half_multi_ln_1div2multi1minuscosx_iterator.hpp
 * @brief Iterator for the series expansion of -0.5 * ln(2 - 2*cos(x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the function f(x) = -0.5 * ln(2 - 2*cos(x)).
 *
 * This class implements the Fourier series for the logarithmic function, which
 * converges for all x except multiples of 2*pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class half_multi_ln_1div2multi1minuscosx_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for half_multi_ln_1div2multi1minuscosx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit half_multi_ln_1div2multi1minuscosx_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (-0.5 * ln(2 - 2*cos(x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of -0.5 * ln(2 - 2*cos(x)).
     */
    T get_sum() const override {
        return utils::cast<T, double>()(-0.5) *
               utils::math<T>::log(utils::cast<T, int>()(2) - utils::cast<T, int>()(2) * utils::math<T>::cos(this->x));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= pi or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) >= utils::cast<float_type, double>()(std::numbers::pi);
    }

    /**
     * @brief Computes the next term in the Fourier expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Fourier series term: cos((n+1)x) / (n+1)
        state = utils::math<T>::cos(utils::cast<T, K>()(n + 1) * this->x) / utils::cast<T, K>()(n + 1);

        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
