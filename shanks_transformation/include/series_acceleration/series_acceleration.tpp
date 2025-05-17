#pragma once
#include <iostream>

template<typename T, typename K, typename series_templ>
series_acceleration<T, K, series_templ>::series_acceleration(const series_templ &series)
    : series(series) {
}

template<typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_info() const {
    std::cout << "transformation: " << typeid(*this).name() << '\n';
}
