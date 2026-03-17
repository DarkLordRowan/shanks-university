#ifndef MINUS_ONE_N_FACT_IN_N_ITERATOR_HPP
#define MINUS_ONE_N_FACT_IN_N_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file minus_one_n_fact_n_in_n_iterator.hpp
 * @brief Iterator for the series expansion related to (-1)^n * n! / n^n.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for a specific combinatorial series.
 *
 * This class implements a specific series expansion whose sum is approximately
 * -0.65583160 * x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class minus_one_n_fact_n_in_n_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for minus_one_n_fact_n_in_n_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit minus_one_n_fact_n_in_n_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the approximate sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The analytic sum.
     */
    T get_sum() const override { return this->x * utils::cast<T, double>()(-0.65583160); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the combinatorial series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Formula for the n-th term: (-1)^(n+1) * (n+1)! * x / (n+1)^(n+1)
        state = utils::math<T>::template minus_one_raised_to_power_n<K>(n + 1) * this->x;
        for (K i = 1; i <= n + 1; ++i) state *= utils::cast<T, K>()(i) / utils::cast<T, K>()(n + 1);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
