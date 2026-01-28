#ifndef FOUR_LN2_M_3_ITERATOR_HPP
#define FOUR_LN2_M_3_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file four_ln2_m_3_iterator.hpp
 * @brief Iterator for the series expansion related to 4*ln(2) - 3.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = x * (4 * ln(2) - 3).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (4 * ln(2) - 3).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class four_ln2_m_3_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for four_ln2_m_3_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit four_ln2_m_3_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * (4 * ln(2) - 3)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (4 * ln(2) - 3).
     */
    T get_sum() const override {
        return this->x * (utils::cast<T>(4) * utils::log(utils::cast<T>(2)) - utils::cast<T>(3));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::isfinite(this->x) || this->x == utils::cast<T>(0); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Term formula based on the specific power series expansion for 4*ln(2)-3
        state = utils::minus_one_raised_to_power_n<T, K>(n + 1) * this->x /
                utils::cast<T>((n + 1) * (n + 1) * (n + 2) * (n + 2));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
