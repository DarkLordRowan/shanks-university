/**
 * @file richardson_algorithm.h
 * @brief This file contains the definition of the Richardson transformation class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0


#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library


 /**
 * @brief Richardson transformation
 * @authors Trudolyubov N.A., Pavlova A.R.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class richardson_algorithm : public series_acceleration<T, K, series_templ>
{
public:
     /**
    * @brief Parameterized constructor to initialize the Richardson transformation for series.
    * @param series The series class object
    */
    richardson_algorithm(const series_templ& series);
     /**
    * @brief Richardson transformation for series function.
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator() (const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
richardson_algorithm<T, K, series_templ>::richardson_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T richardson_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
    // in the method we don't use order, it's only a stub 
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0)
        return DEF_UNDEFINED_SUM;

    // create a matrix
    std::vector<std::vector<T>> D(n + 1, std::vector<T>(n + 1, 0));

    // Fill partial sums in the first string 
    for (int i = 0; i <= n; ++i) {
        D[i][0] = this->series->S_n(i);
    }

    // The Richardson method main function 
    for (int l = 1; l <= n; ++l) {
        for (int m = l; m <= n; ++m) {
            D[m][l] = (pow(4, l) * D[m][l - 1] - D[m - 1][l - 1]) / (pow(4, l) - 1);
        }
    }

    // Get the last transfomation element from the matrix
    return D[n][n];
}