#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class epsilon_three_algorithm final : public series_acceleration<T, K, series_templ> {
    T epsilon_threshold;

public:
    explicit epsilon_three_algorithm(const series_templ &series, T epsilon_threshold_ = T(1e-3));

    T operator()(K n, int order) const override;
};

#include "epsilon_three_algorithm.tpp"
