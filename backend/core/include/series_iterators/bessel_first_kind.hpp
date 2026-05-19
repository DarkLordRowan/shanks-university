#ifndef BESSEL_FIRST_KIND_HPP
#define BESSEL_FIRST_KIND_HPP
#pragma once

/**
 * @file bessel_first_kind_iterator.hpp
 * @brief
 * https://ru.wikipedia.org/wiki/%D0%A4%D1%83%D0%BD%D0%BA%D1%86%D0%B8%D0%B8_%D0%91%D0%B5%D1%81%D1%81%D0%B5%D0%BB%D1%8F?ysclid=mpcnabtfyq215644600.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the inverse hyperbolic tangent function arctanh(x^2).
 *
 * This class implements the Maclaurin expansion of arctanh(x^2), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class bessel_first_kind_iterator final : public series_base_succ<T, K> {
public:
    using float_type = interval_subtype<typename real_of<T>::value>::value;
    float_type m_alpha;

    /**
     * @brief Default constructor for bessel_first_kind_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit bessel_first_kind_iterator(T x, T alpha) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
        if constexpr (is_complex_t<T>::value || is_complex_interval<T>::value) {
            using sub_type = real_of<T>::value;
            if constexpr (is_interval<sub_type>::value) {
                m_alpha = utils::math<T>::real(alpha).mid();
            } else {
                m_alpha = utils::math<T>::real(alpha);
            }
        } else {
            if constexpr (is_interval<T>::value) {
                m_alpha = alpha.mid();
            } else {
                m_alpha = alpha;
            }
        }
    }

    explicit bessel_first_kind_iterator(T x, K alpha) = delete;

    /**
     * @brief Retrieves the analytic sum of the series (arctanh(x^2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arctanh(x^2).
     */
    T get_sum() const override {
        if constexpr (typename utils::math<T>::has_bessel_first_kind{})
            return utils::math<T>::bessel_first_kind(this->x, this->m_alpha);

        assert(false);
        return utils::helpers<T>::get_nan();
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in bessel_first_kind_iterator Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        if (n == static_cast<K>(0)) {
            state = utils::math<T>::pow(this->x * utils::cast<T, double>()(0.5), utils::cast<T, float_type>()(m_alpha));
            state /= utils::cast<T, float_type>()(utils::math<float_type>::tgamma(m_alpha + 1));
        } else {
            state /= utils::cast<T, K>()(4 * n);
            state /= (utils::cast<T, K>()(n) + utils::cast<T, float_type>()(m_alpha));
            state *= utils::cast<T, int>()(-1) * this->x * this->x;
        }
        return state;
    }
};

}  // namespace series
}  // namespace shanks

#endif