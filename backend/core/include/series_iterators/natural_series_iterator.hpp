#ifndef NATURAL_SERIES__ITERATOR_HPP
#define NATURAL_SERIES__ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

// Ordinary terms a_n=n. The partial sums diverge; zeta(-1)=-1/12 is
// available only through get_regularized_sum(), never as an ordinary sum.
// Reference: https://dlmf.nist.gov/25.6
namespace shanks {
namespace series {

template <AcceptedLike T, UnsignedIntLike K>
class natural_series_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Construct the natural-number series (legacy x is unused).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit natural_series_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Reject an ordinary sum for this divergent series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @throws std::domain_error Always.
     */
    T get_sum() const override {
        throw std::domain_error("Ordinary natural series diverges; -1/12 is a regularized value");
    }

    T get_regularized_sum() const {
        const auto p=utils::helpers<T>::get_precision(this->x);
        return utils::cast<T,int>()(-1,p)/utils::cast<T,int>()(12,p);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // a_n = n, independent of the legacy argument x.
        state = utils::cast<T, K>()(n, utils::helpers<T>::get_precision(this->x));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
