#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class brezinski_theta_algorithm final : public series_acceleration<T, K, series_templ> {
public:
    explicit brezinski_theta_algorithm(const series_templ &series);

    T operator()(K n, int order) const override;

protected:
    T theta(K n, int order, T S_n, K j) const;
};

#include "brezinski_theta_algorithm.tpp"
