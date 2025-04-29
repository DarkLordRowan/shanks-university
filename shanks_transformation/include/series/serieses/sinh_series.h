
/**
* @brief Maclaurin series of sinh function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>
#include <vector>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class sinh_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
    sinh_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Bolshakov M.P.
    * @param x The argument for function series
    */
    explicit sinh_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sinh function
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sinh functions
    */
    [[nodiscard]] T operator()(K n) const override;

private:
    /**
    * @brief Computes nth term of the series
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the series
    */
    T acsess_row(K n) override;
};

template <typename T, typename K>
sinh_series<T, K>::sinh_series(T x) : series_base<T, K>(x, std::sinh(x)), recurrent_series_base<T, K>(x) {}

template <typename T, typename K>
T sinh_series<T, K>::acsess_row(K n)
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    auto old_size = this->series_vector.size();
    this->series_vector.reserve(n);

    for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
        this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(this->x * this->x / (i * (4 * i + 2))));
    }

    return this->series_vector[n];
}

template <typename T, typename K>
T sinh_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    const T a = const_cast<sinh_series*>(this)->acsess_row(n);
    return a;
}
