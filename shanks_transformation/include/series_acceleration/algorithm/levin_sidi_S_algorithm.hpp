#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class levin_sidi_S_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const T beta;
    const transform_base<T, K> *remainder_func;
    bool recursive;

    virtual T calculate(const K &n, const int &order) const;
    T calculate_rec(const K &n, const int &order) const;

public:
    levin_sidi_S_algorithm(const series_templ &series, const transform_base<T, K> *func, bool recursive = false,
                           const T beta_ = T(1));

    ~levin_sidi_S_algorithm();

    T operator()(const K n, const int order) const;
};

#include "levin_sidi_S_algorithm.tpp"
