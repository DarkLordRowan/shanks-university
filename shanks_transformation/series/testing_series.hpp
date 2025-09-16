#pragma once
#include "series.h"

/**
* @brief function for testing new serieses or converting basic to
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class testing_series final : public series_base<T, K>
{
public:
    testing_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    testing_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
testing_series<T, K>::testing_series(T x) : series_base<T, K>(x, 0)
{
    this->series_name = "testing_series";
    // Область сходимости зависит от конкретной тестовой функции
    // Требуется уточнение для конкретного тестового случая

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T testing_series<T, K>::operator()(K n) const
{
    return 0;
}
