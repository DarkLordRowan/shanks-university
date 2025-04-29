#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class ford_sidi_two_algorithm final: public series_acceleration<T, K, series_templ> {
public:
    explicit ford_sidi_two_algorithm(const series_templ &series);

    T operator()(K n, int order) const override;
};

#include "ford_sidi_two_algorithm.tpp"
