/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the second implementation of Epsilon Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
 * @brief Epsilon Algorithm MK-2 class template. "Scalar Epsilon Algorithm"
 //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
 //On a Device for Computing the e (S ) Transformation - nothing new, just matrix
 //euler algoritm A_Note_on_the_Generalised_Euler_Transformation-Wynn-1971 - has Euler, but for um = z^m * v_m
 * @authors  Kreinin R.G.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class epsilon_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:
    /**
    * @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
    * @param series The series class object to be accelerated
    */
    epsilon_algorithm_two(const series_templ& series);

    /**
    * @brief Fast impimentation of Levin algorithm.
    * Computes the partial sum after the transformation using the Epsilon Algorithm.
    * For more information, see page 20-21 in [https://hal.science/hal-04207550/document]
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
epsilon_algorithm_two<T, K, series_templ>::epsilon_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T epsilon_algorithm_two<T, K, series_templ>::operator()(const K n, const int order) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0)
        return DEF_UNDEFINED_SUM;
    else if (order == 0)
        return this->series->S_n(n);

    int k = 2 * order;

    (n % 2 == 0) ? k += n : k += n - 1;

    std::vector<std::vector<T>> e(4, std::vector<T>(k + 3, 0)); //4 vectors k+3 length containing four Epsilon Table rows 

    for (int j = k; j >= 0; --j) //Counting first row of Epsilon Table
    {
        e[3][j] = this->series->S_n(j);
    }

    T a = 0, a1 = 0, a2 = 0;

    while (k > -1)
    {
        for (int i = 0; i < k; ++i)
        {
            e[0][i] = e[2][i + 1] + 1.0 / (e[3][i + 1] - e[3][i]); //Standart Epsilon Wynn algorithm

            if (!std::isfinite(e[0][i]) && i + 2 <= k) //This algorithm is used if new elliment is corrupted.
            {
                a2 = 1.0 / e[2][i + 1];

                a1 = 1.0 / (1.0 - (a2 * e[2][i + 2]));
                a = e[2][i + 2] * a1;

                a1 = 1.0 / (1.0 - (a2 * e[2][i]));
                a += e[2][i] * a1;

                a1 = 1.0 / (1.0 - (a2 * e[0][i + 2]));
                a -= e[0][i + 2] * a1;

                e[0][i] = 1.0 / e[2][i + 1];
                e[0][i] = 1.0 / (1.0 + a * e[0][i]);
                e[0][i] = e[0][i] * a;
            }
            if (!std::isfinite(e[0][i])) //If new element is still corrupted we just copy prev. element, so we will get result
            {
                e[0][i] = e[2][i];
            }
        }
        std::swap(e[0], e[1]); //Swapping rows of Epsilon Table. First ine will be overwriteen next turn
        std::swap(e[1], e[2]);
        std::swap(e[2], e[3]);

        --k;
    }
    const T result = e[0][0]; //Only odd rows have mathmatical scence. Always returning e[0][0]

    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}