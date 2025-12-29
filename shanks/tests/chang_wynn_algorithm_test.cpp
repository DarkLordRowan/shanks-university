#include <gtest/gtest.h>

#include "../include/series.hpp"
#include "../include/transformations/chang_wynn_algorithm.hpp"

TEST(chang_wynn_algorithm, HapyPath)
{
    using K = unsigned;
    using T = double;

    exp_series<T, K> series{};
    const chang_wynn_algorithm<T, K> algo{};

    constexpr K n = 7;
    constexpr K order = 0;

    const series_result<T> res = series.generate_series(0.5, n + 2);

    const T got = algo(n, order, res);
    const T expected = res.Sn[n];

    std::cout << got << "\n" << expected << std::endl;

    EXPECT_NEAR(got, expected, 1e-3);
}
