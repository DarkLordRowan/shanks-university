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

 //Construction of new generalizations of Wynn’s
epsilon and rho algorithm by solving finite difference
equations in the transformation order
chang algo!
 * @authors  Kreinin R.G.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class chang_whynn_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
	* @param series The series class object to be accelerated
	*/
    chang_whynn_algorithm(const series_templ& series);

	/**
	* @brief Fast impimentation of Levin algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
chang_whynn_algorithm<T, K, series_templ>::chang_whynn_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T chang_whynn_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0)
        return DEF_UNDEFINED_SUM;

    T up, down, coef, coef2;

    int max = 0;

    (n % 2 == 0) ? max += n : max += n - 1;

    std::vector<std::vector<T>> t(2, std::vector<T>(n, 0));
    std::vector<T> f(n, 0);

    for (int i = 0; i < max; ++i)
    {
        t[0][i] = 1.0 / (this->series->operator()(i + 1));
    }

    for (int i = 0; i < max; ++i)
    {
        coef = (this->series->S_n(i + 3) + this->series->S_n(i + 1) - 2 * this->series->S_n(i + 2));
        coef2 = (this->series->S_n(i + 2) + this->series->S_n(i) - 2 * this->series->S_n(i + 1));

        up = this->series->operator()(i + 1) * this->series->operator()(i + 2) * coef;
        down = this->series->operator()(i + 3) * coef2;
        down -= (this->series->operator()(i + 1)) * coef;
        down = 1.0 / down;
        t[1][i] = this->series->S_n(i + 1) - up * down;

        f[i] = coef * coef2 * down; //Can make coeff2 ^2 for better effect
    }

    for (int k = 2; k <= max; ++k) {
        for (int i = 0; i < max - k; ++i) {

            up = 1 - k + k * f[i];
            down = 1.0 / (t[1][i + 1] - t[1][i]);
            t[0][i] = t[0][i + 1] + up * down;
        }
        std::swap(t[0], t[1]);
    }

    const T result = t[0][0];
    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}