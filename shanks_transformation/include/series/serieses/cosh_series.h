
/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>
#include <vector>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class cosh_series : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
    cosh_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    explicit cosh_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of hyperbolic cosine
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;

private:
    /**
    * @brief Computes nth term of the series
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the series
    */
    T acsess_row(K n);
};

template <typename T, typename K>
cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x)), recurrent_series_base<T, K>(std::vector<T>{1, T(x* x / 2) }) {}

template <typename T, typename K>
T cosh_series<T, K>::acsess_row(K n)
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    auto old_size = this->series_vector.size();
    this->series_vector.reserve(n);

    for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
        this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(this->x * this->x / (i * (4 * i - 2))));
    }

    return this->series_vector[n];
}

template <typename T, typename K>
T cosh_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    const T a = const_cast<cosh_series*>(this)->acsess_row(n);
    return a;
}
