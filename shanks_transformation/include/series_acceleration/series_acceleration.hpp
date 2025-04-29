#pragma once

#include <functional>
#include <iostream>

/**
 * @brief Base class series_acceleration
 * @tparam T Type of the series elements
 * @tparam K Type of index
 * @tparam series_templ Type of the series
 */
template<typename T, typename K, typename series_templ>
class series_acceleration {
public:
    virtual ~series_acceleration() = default;

    explicit series_acceleration(const series_templ &series);

    constexpr void print_info() const;

    virtual T operator()(K n, int order) const = 0;

protected:
    series_templ series;
};

#include "series_acceleration.tpp"
