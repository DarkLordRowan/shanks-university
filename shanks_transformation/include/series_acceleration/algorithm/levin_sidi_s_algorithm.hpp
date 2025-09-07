#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class levin_sidi_s_algorithm final : public series_acceleration<T, K, series_templ> {
protected:
    const T beta;
    const transform_base<T, K> *remainder_func;
    bool recursive;

    T calculate(const K &n, const int &order) const;
    T calculate_rec(const K &n, const int &order) const;

public:
    levin_sidi_s_algorithm(const series_templ &series, const transform_base<T, K> *func, bool recursive = false,
                           T beta_ = T(1));

    ~levin_sidi_s_algorithm() override;

    T operator()(K n, int order) const override;
};

#include "levin_sidi_s_algorithm.tpp"
