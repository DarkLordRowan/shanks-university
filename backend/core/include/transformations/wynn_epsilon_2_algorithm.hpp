#ifndef WYNN_EPSILON_2_ALGORITHM_HPP
#define WYNN_EPSILON_2_ALGORITHM_HPP
#pragma once
/** Rolling-row Wynn epsilon implementation: https://dlmf.nist.gov/3.9.E11.
 * Return epsilon_(2*order)^(n), including n=0 and order=0.
 * The former circular-buffer parity extraction and unverified singular
 * corrections are removed; singular cells now throw as in implementation 1.
 * Original authors: Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace algos {

template <AcceptedLike T, UnsignedIntLike K>
class wynn_epsilon_2_algorithm final : public series_acceleration<T, K> {
public:

    explicit wynn_epsilon_2_algorithm() : series_acceleration<T, K>("wynn epsilon 2") {};

    T operator()(const K n, const K order, const series_result<T>& data) const override;

    inline std::size_t how_much(const std::size_t n, const std::size_t order) const {
        return n + std::size_t(2) * order + std::size_t(1);
    }
};

template <AcceptedLike T, UnsignedIntLike K>
T wynn_epsilon_2_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Ensure we have enough data points: 2*order + n + 1 terms are required
    const std::size_t required_size = wynn_epsilon_2_algorithm<T, K>::how_much(n, order);
    if (data.Sn.size() < required_size) {
        throw std::out_of_range("Insufficient partial sums for Wynn epsilon");
    }
    if (order == static_cast<K>(0)) return data.Sn.at(n);
    const std::size_t precision = utils::helpers<T>::get_precision(data.Sn[0]);
    const std::size_t count = required_size - static_cast<std::size_t>(n);
    // Wynn recurrence, https://dlmf.nist.gov/3.9#E11.
    // previous is epsilon_-1, current is epsilon_0. Keep the n+1
    // alignment explicitly; n is a zero-based index, including n=0.
    std::vector<T> previous(count + 1, utils::cast<T, int>()(0, precision));
    std::vector<T> current(data.Sn.begin() + n, data.Sn.begin() + required_size);
    for (std::size_t column = 1; column < count; ++column) {
        std::vector<T> next(current.size() - 1);
        for (std::size_t j = 0; j < next.size(); ++j) {
            const T difference = current[j + 1] - current[j];
            if (difference == utils::cast<T, int>()(0, precision))
                throw std::overflow_error("Singular Wynn epsilon table");
            next[j] = previous[j + 1] + utils::cast<T, int>()(1, precision) / difference;
            if (!utils::helpers<T>::isfinite(next[j]))
                throw std::overflow_error("Non-finite Wynn epsilon cell");
        }
        previous = std::move(current);
        current = std::move(next);
    }
    return current[0];
}

}  // namespace algos
}  // namespace shanks

#endif
