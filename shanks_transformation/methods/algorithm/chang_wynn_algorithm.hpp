/**
 * @file chang_wynn_algorithm.hpp
 * @brief This file contains the declaration of the chang-wynn algorith,
 */

#pragma once

#include "../series_acceleration.hpp"
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
class chang_wynn_algorithm final : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the chang_wynn_algorithm
	 * @param series The series class object to be accelerated
	 */
    explicit chang_wynn_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief Fast impimentation of Levin algorithm.
	 * Computes the partial sum after the transformation using the chang_wynn_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */
	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T chang_wynn_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::fma;

    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");

    T up, down, coef, coef2;

    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    K i1, i2, i3, k1;
    K max = n - (n & static_cast<K>(1)); // int -> K mark 

    std::vector<std::vector<T>> e(
        2, 
        std::vector<T>(
            n, 
            static_cast<T>(0)
        )
    ); //2 vectors n length containing Epsilon table next and previous 

    std::vector<T> f(n, static_cast<T>(0)); //vector for containing F results from 0 to n-1

    for (K i = static_cast<K>(0); i < max; ++i) //Counting first row of Epsilon Table
        e[0][i] = static_cast<T>(1) / (this->series->operator()(i + static_cast<K>(1)));

    for (K i = static_cast<K>(0); i < max; ++i) { //Counting F function

        i1 = i + static_cast<K>(1);
        i2 = i + static_cast<K>(2);
        i3 = i + static_cast<K>(3);

        coef = fma(
            static_cast<T>(-2), 
            this->series->S_n(i2), 
            this->series->S_n(i3) + this->series->S_n(i1)
        );
        coef2 = fma(
            static_cast<T>(-2), 
            this->series->S_n(i1), 
            this->series->S_n(i2) + this->series->S_n(i)
        );

        up = this->series->operator()(i1);
        up*= this->series->operator()(i2);
        up*= coef;

        down = this->series->operator()(i3) * coef2;
        down -= this->series->operator()(i1) * coef;
        down = static_cast<T>(1) / down;

        e[1][i] = static_cast<T>(fma(-up, down, this->series->S_n(i1)));
        f[i] = coef * coef2 * down; //Can make coeff2 ^2 for better effect
    }
    for (K k = static_cast<K>(2); k <= max; ++k) { //Counting from 2 to n rows of Epsilon Table
        k1 = static_cast<K>(1) - k;

        for (K i = static_cast<K>(0); i < max - k; ++i) {
            i1 = i + static_cast<K>(1);

            up = fma(static_cast<T>(k), f[i], static_cast<T>(k1));
            down = static_cast<T>(1);
            down/= (e[1][i1] - e[1][i]);

            e[0][i] = fma(up, down, e[0][i1]);
            if (!isfinite(e[0][i])) { //Check for invalid values to avoid them
                max = k + i1;
                break;
            }

        }
        
        std::swap(e[0], e[1]); //Swapping 1 and 2 rows of Epsilon Table. First ine will be overwriteen next turn
    }

    if (!isfinite(e[max & static_cast<K>(1)][0])) //Only odd rows have mathmatical scence. Always returning e[0][0]
        throw std::overflow_error("division by zero");

    return e[max & static_cast<K>(1)][0];
}