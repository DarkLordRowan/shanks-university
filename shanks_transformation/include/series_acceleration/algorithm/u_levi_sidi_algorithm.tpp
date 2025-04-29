#pragma once

#include "transform/u_transform.h"

template<typename T, typename K, typename series_templ>
u_levi_sidi_algorithm<T, K, series_templ>::u_levi_sidi_algorithm(const series_templ &series)
    : levi_sidi_algorithm<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T u_levi_sidi_algorithm<T, K, series_templ>::operator()(K k, int n) const {
    return this->calculate(k, n, u_transform<T, K>{});
}
