#pragma once

#include "transform/v_transform.h"

template<typename T, typename K, typename series_templ>
levin_sidi_v_recursive_algorithm<T, K, series_templ>::levin_sidi_v_recursive_algorithm(const series_templ &series)
    : levin_sidi_basic_algorithm<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T levin_sidi_v_recursive_algorithm<T, K, series_templ>::operator()(K k, int n) const {
    return this->calculate_recursively(k, n, v_transform<T, K>{});
}
