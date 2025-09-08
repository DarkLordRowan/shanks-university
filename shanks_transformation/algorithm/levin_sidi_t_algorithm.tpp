#pragma once

#include "transform/t_transform.h"

template<typename T, typename K, typename series_templ>
levin_sidi_t_algorithm<T, K, series_templ>::levin_sidi_t_algorithm(const series_templ &series)
    : levin_sidi_basic_algorithm<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T levin_sidi_t_algorithm<T, K, series_templ>::operator()(K k, int n) const {
    return this->calculate(k, n, t_transform<T, K>{});
}
