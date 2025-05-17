//
//
// #include <memory>
//
// #include "series/series_base.h"
// #include "series/serieses/exp_series.h"
// #include "series_acceleration/series_acceleration.hpp"
// #include "series_acceleration/algorithm/ford_sidi_two_algorithm.hpp"
//
// int main(int argc, char *argv[]) {
//
//     main_testing_function<long double, long long int>();
// }
//
// template <typename T, typename K>
//  static void main_testing_function() {
//
//     std::unique_ptr<series_base<T, K>> series;
//     T x = 1;
//
//     series.reset(new exp_series<T, K>(x));
//
//     std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform;
//
//     transform.reset(new ford_sidi_two_algorithm<T, K, decltype(series.get())>(series.get()));
//
//
//
//     int n = 3;
//     int order = 3;
//
//     cmp_sum_and_transform(n, order, std::move(series.get()), std::move(transform.get()));
//
// }
