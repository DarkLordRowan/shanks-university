#pragma once

template<typename T, typename K, typename series_templ>
class chang_whynn_algorithm final: public series_acceleration<T, K, series_templ> {
public:
    explicit chang_whynn_algorithm(const series_templ &series);

    T operator()(K n, int order) const override;
};

#include "./chang_wynn_algorithm.tpp"
