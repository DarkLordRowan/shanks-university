#ifndef RUMP_SEQ3_ITERATOR_HPP
#define RUMP_SEQ3_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file rump_seq3_iterator.hpp
 * @brief Iterator for the sequence x_{n+1} = 111 - (1130 - 3000/x_{n-1})/x_{n} with initial values x_0 = 11/2, x_1 =
 * 61/11
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * On recurrences converging to the wrong limit infnite precision and some new examples (2), limit = 6
 */

namespace shanks {
namespace series {

/**
 * @brief Iterator for the sequence x_{n+1} = 111 - (1130 - 3000/x_{n-1})/x_{n} with initial values x_0 = 11/2, x_1 =
 * 61/11
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 * On recurrences converging to the wrong limit infnite precision and some new examples (14), limit = 1.6666667
 */
template <AcceptedLike T, UnsignedIntLike K>
class rump_seq3_iterator final : public series_base_seq_iter<T, K, std::pair<T, T>> {
public:
    /**
     * @brief Default constructor for rump_seq3_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit rump_seq3_iterator(T x) : series_base_seq_iter<T, K, std::pair<T, T>>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves limit for the series x_{n+1} = 111 - (1130 - 3000/x_{n-1})/x_{n} with initial values x_0 = 11/2,
     * x_1 = 61/11
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The limit of the sequence.
     */
    T get_sum() const override {
        // Reference is for exact unperturbed initial data, not a rounded fixed point.
        // Rump, ETNA 52 (2020): https://doi.org/10.1553/etna_vol52s358
        if(this->x!=utils::cast<T,int>()(0)) throw std::domain_error("Reference unavailable for perturbed initial data");
        return utils::cast<T,int>()(6,utils::helpers<T>::get_precision(this->x));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return always valid
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    std::pair<T, T> initial_state() const override {
        return std::make_pair(utils::cast<T, int>()(0, utils::helpers<T>::get_precision(this->x)),
                              utils::cast<T, int>()(0, utils::helpers<T>::get_precision(this->x)));
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, std::pair<T, T>& state) const override {
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        if (n == 0)
            state =
                std::make_pair(utils::cast<T, int>()(0, precision), this->x + utils::cast<T, double>()(5.5, precision));
        else if (n == 1)
            state = std::make_pair(this->x + utils::cast<T, double>()(5.5, precision),
                                   (utils::cast<T,int>()(61,precision)/utils::cast<T,int>()(11,precision)));
        else {
            state.first =
                utils::cast<T, int>()(111, precision) -
                (utils::cast<T, int>()(1130, precision) - utils::cast<T, int>()(3000, precision) / state.first) /
                    state.second;
            std::swap(state.first, state.second);
        }
        return state.second;
    }
};

}  // namespace series
}  // namespace shanks

#endif