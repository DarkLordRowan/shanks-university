#include <gtest/gtest.h>

#include "../include/series.hpp"
#include "../include/transformations/ford_sidi_2_algorithm.hpp"

TEST(ford_sidi_2_algorithm, HapyPath)
{
    using K = unsigned;
    using T = double;

    exp_series<T, K> series{};
    const ford_sidi_2_algorithm<T, K> algo{};

    constexpr K n = 7;
    constexpr K order = 0;

    const series_result<T> res = series.generate_series(0.5, n + 3);

    const T got = algo(n, order, res);
    const T expected = res.Sn[n];

    std::cout << got << "\n" << expected << std::endl;

    EXPECT_NEAR(got, expected, 1e-3);
}
