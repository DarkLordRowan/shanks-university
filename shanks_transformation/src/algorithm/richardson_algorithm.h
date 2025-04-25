#pragma once
#define DEF_UNDEFINED_SUM 0

#include "../series_acceleration.h"
#include <vector>

template<typename T, typename K, typename series_templ>
class richardson_algorithm : public series_acceleration<T, K, series_templ> {
public:
    richardson_algorithm(const series_templ &series);
    T operator()(const K n, const int order) const;
};

template<typename T, typename K, typename series_templ>
richardson_algorithm<T, K,
    series_templ>::richardson_algorithm(const series_templ &series) : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T richardson_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    // in the method we don't use order, it's only a stub 
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    if (n == 0)
        return DEF_UNDEFINED_SUM;

    std::vector<std::vector<T> > e(2, std::vector<T>(n + 1, 0));
    //2 vectors n + 1 length containing Richardson table next and previous

    for (int i = 0; i <= n; ++i)
        e[0][i] = this->series->S_n(i);

    // The Richardson method main function 
    T a, b;
    a = static_cast<T>(1);

    for (int l = 1; l <= n; ++l) {
        a *= 4;
        b = a - 1;
        for (int m = l; m <= n; ++m)
            e[1][m] = (a * e[0][m] - e[0][m - 1]) / b;

        std::swap(e[0], e[1]);
    }

    // n & 1 gives first bit 

    const T res = e[n & 1][n]; // get n & 1, cause if n is even, result is e[0][n], if n is odd, result is e[1][n]

    if (!std::isfinite(res))
        throw std::overflow_error("division by zero");

    return res;
}
