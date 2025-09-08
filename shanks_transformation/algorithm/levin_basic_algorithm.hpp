#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class levin_basic_algorithm final: public series_acceleration<T, K, series_templ> {
public:
    explicit levin_basic_algorithm(const series_templ &series);

    T operator()(K n, int order) const override;
};

#include "levin_basic_algorithm.tpp"
