#pragma once

#include "transform/t_transform.h"

template<typename T, typename K, typename series_templ>
t_levi_sidi_algorithm<T, K, series_templ>::t_levi_sidi_algorithm(const series_templ &series)
    : levi_sidi_algorithm<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T t_levi_sidi_algorithm<T, K, series_templ>::operator()(K k, int n) const {
    return this->template calculate(k, n, t_transform<T, K>{});
}
