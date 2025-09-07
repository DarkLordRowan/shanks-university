#pragma once

#include "transform/u_transform.h"

template<typename T, typename K, typename series_templ>
levin_sidi_u_recursive_algorithm<T, K, series_templ>::levin_sidi_u_recursive_algorithm(const series_templ &series)
    : levin_sidi_basic_algorithm<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T levin_sidi_u_recursive_algorithm<T, K, series_templ>::operator()(K k, int n) const {
    return this->calculate_recursively(k, n, u_transform<T, K>{});
}
