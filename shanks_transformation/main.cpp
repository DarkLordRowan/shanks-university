#include <memory>

#include "series/series_base.h"
#include "series/serieses/exp_series.h"
#include "series_acceleration/series_acceleration.hpp"
#include "series_acceleration/algorithm/ford_sidi_two_algorithm.hpp"

template <typename series_templ, typename transform_type>
void cmp_sum_and_transform(const int n, const int order, const series_templ&& series, const transform_type&& test)
{
    test->print_info();
    for (int i = 1; i <= n; ++i)
    {
        try
        {
            std::cout << "S_" << i << " : " << series->S_n(i) << '\n';
            std::cout << "T_" << i << " of order " << order << " : " << test->operator()(i, order) << '\n';
            std::cout << "T_" << i << " of order " << order << " - S_" << i
                << " : " << test->operator()(i, order) - series->S_n(i) << '\n';
        }
        catch (std::domain_error& e)
        {
            std::cout << e.what() << '\n';
        }
        catch (std::overflow_error& e)
        {
            std::cout << e.what() << '\n';
        }
    }
}


template<typename T, typename K>
void main_testing_function() {
    std::unique_ptr<series_base<T, K> > series;
    T x = 1;

    series.reset(new exp_series<T, K>(x));

    std::unique_ptr<series_acceleration<T, K, decltype(series.get())> > transform;

    transform.reset(new ford_sidi_two_algorithm<T, K, decltype(series.get())>(series.get()));


    int n = 10;
    int order = 5;


    std::cout << "S_" << n << " : " << series->S_n(n) << '\n';
    std::cout << "T_" << n << " of order " << order << " : " << transform->operator()(n, order) << '\n';
    std::cout << "T_" << n << " of order " << order << " - S_" << n
        << " : " << transform->operator()(n, order) - series->S_n(n) << '\n';
}

int main(int argc, char *argv[]) {
    main_testing_function<long double, long long int>();
}
