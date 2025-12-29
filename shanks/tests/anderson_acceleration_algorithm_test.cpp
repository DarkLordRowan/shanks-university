#include <gtest/gtest.h>

#include <math.h>
#include "../libs/arbitrary_arithmetics/precisioncore.cpp"

#include "../include/series.hpp"
#include "../include/transformations/anderson_acceleration_algorithm.hpp"

TEST(anderson_acceleration_algorithm, HappyPath)
{
    using K = unsigned;
    using T = double;

    exp_series<T, K> series{};
    const anderson_acceleration_algorithm<T, K> algo{};

    constexpr K n = 7;
    constexpr K order = 0;

    const series_result<T> res = series.generate_series(0.5, n + 1);

    const T got = algo(n, order, res);
    const T expected = exp(0.5);

    std::cout << got << "\n" << expected << std::endl;

    EXPECT_NEAR(got, expected, 1e-4);
}
