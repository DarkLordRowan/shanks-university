#pragma once

#include "series/series.h"

template<typename T, typename K>
class u_transform {
    const T beta;

public:
    explicit u_transform(const T beta_ = T(1))
        : beta(beta_) {
    }

    T operator()(const int n, const int j, const series_base<T, K> *series) const {
        return T(1) / ((beta + n) * series->operator()(n + j + 1));
    }
};
