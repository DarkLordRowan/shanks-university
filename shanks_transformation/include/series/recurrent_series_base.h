
/**
* @brief Abstract class for recurrent series
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
#pragma once
#include <stdexcept>

template <typename T, typename K>
class recurrent_series_base
{
public:
    virtual ~recurrent_series_base() = default;

    /**
    * @brief Parameterized constructor to initialize the recurrent series with function argument
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit recurrent_series_base(T x);

    /**
    * @brief Parameterized constructor to initialize the recurrent series with vector, containing elements of series
    * @authors Kreynin R.G.
    * @param row The first elements of the series
    */
    explicit recurrent_series_base(std::vector<T> row);

    /**
    * @brief Computes nth term of the series
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the series
    */
    virtual T acsess_row(K n) = 0;

    /**
    * @brief Vector, containing elements of the series
    * It's empty by default
    * @authors Kreynin R.G.
    */
    std::vector<T> series_vector;
};

template <typename T, typename K>
recurrent_series_base<T, K>::recurrent_series_base(T x)
{
    this->series_vector.push_back(x);
}

template <typename T, typename K>
recurrent_series_base<T, K>::recurrent_series_base(std::vector<T> row)
{
    if (row.empty())
        throw std::domain_error("empty row imput");

    this->series_vector = row;
}