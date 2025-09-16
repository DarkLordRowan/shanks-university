#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function arth(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arctanh_x2_series final : public series_base<T, K>
{
public:
    arctanh_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @param x The argument for function series
    */
    arctanh_x2_series(T x);

    /**
    * @brief Computes the nth term of the arctanhx2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
arctanh_x2_series<T, K>::arctanh_x2_series(T x) : series_base<T, K>(x, atanh(x* x))
{
    this->series_name = "arctanh(x²)";
    // Сходится при |x| < 1 (ряд для гиперболического арктангенса, где z = x²)
    // Расходится при |x| ≥ 1

    if (abs(x) >= static_cast<T>(1) || !isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T arctanh_x2_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(fma(2, n, 1));
    return pow(this->x, static_cast<T>(2) * a) / a; // (90.2) [Rows.pdf]
}
