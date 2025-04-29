
/**
* @brief Taylor series of function sqrt(1 + x) (��� ������ ����������� �������, �������� ��-�� �����������)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"


template <typename T, typename K>
class sqrt_1plusx_series final : public series_base<T, K>
{
public:
    sqrt_1plusx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit sqrt_1plusx_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
sqrt_1plusx_series<T, K>::sqrt_1plusx_series(T x) : series_base<T, K>(x, std::sqrt(1 + x)) {}

template <typename T, typename K>
T sqrt_1plusx_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x < -1)
        throw std::domain_error("The value x must be more or equel to -1");
    return static_cast<T>(std::pow(-1, n) * this->fact(2 * n) * std::pow(this->x, n) / ((1 - 2 * n) * this->fact(n) * this->fact(n) * std::pow(4, n)));
}

