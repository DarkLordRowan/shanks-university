#pragma once

#include <vector>

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class ford_sidi_three_algorithm final : public series_acceleration<T, K, series_templ> {
public:
    explicit ford_sidi_three_algorithm(const series_templ &series);

    T operator()(K n, int order) const override;

protected:
    mutable std::vector<std::vector<T> > FSG; // mutable allows modification in const methods
    mutable std::vector<T> FSA;
    mutable std::vector<T> FSI;
    mutable std::vector<T> G;
};

#include "ford_sidi_three_algorithm.tpp"
