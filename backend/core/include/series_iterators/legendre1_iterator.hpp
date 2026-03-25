#ifndef LEGENDRE1_ITERATOR_HPP
#define LEGENDRE1_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file legendre1_iterator.hpp
 * @brief Iterator for finding value of airy Ai(x) function through integral and trapezoid method
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Iterator for finding value of airy Ai(x) function through integral and trapezoid method
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class legendre1_iterator final : public series_base_iter<T, K, std::pair<T, T>> {
public:
    /**
     * @brief Default constructor for legendre1_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit legendre1_iterator(T x) : series_base_iter<T, K, std::pair<T, T>>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the sum of Ai(x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arcsin(x).
     */
    T get_sum() const override {
        return utils::math<T>::sqrt((utils::cast<T, int>()(1) - this->x) * utils::cast<T, double>()(0.125));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true non-finite, false otherwise.
     */
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) > utils::cast<typename real_of<T>::value, int>()(1);
    }

    /**
     * @brief Computes the next term in the sequence.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the sequence.
     */
    T next(K n, std::pair<T, T>& state) const override {
        // state->first  : - P_{n-1}(x)
        // state->second : - P_{n}  (x)

        const std::size_t precision = utils::helpers<T>::get_precision(this->x);

        if (n == 0) {
            state = std::make_pair(utils::cast<T, int>()(0), utils::cast<T, int>()(1, precision));
            return state.second / utils::cast<T, int>()(3);
        }
        if (n == 1) {
            state = std::make_pair(state.second, this->x);
            return state.second / utils::cast<T, int>()(-5);
        }

        const T P_n = state.second;
        state.second =
            (utils::cast<T, K>()(2 * n - 1) * this->x * state.second - utils::cast<T, K>()(n - 1) * state.first) /
            utils::cast<T, K>()(n);
        state.first = P_n;
        return -state.second / utils::cast<T, K>()((2 * n - 1) * utils::math<K>::fma(K{2}, n, K{3}));
    }
};

}  // namespace series
}  // namespace shanks
#endif
