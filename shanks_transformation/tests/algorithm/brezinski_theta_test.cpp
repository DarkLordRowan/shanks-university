#include <gtest/gtest.h>

#include <math.h>
#include "libs/arbitrary_arithmetics/precisioncore.cpp"

#include "series.hpp"
#include "methods/algorithm/brezinski_theta_algorithm.hpp"

TEST(brezinski_theta_algorithm, HapyPath)
{
    using K = unsigned;
    using T = double;

    const exp_series<T, K> series(0.5);
    using SeriesTempl = const series_base<T, K>*;
    const SeriesTempl p = &series;
    const brezinski_theta_algorithm<T, K, SeriesTempl> algo(p);

    constexpr K n = 6;
    constexpr K order = 0;

    const T got = algo(n, order);
    const T expected = series.S_n(n);

    std::cout << got << "\n" << expected << std::endl;

    EXPECT_NEAR(got, expected, 1e-3);
}
