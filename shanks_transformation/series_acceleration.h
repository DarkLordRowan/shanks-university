/**
 * @file series_acceleration.h
 * @brief This file contains the declaration of the base class series_acceleration
 */

#pragma once
 /** @brief Default value for undefined transformation */
#define DEF_NO_TRANSFORM 0
/** @brief Default value for an unspecified series */
#define NO_SERIES_GIVEN 0

#include <functional>  // Include the functional library for std::function
#include <iostream>   // Include the iostream library for I/O functionalities
#include <exception>  // Include the exception library for std::exception
#include <math.h>     // Include the math library for mathematical functions
#include <string>	  // Include the library which contains the string class
//#include "series.h"
#include "series +.h" 


/**
 * @brief Base class series_acceleration
 * @authors Bolshakov M.P.
 * It is not used on it's own, but it is inherited by shanks_transformation and epsilon_algorithm to implement the corresponding methods.
 * The class implementation provides everything needed for construction of an arbitrary series up to n terms and printing out the partial sum,
 * the partial sum after transformation is used, and the difference between the latter and the former.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class series_acceleration
{
public:
	/**
   * @brief Parameterized constructor to initialize the Transformation.
   * @authors Bolshakov M.P.
   * @param series The series class object to be accelerated
   */
	series_acceleration(const series_templ& series);

	/**
   * @brief Method for printing out the info about the object of this class
   * @authors Bolshakov M.P.
   */
	constexpr void print_info() const;

	/**
   * @brief Virtual operator() that returns the partial sum after transformation of the series
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order of the transformation
   * @return The transformed partial sum
   */
	virtual T operator()(const K n, const int order) const = 0;

protected:
	/**
   * @brief Series whose convergence is being accelerated
   * @authors Bolshakov M.P.
   */
	series_templ series;
};

template <typename T, typename K, typename series_templ>
series_acceleration<T, K, series_templ>::series_acceleration(const series_templ& series) : series(series) {}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_info() const
{
	std::cout << "transformation: " << typeid(*this).name() << std::endl;
}