#pragma once
class t_transform {
public:
    t_transform() {
    }

    template<typename T, typename K>
    T operator()(const int &n, const int &j, const series_base<T, K> *series) const {
        return T(1) / series->operator()(n + j);
    }
};
