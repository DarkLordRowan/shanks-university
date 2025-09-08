#pragma once
#define DEF_UNDEFINED_SUM 0
#include "series_acceleration/series_acceleration.hpp"


template<typename T, typename K, typename series_templ>
class shanks_basic_algorithm final : public series_acceleration<T, K, series_templ> {
public:
    explicit shanks_basic_algorithm(const series_templ &series);
    T operator()(K n, int order) const override;
};

#include "shanks_basic_algorithm.tpp"
