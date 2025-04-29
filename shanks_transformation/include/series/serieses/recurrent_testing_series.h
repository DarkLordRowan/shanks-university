
/**
* @brief function for testing new requrrive serieses or converting basic to
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class recurrent_testing_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
    recurrent_testing_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit recurrent_testing_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
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
recurrent_testing_series<T, K>::recurrent_testing_series(T x) : series_base<T, K>(x, 0), recurrent_series_base<T, K>(x) {}

template <typename T, typename K>
T recurrent_testing_series<T, K>::acsess_row(K n)
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    K old_size = this->series_vector.size();
    this->series_vector.reserve(n);

    for (K i = old_size; i <= n; ++i) {
        this->series_vector.push_back(this->series_vector[i - 1] * (this->x * this->x) / (i * (4 * i + 2)));
    }

    return this->series_vector[n];
}

template <typename T, typename K>
T recurrent_testing_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    const T a = const_cast<recurrent_testing_series<T, K>*>(this)->acsess_row(n);
    return a;

}
