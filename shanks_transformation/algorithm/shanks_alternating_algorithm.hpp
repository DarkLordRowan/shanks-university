#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class shanks_alternating_algorithm final : public series_acceleration<T, K, series_templ> {
public:
    explicit shanks_alternating_algorithm(const series_templ &series);
    T operator()(K n, int order) const override;
};

#include "shanks_alternating_algorithm.tpp"
