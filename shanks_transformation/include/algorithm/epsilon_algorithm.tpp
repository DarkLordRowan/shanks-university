#pragma once

template<typename T, typename K, typename series_templ>
epsilon_algorithm<T, K, series_templ>::epsilon_algorithm(const series_templ &series)
    : series_acceleration<T, K, series_templ>(series) {

}

template<typename T, typename K, typename series_templ>
T epsilon_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    int m = 2 * order;
    if (n <= 0)
        throw std::domain_error("negative integer in the input");

    if (order == 0)
        return this->series->S_n(n);

    K max_ind = m + n; // int -> K mark

    std::vector<T> e1(max_ind, 0);
    std::vector<T> e0(max_ind + 1, 0);

    auto e0_add = &e0; // Pointer for vector swapping
    auto e1_add = &e1; // Pointer for vector swapping

    for (int i = 0; i < m; ++i) {
        for (K j = n - 1; j < max_ind; ++j)
            (*e1_add)[j] += static_cast<T>(1.0 / ((*e0_add)[j + 1] - (*e0_add)[j]));

        --max_ind;
        std::swap(e0_add, e1_add);
        (*e1_add).erase((*e1_add).begin());
    }

    const auto result = (*e0_add)[n - 1];

    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}