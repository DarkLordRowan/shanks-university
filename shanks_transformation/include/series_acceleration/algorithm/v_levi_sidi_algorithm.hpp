#pragma once

#include "levi_sidi_algorithm.hpp"

template<typename T, typename K, typename series_templ>
class v_levi_sidi_algorithm final : public levi_sidi_algorithm<T, K, series_templ> {
public:
    explicit v_levi_sidi_algorithm(const series_templ &series);

    T operator()(K k, int n) const override;
};

#include "v_levi_sidi_algorithm.tpp"
