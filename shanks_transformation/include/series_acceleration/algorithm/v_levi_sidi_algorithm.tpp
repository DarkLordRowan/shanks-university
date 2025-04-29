#pragma once

#include "transform/v_transform.h"

template<typename T, typename K, typename series_templ>
v_levi_sidi_algorithm<T, K, series_templ>::v_levi_sidi_algorithm(const series_templ &series)
    : levi_sidi_algorithm<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T v_levi_sidi_algorithm<T, K, series_templ>::operator()(K k, int n) const {
    return this->calculate(k, n, v_transform<T, K>{});
}
