
/**
* @brief using gamma serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class gamma_series final : public series_base<T, K>
{
public:
    gamma_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param t
    * @param x The argument for function series
    */
    gamma_series(T t, T x);

    /**
    * @brief Computes the nth term of the using gamma series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the using gamma series
    */
    [[nodiscard]] T operator()(K n) const override;

private:
    T t;
    T x;

    static T a_k(K n);
};

template <typename T, typename K>
gamma_series<T, K>::gamma_series(T t, T x) : series_base<T, K>(x), t(t), x(x) {}

template <typename T, typename K>
T gamma_series<T, K>::a_k(K n) {
    return static_cast<T>(n + 1);
}

template <typename T, typename K>
T gamma_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    T a_k_n = a_k(n);  // ��������� a_k
    return static_cast<T>(a_k_n * std::pow(this->t, n) * std::tgamma(a_k_n));
}
