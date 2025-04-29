#pragma once

#include "series/series.h"

class v_transform {
public:
    v_transform() {
    }

    template<typename T, typename K>
    T operator()(const int n, const int j, const series_base<T, K> *series) const {
        return (series->operator()(n + j + 1) - series->operator()(n + j)) /
               (series->operator()(n + j + 1) * series->operator()(n + j));
    }
};
