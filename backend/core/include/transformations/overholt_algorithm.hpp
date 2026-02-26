#ifndef OVERHOLT_ALGORITHM_HPP
#define OVERHOLT_ALGORITHM_HPP
#pragma once

namespace shanks {
namespace algos {


template <AcceptedLike T, UnsignedIntLike K>
class overholt_algorithm final : public series_acceleration<T, K> {
protected:

    using float_type = real_of<T>::value;  // type in case of complex or interval

public:

    explicit overholt_algorithm() : series_acceleration<T, K>("overholt process") {}
    ~overholt_algorithm() = default;  // Default destructor is sufficient since unique_ptr handles deletion


    T operator()(const K n, const K order, const series_result<T>& data) const override;

};

template <AcceptedLike T, UnsignedIntLike K>
T overholt_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {

    if (data.Sn.size() < n + order + static_cast<K>(1) || data.an.size() < n + 2 * order + static_cast<K>(1)) {
        throw std::out_of_range("The Sn smaller then required for V_{" + utils::helpers<K>::to_string(order) + "}^{" +
                                utils::helpers<K>::to_string(n) + "}\n" + "the size of Sn must be at least " +
                                utils::helpers<K>::to_string(n + order + static_cast<K>(1)) + "and size of an - " + utils::helpers<K>::to_string(n + order + static_cast<K>(1)));
    }

    const size_t precision = std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    std::vector<T> V(order+1, utils::cast<T, int>()(0, precision));
    for(K i{0}; i <= order; ++i) V[i] += data.Sn.at(n + i);


    for(K i{1}; i <= order; ++i){
        for(K j{0}; j <= order - i; ++j){

            const T delta_left_s  = utils::math<T>::pow(data.an.at(n + i + j), utils::cast<T,int>()(i, precision));      /// %delta{s}_{n + k - 1}
            const T delta_right_s = utils::math<T>::pow(data.an.at(n + i + j + 1), utils::cast<T,int>()(i, precision));  /// %delta{s}_{n + k}
            const T tmp = ( delta_left_s * V[j+1] -  delta_right_s* V[j]) / (delta_left_s - delta_right_s);

            V[j] = (utils::helpers<T>::isfinite(tmp) ? tmp : V[j]);

        }
    }

    // Numerical stability check
    if (!utils::helpers<T>::isfinite(V[0])) throw std::overflow_error("division by zero");

    return V[0];

}

}  // namespace algos
}  // namespace shanks

#endif
