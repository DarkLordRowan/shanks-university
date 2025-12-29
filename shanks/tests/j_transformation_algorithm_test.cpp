#include <gtest/gtest.h>

#include "../include/series.hpp"
#include "../include/transformations/j_transformation_algorithm.hpp"

TEST(j_transformation_algorithm, HappyPath)
{
    using K = unsigned;
    using T = double;

    exp_series<T, K> series{};
    const j_transformation_algorithm<T, K> algo{};

    constexpr K n = 7;
    constexpr K order = 2;   // пример ненулевого порядка

    const series_result<T> res = series.generate_series(0.5, n + order + 1);

    const T got = algo(n, order, res);
    const T expected = exp(0.5);

    std::cout << got << "\n" << expected << std::endl;

    EXPECT_NEAR(got, expected, 1e-4);
}
