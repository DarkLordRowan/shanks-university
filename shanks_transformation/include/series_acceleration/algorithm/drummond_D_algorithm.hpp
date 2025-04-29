#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class drummond_D_algorithm final : public series_acceleration<T, K, series_templ> {
protected:
    const transform_base<T, K> *remainder_func;
    bool recursive;

    T calculate(const K &n, const int &order) const;

    T calculate_rec(const K &n, const int &order) const;

public:
    drummond_D_algorithm(const series_templ &series, const transform_base<T, K> *func, bool recursive = false);

    T operator()(K n, int order) const override;
};

#include "drummond_D_algorithm.tpp"
