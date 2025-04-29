#pragma once
#include <cmath>
#include <vector>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class exp_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
    exp_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Bolshakov M.P.
    * @param x The argument for function series
    */
    explicit exp_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the exponent
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the exponent
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
exp_series<T, K>::exp_series(T x) : series_base<T, K>(x, std::exp(x)), recurrent_series_base<T, K>(T(1)) {  }

template <typename T, typename K>
T exp_series<T, K>::acsess_row(K n)
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    auto old_size = this->series_vector.size();
    this->series_vector.reserve(n);

    for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
    {
        this->series_vector.push_back(this->series_vector[i - 1] * this->x / i);
    }

    return this->series_vector[n];
}

template <typename T, typename K>
T exp_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    const T a = const_cast<exp_series*>(this)->acsess_row(n);
    return a;
}
