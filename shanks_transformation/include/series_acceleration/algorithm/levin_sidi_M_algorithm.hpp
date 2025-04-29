#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class levin_sidi_M_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const T gamma;
    const transform_base<T, K> *remainder_func;

    T calculate(const K &n, const int &order) const;

public:
    levin_sidi_M_algorithm(const series_templ &series, const transform_base<T, K> *func, const T gamma_ = T(10));

    ~levin_sidi_M_algorithm();

    T operator()(const K n, const int order) const;
};

#include "levin_sidi_M_algorithm.tpp"
