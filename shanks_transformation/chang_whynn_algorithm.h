/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the chang-wynn algorith,
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
 * @brief Chang-Wynn algorithm class template. 
 //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
 //On a Device for Computing the e (S ) Transformation - nothing new, just matrix

 //Construction of new generalizations of Wynn’s epsilon and rho algorithm by solving finite difference equations in the transformation order
 * @authors  Kreinin R.G.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class chang_whynn_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the chang_wynn_algorithm
	* @param series The series class object to be accelerated
	*/
    chang_whynn_algorithm(const series_templ& series);

	/**
	* @brief Fast impimentation of Levin algorithm.
	* Computes the partial sum after the transformation using the chang_wynn_algorithm
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

    std::vector<std::vector<T>> e(2, std::vector<T>(n, 0)); //2 vectors n-1 length containing Epsilon table next and previous 
    std::vector<T> f(n, 0); //vector for containing F results from 0 to n-1

    for (int i = 0; i < max; ++i) //Counting first row of Epsilon Table
    {
        e[0][i] = 1.0 / (this->series->operator()(i + 1)); 
    }

    for (int i = 0; i < max; ++i) //Counting F function
    {
        coef = (this->series->S_n(i + 3) + this->series->S_n(i + 1) - 2 * this->series->S_n(i + 2));
        coef2 = (this->series->S_n(i + 2) + this->series->S_n(i) - 2 * this->series->S_n(i + 1));

        up = this->series->operator()(i + 1) * this->series->operator()(i + 2) * coef;
        down = this->series->operator()(i + 3) * coef2;
        down -= (this->series->operator()(i + 1)) * coef;
        down = 1.0 / down;
        e[1][i] = this->series->S_n(i + 1) - up * down;

        f[i] = coef * coef2 * down; //Can make coeff2 ^2 for better effect
    }

    for (int k = 2; k <= max; ++k) { //Counting from 2 to n rows of Epsilon Table
        for (int i = 0; i < max - k; ++i) {

            up = 1 - k + k * f[i];
            down = 1.0 / (e[1][i + 1] - e[1][i]);
            e[0][i] = e[0][i + 1] + up * down;
        }
        std::swap(e[0], e[1]); //Swapping 1 and 2 rows of Epsilon Table. First ine will be overwriteen next turn
    }

    const T result = e[0][0]; //Only odd rows have mathmatical scence. Always returning e[0][0]
    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}