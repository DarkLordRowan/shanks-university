#pragma once
#define DEF_UNDEFINED_SUM 0
#include "series_acceleration/series_acceleration.hpp"


template<typename T, typename K, typename series_templ>
class shanks_transform : public series_acceleration<T, K, series_templ> {
public:
    shanks_transform(const series_templ &series);
    T operator()(const K n, const int order) const;
};

#include "shanks_transform.tpp"