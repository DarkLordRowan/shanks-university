#pragma once

#include <functional>
#include <iostream>


/**
 * @brief Base class series_acceleration
 * It is not used on it's own, but it is inherited by shanks_transformation and epsilon_algorithm to implement the corresponding methods.
 * The class implementation provides everything needed for construction of an arbitrary series up to n terms and printing out the partial sum,
 * the partial sum after transformation is used, and the difference between the latter and the former.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template<typename T, typename K, typename series_templ>
class series_acceleration {
public:
    virtual ~series_acceleration() = default;

    explicit series_acceleration(const series_templ &series);

    constexpr void print_info() const;

    /**
   * @brief Virtual operator() that returns the partial sum after transformation of the series
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order of the transformation
   * @return The transformed partial sum
   */
    virtual T operator()(K n, int order) const = 0;

protected:
    series_templ series;
};

template<typename T, typename K, typename series_templ>
series_acceleration<T, K, series_templ>::series_acceleration(const series_templ &series)
    : series(series) {
}

template<typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_info() const {
    std::cout << "transformation: " << typeid(*this).name() << '\n';
}
