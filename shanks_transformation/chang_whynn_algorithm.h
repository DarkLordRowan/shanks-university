/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the chang-wynn algorith,
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Chang-Wynn algorithm class template. 
  //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
  //On a Device for Computing the e (S ) Transformation - nothing new, just matrix

 //Construction of new generalizations of Wynn’s epsilon and rho algorithm by solving finite difference equations in the transformation order
  * @authors Kreinin R.G.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class chang_whynn_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the chang_wynn_algorithm
	 * @param series The series class object to be accelerated
	 */
    chang_whynn_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief Fast impimentation of Levin algorithm.
	 * Computes the partial sum after the transformation using the chang_wynn_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */
	T operator()(const K n, const K order) const {
        if (n == 0)
            throw std::domain_error("zero integer in the input");

        T up, down, coef, coef2;

        //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
        K i1, i2, i3, k1;

        K max = n - (n & 1); // int -> K mark 

        std::vector<std::vector<T>> e(2, std::vector<T>(n, 0)); //2 vectors n length containing Epsilon table next and previous 
        std::vector<T> f(n, 0); //vector for containing F results from 0 to n-1

        for (K i = 0; i < max; ++i) //Counting first row of Epsilon Table
            e[0][i] = static_cast<T>(1.0 / (this->series->operator()(i + 1)));

        for (K i = 0; i < max; ++i) { //Counting F function
            i1 = i + 1;
            i2 = i + 2;
            i3 = i + 3;

            coef = static_cast<T>(fma(-2, this->series->S_n(i2), this->series->S_n(i3) + this->series->S_n(i1)));

            coef2 = static_cast<T>(fma(-2, this->series->S_n(i1), this->series->S_n(i2) + this->series->S_n(i)));

            up = this->series->operator()(i1) * this->series->operator()(i2) * coef;

            down = this->series->operator()(i3) * coef2;
            down -= this->series->operator()(i1) * coef;
            down = static_cast<T>(1.0 / down);

            e[1][i] = static_cast<T>(fma(-up, down, this->series->S_n(i1)));

            f[i] = coef * coef2 * down; //Can make coeff2 ^2 for better effect
        }

        for (K k = 2; k <= max; ++k) { //Counting from 2 to n rows of Epsilon Table
            k1 = 1 - k;
            for (K i = 0; i < max - k; ++i) {
                i1 = i + 1;
                up = static_cast<T>(fma(k, f[i], k1));
                down = static_cast<T>(1.0 / (e[1][i1] - e[1][i]));
                e[0][i] = static_cast<T>(fma(up, down, e[0][i1]));

                if (!std::isfinite(e[0][i])) { //Check for invalid values to avoid them
                    max = k + i1;
                    break;
                }
            }
            std::swap(e[0], e[1]); //Swapping 1 and 2 rows of Epsilon Table. First ine will be overwriteen next turn
        }

        const T result = e[max & 1][0]; //Only odd rows have mathmatical scence. Always returning e[0][0]

        if (!std::isfinite(result))
            throw std::overflow_error("division by zero");

        return result;
    }
};