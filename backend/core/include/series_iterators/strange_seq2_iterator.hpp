#ifndef STRANGE_SEQ2_ITERATOR_HPP
#define STRANGE_SEQ2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file strange_seq2_iterator.hpp
 * @brief Iterator for the series x_{n+1} = -256 + 131072/x_{n-1}/x_n with initial values x_0 = 3, x_1 = 170
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Iterator for the series x_{n+1} = -256 + 131072/x_{n-1}/x_n with initial values x_0 = 3, x_1 = 170
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class strange_seq2_iterator final : public series_base_iter<T, K, std::pair<T, T>> {
private:
    T prev_state;
public:
    /**
     * @brief Default constructor for strange_seq2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit strange_seq2_iterator(T x) : series_base_iter<T, K, std::pair<T, T>>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of sqrt((1 - sqrt(1 - x)) / x).
     */
    T get_sum() const override {
        return utils::cast<T>(0.0);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1, x is zero, or x is non-finite; false otherwise.
     */
    bool is_invalid() const override {
        return false;
    }

    std::pair<T, T> initial_state() const override { 
        return std::make_pair(
            utils::cast<T>(3, utils::get_precision(series_base_iter<T, K, std::pair<T,T>>::x)),
            utils::cast<T>(170, utils::get_precision(series_base_iter<T, K, std::pair<T,T>>::x))
        ); 
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, std::pair<T, T>& state) const override {
        const size_t precision = utils::get_precision(state.first);
        state.first = utils::cast<T>(-256, precision) + utils::cast<T>(131072, precision) / state.first / state.second;
        std::swap(state.first, state.second);
        return state.second;
    }
};

}  // namespace series
}  // namespace shanks

#endif