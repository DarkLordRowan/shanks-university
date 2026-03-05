#ifndef RUMP_SEQ1_ITERATOR_HPP
#define RUMP_SEQ1_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file rump_seq1_iterator.hpp
 * @brief Iterator for the sequence x_{n+1} = 6496 - (4205*2^10+609725*2^15/x_{n-1})/x_n with initial values x_0 =
 * -1305, x_1 = -1440
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * "On recurrences converging to the wrong limit in fnite precision and some new examples" Table 1  limit = 0
 */

namespace shanks {
namespace series {

/**
 * @brief Iterator for the sequence x_{n+1} = 6496 - (4205*2^10+609725*x^15/x_{n-1})/x_n with initial values x_0 =
 * -1305, x_1 = -1440
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class rump_seq1_iterator final : public series_base_seq_iter<T, K, std::pair<T, T>> {
public:
    /**
     * @brief Default constructor for rump_seq1_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit rump_seq1_iterator(T x) : series_base_seq_iter<T, K, std::pair<T, T>>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves limit for the series x_{n+1} = 6496 - (4205*2^10+609725*x^15/x_{n-1})/x_n with initial values
     * x_0 = -1305, x_1 = -1440
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The limit of the sequence.
     */
    T get_sum() const override { return utils::cast<T, int>()(0); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return always valid
     */
    bool is_invalid() const override { return false; }

    std::pair<T, T> initial_state() const override {
        return std::make_pair(utils::cast<T, int>()(0, utils::helpers<T>::get_precision(this->x)),
                              utils::cast<T, int>()(0, utils::helpers<T>::get_precision(this->x)));
    }

    /**
     * @brief Computes the next term in the sequence.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the sequence.
     */
    T next(K n, std::pair<T, T>& state) const override {
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        if (n == 0)
            state = std::make_pair(utils::cast<T, int>()(0, precision), utils::cast<T, int>()(-1305, precision));
        else if (n == 1)
            state = std::make_pair(utils::cast<T, int>()(-1305, precision), utils::cast<T, int>()(-1440, precision));
        else {
            state.first = utils::cast<T, int>()(6496, precision) -
                          (utils::cast<T, unsigned long long int>()(4205ull * (2 << 9), precision) +
                           utils::cast<T, unsigned long long int>()(609725ull * (2 << 14), precision) / state.first) /
                              state.second;
            std::swap(state.first, state.second);
        }
        return state.second;
    }
};

}  // namespace series
}  // namespace shanks

#endif