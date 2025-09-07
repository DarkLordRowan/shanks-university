#pragma once
#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class levin_sidi_basic_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const T beta;

    template<class remainderType>
    T calculate(K k, int n, remainderType remainder_func) const;

    template<class remainderType>
    T calculate_recursively(K k, int n, remainderType remainder_func) const;

public:
    explicit levin_sidi_basic_algorithm(const series_templ &series, T beta_ = T(1));

    T operator()(K k, int n) const override = 0;
};

#include "levin_sidi_basic_algorithm.tpp"
