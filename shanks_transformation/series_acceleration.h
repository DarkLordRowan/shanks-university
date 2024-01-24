/**
 * @file series_acceleration.h
 * @brief This file contains the declaration of the base class series_acceleration
 */

#pragma once
 /** @brief Default value for undefined transformation */
#define DEF_NO_TRANSFORM 0
/** @brief Default value for an unspecified series */
#define NO_SERIES_GIVEN 0
/** @brief if 1 then the class has methods of printing partial sum of series, partial sum of transformed series and difference between them */
#define DEBUGGING_MODE 1

#include <functional>  // Include the functional library for std::function
#include <iostream>   // Include the iostream library for I/O functionalities
#include <exception>  // Include the exception library for std::exception
#include <math.h>     // Include the math library for mathematical functions
#include <string>	  // Include the library which contains the string class
#include "series.h"


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

	virtual ~series_acceleration() = 0;

	/**
  * @brief Method for printing out the partial sum of the given series up to n terms
  * @authors Bolshakov M.P.
  * @param n The number of terms
  */

#if DEBUGGING_MODE

	constexpr void print_s_n(const K n) const;
	/**
   * @brief Method for printing out the partial sum of the given series up to n terms to a specified output stream
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   */

	constexpr void print_s_n(const K n, std::ostream& out) const;
	/**
   * @brief Method for printing out the nth term of the given series up to n terms and a specified order
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param out The output stream
   */
	constexpr void print_t_n(const K n, const int order) const;

	/**
   * @brief Method for printing out the nth term of the given series up to n terms and a specified order to a specified output stream
   * @authors Bolshakov M.P.
   * @param n The number of terms
   * @param order The order
   */
	constexpr void print_t_n(const K n, const int order, std::ostream& out) const;

	/**
   * @brief Method for printing out the difference between the partial sum after transformation and the original partial sum up to n terms and a specified order
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order
   * @param out The output stream
   */
	constexpr void print_diff_t_s(const K n, const int order) const;

	/**
   * @brief Method for printing out the difference between the partial sum after transformation and the original partial sum up to n terms and a specified order to a specified output stream
   * @authors Bolshakov M.P.
   * @param n The number of terms
   * @param order The order
   */
	constexpr void print_diff_t_s(const K n, const int order, std::ostream& out) const;

	/**
   * @brief Method for printing out the nth term of the series
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of the term
   */
	constexpr void print_a_n(const K n) const;

	/**
   * @brief Method for printing out the nth term of the series in the specfied ostream
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of the term
   * @param out The output stream
   */
	constexpr void print_a_n(const K n, std::ostream& out) const;

	/**
   * @brief Method for printing out the nth term of the transformed series
   * @authors Bolshakov M.P.
   * @param n The number of the term
   * @param order The order
   */
	constexpr void print_t_n_term(const K n, const int order) const;

	/**
   * @brief Method for printing out the nth term of the transformed series in the specfied ostream
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order
   * @param out The output stream
   */
	constexpr void print_t_n_term(const K n, const int order, std::ostream& out) const;

	/**
   * @brief Method for printing out the difference between the nth term of the series and the nth term of the transformed sum
   * @authors Bolshakov M.P.
   * @param n The number of terms
   * @param order The order
   */
	constexpr void print_diff_t_n_term_a_n(const K n, const int order) const;

	/**
   * @brief Method for printing out the difference between the nth term of the series and the nth term of the transformed sum in the specfied ostream
   * @authors Bolshakov M.P.
   * @param n The number of terms
   * @param order The order
   */
	constexpr void print_diff_t_n_term_a_n(const K n, const int order, std::ostream& out) const;

	/**
   * @brief Method for printing out the info about the object of this class
   * @authors Bolshakov M.P.
   */
	constexpr void print_info() const;

#endif

	/**
   * @brief Virtual operator() that returns the partial sum after transformation of the series
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order of the transformation
   * @return The transformed partial sum
   */
	virtual T operator()(const K n, const int order) const;

protected:
	/**
   * @brief Series whose convergence is being accelerated
   * @authors Bolshakov M.P.
   */
	series_templ series;
};

template <typename T, typename K, typename series_templ>
series_acceleration<T, K, series_templ>::series_acceleration(const series_templ& series) : series(series)
{

}

template <typename T, typename K, typename series_templ>
series_acceleration<T, K, series_templ>::~series_acceleration()
{

}

#if DEBUGGING_MODE

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_s_n(const K n) const
{
	print_s_n(n, std::cout);
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_s_n(const K n, std::ostream& out) const
{
	out << "S_" << n << " : " << this->series->S_n(n) << std::endl;
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_t_n(const K n, const int order) const
{
	print_t_n(n, order, std::cout);
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_t_n(const K n, const int order, std::ostream& out) const
{
	out << "T_" << n << " of order " << order << " : " << this->operator()(n, order) << std::endl;
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_diff_t_s(const K n, const int order) const
{
	print_diff_t_s(n, order, std::cout);
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_diff_t_s(const K n, const int order, std::ostream& out) const
{
	out << "T_" << n << " of order " << order << " - S_" << n
		<< " : " << this->operator()(n, order) - this->series->S_n(n) << std::endl;
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_a_n(const K n) const
{
	print_a_n(n, std::cout);
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_a_n(const K n, std::ostream& out) const
{
	out << "a_" << n << " : " << this->series->a_n(n) << std::endl;
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_t_n_term(const K n, const int order) const
{
	print_t_n_term(n, order, std::cout);
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_t_n_term(const K n, const int order, std::ostream& out) const
{
	out << "t_" << n << " : " << this->operator()(n, order) - this->operator()(n - 1, order) << std::endl;
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_diff_t_n_term_a_n(const K n, const int order) const
{
	print_diff_t_n_term_a_n(n, order, std::cout);
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_diff_t_n_term_a_n(const K n, const int order, std::ostream& out) const
{

	out << "t_" << n << " of order " << order << " - a_" << n
		<< " : " << (this->operator()(n, order) - this->operator()(n - 1, order)) - this->series->a_n(n) << std::endl;
}

template <typename T, typename K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_info() const
{
	std::cout << "transformation: " << typeid(*this).name() << std::endl;
}

#endif

template <typename T, typename K, typename series_templ>
T series_acceleration<T, K, series_templ>::operator()(const K n, const int order) const
{
	return DEF_NO_TRANSFORM;
}