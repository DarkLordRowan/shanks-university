#include <gtest/gtest.h>

#include "../include/series.hpp"
#include "../include/transformations/wynn_rho_algorithm.hpp"

TEST(wynn_rho_algorithm, HapyPath)
{
    using K = unsigned;
    using T = double;

    exp_series<T, K> series{};
    const wynn_rho_algorithm<T, K> algo{};

    constexpr K n = 7;
    constexpr K order = 0;

    const series_result<T> res = series.generate_series(0.5, n + 1);

    const T got = algo(n, order, res);
    const T expected = res.Sn[n];

    std::cout << got << "\n" << expected << std::endl;

    EXPECT_NEAR(got, expected, 1e-3);
}
