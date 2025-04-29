#pragma once

#include "transform/t_transform.h"

template<typename T, typename K, typename series_templ>
recursive_t_levi_sidi_algorithm<T, K, series_templ>::recursive_t_levi_sidi_algorithm(const series_templ &series)
    : levi_sidi_algorithm<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T recursive_t_levi_sidi_algorithm<T, K, series_templ>::operator()(K k, int n) const {
    return this->calculate_recursively(k, n, t_transform{});
}
