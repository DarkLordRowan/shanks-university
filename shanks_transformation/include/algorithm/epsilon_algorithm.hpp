#pragma once

template<typename T, typename K, typename series_templ>
class epsilon_algorithm final: public series_acceleration<T, K, series_templ> {
public:
    explicit epsilon_algorithm(const series_templ &series);

    T operator()(K n, int order) const override;
};

#include "epsilon_algorithm.tpp"
