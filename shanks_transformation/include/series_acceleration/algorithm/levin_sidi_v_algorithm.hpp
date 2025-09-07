#pragma once

#include "levin_sidi_basic_algorithm.hpp"

template<typename T, typename K, typename series_templ>
class levin_sidi_v_algorithm final : public levin_sidi_basic_algorithm<T, K, series_templ> {
public:
    explicit levin_sidi_v_algorithm(const series_templ &series);

    T operator()(K k, int n) const override;
};

#include "levin_sidi_v_algorithm.tpp"
