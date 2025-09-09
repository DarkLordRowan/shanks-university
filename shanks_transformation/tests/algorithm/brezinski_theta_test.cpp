#include <gtest/gtest.h>

#include "series.h"
#include "../service_pointer_adapter.hpp"
#include "methods/algorithm/brezinski_theta_algorithm.hpp"


using K = unsigned;

TEST(BrezinskiTheta, HapyPath)
{
    const exp_series<double, K> series(0.5);
    using SeriesAdapter = SeriesPointerAdapter<exp_series<double, K> >;
    const brezinski_theta_algorithm<double, K, SeriesAdapter> algo(SeriesAdapter{&series});

    constexpr K n = 6;
    constexpr K order = 0;

    const double expected = series.S_n(n);
    const double got = algo(n, order);

    std::cout << got << "\n" << expected << std::endl;

    EXPECT_DOUBLE_EQ(got, expected);
}
