#pragma once

template<typename T, typename K, typename series_templ>
class levin_recursion_algorithm final: public series_acceleration<T, K, series_templ> {
public:
    explicit levin_recursion_algorithm(const series_templ &series, const T beta_ = T(-1.5));

    T operator()(K n, int order) const override;

protected:
        const T beta;
    T operator()(K n_time, K k_time, T b, bool ND) const;
};

#include "levin_recursion_algorithm.tpp"
