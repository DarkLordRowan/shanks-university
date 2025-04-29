#pragma once

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class shanks_transform_alternating final : public series_acceleration<T, K, series_templ> {
public:
    explicit shanks_transform_alternating(const series_templ &series);
    T operator()(K n, int order) const override;
};

#include "shanks_transform_alternating.tpp"