#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

#include "series.h"
#include "methods/algorithm/brezinski_theta_algorithm.hpp"


using K = unsigned;

TEST(BrezinskiTheta, HapyPath) {
    exp_series<double, K> series(0.5);
    const brezinski_theta_algorithm<double, K, decltype(series)> algo(series);

    constexpr K n = 6;
    constexpr K order = 0;

    const double expected = series.S_n(n);
    const double got = algo.operator()(n, order);

    std::cout << got << std::endl;
    std::cout << expected << std::endl;

    EXPECT_DOUBLE_EQ(got, expected);
}