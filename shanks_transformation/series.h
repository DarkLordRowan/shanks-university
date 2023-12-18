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


/**
 * @brief Base class series_acceleration
 *
 * It is not used on it's own, but it is inherited by shanks_transformation and epsilon_algorithm to implement the corresponding methods.
 * The class implementation provides everything needed for construction of an arbitrary series up to n terms and printing out the partial sum,
 * the partial sum after transformation is used, and the difference between the latter and the former.
 */
template <typename T, typename K>
class series_acceleration
{
public:
	series_acceleration();

	/**
   * @brief Constructor that receives a series function and a point x
   * @authors Bolshakov M.P.
   * @param series The series function which returns the nth term of the given series at point x
   * @param x The point x
   */
	series_acceleration(const std::function<T(const T, const K)>& series, const T x);

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
   * @param out The output stream
   */

	constexpr void print_s_n(const K n, std::ostream& out) const;
	/**
   * @brief Method for printing out the nth term of the given series up to n terms and a specified order
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order
   */
	constexpr void print_t_n(const K n, const int order) const;

	/**
   * @brief Method for printing out the nth term of the given series up to n terms and a specified order to a specified output stream
   * @authors Bolshakov M.P.
   * @param n The number of terms
   * @param order The order
   * @param out The output stream
   */
	constexpr void print_t_n(const K n, const int order, std::ostream& out) const;

	/**
   * @brief Method for printing out the difference between the partial sum after transformation and the original partial sum up to n terms and a specified order
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order
   */
	constexpr void print_diff_t_s(const K n, const int order) const;

	/**
   * @brief Method for printing out the difference between the partial sum after transformation and the original partial sum up to n terms and a specified order to a specified output stream
   * @authors Bolshakov M.P.
   * @param n The number of terms
   * @param order The order
   * @param out The output stream
   */
	constexpr void print_diff_t_s(const K n, const int order, std::ostream& out) const;

#endif

protected:
	/**
   * @brief Virtual operator() that returns the partial sum after transformation of the series
   * @authors Bolshakov M.P., Pashkov B.B. 
   * @param n The number of terms
   * @param order The order
   * @return The transformed partial sum
   */
	virtual T operator()(const K n, const int order) const;

	/**
   * @brief Series function which returns the nth term of the given series at point x
   */
	std::function<T(const T, const K)> series;

	/**
   * @brief Point x
   */
	T x;

	/**
	   * @brief Partial sum of the series up to n terms
	   * @authors Bolshakov M.P.
	   * @param n The number of terms
	   * @return The partial sum
	   */
	constexpr T S_n(K n) const;
};

/**
 * @brief Default constructor for the series_acceleration class.
 * Initializes the series function with a lambda function returning NO_SERIES_GIVEN.
 */
template <typename T, typename K>
series_acceleration<T, K>::series_acceleration()
{
	series = [](const T x, const int n) -> T {return NO_SERIES_GIVEN; };
}

/**
 * @brief Parameterized constructor for the series_acceleration class.
 * Initializes the series function and the value of x.
 * @param series The series function to be accelerated.
 * @param x The value of x.
 */
template <typename T, typename K>
series_acceleration<T, K>::series_acceleration(const std::function<T(const T, const K)>& series, const T x) : x(x), series(series)
{

}

/**
 * @brief Destructor for the series_acceleration class.
 * Provides cleanup for the series acceleration.
 */
template <typename T, typename K>
series_acceleration<T, K>::~series_acceleration()
{

}

/**
 * @brief Print the partial sum S_n to the standard output stream.
 *
 * @param n The number of terms in the partial sum.
 */

#if DEBUGGING_MODE

template <typename T, typename K>
constexpr void series_acceleration<T, K>::print_s_n(const K n) const
{
	print_s_n(n, std::cout);
}

/**
 * @brief Print the partial sum S_n to the specified output stream.
 *
 * @param n The number of terms in the partial sum.
 * @param out The output stream.
 */
template <typename T, typename K>
constexpr void series_acceleration<T, K>::print_s_n(const K n, std::ostream& out) const
{
	out << "S_" << n << " : " << S_n(n) << std::endl;
}

/**
 * @brief Print the nth term of the series after transformation of order to the standard output stream.
 *
 * @param n The number of terms.
 * @param order The order of transformation.
 */
template <typename T, typename K>
constexpr void series_acceleration<T, K>::print_t_n(const K n, const int order) const
{
	print_t_n(n, order, std::cout);
}


/**
 * @brief Print the nth term of the series after transformation of order to the specified output stream.
 *
 * @param n The number of terms.
 * @param order The order of transformation.
 * @param out The output stream.
 */
template <typename T, typename K>
constexpr void series_acceleration<T, K>::print_t_n(const K n, const int order, std::ostream& out) const
{
	out << "T_" << n << " of order " << order << " : " << this->operator()(n, order) << std::endl;
}

/**
 * @brief Print the difference between the partial sum after transformation and the original partial sum to the standard output stream.
 *
 * @param n The number of terms.
 * @param order The order of transformation.
 */
template <typename T, typename K>
constexpr void series_acceleration<T, K>::print_diff_t_s(const K n, const int order) const
{
	print_diff_t_s(n, order, std::cout);
}

/**
 * @brief Print the difference between the partial sum after transformation and the original partial sum to the specified output stream.
 *
 * @param n The number of terms.
 * @param order The order of transformation.
 * @param out The output stream.
 */
template <typename T, typename K>
constexpr void series_acceleration<T, K>::print_diff_t_s(const K n, const int order, std::ostream& out) const
{
	out << "T_" << n << " of order " << order << " - S_" << n
		<< " : " << this->operator()(n, order) - S_n(n) << std::endl;
}

#endif

/**
 * @brief Calculate the partial sum of the series up to n terms.
 *
 * @param n The number of terms.
 * @return The partial sum S_n.
 */
template <typename T, typename K>
constexpr T series_acceleration<T, K>::S_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	T s_n = 0;
	for (int i = n; i >= 0; --i)
		s_n += series(x, i);
	return s_n;
}

/**
 * @brief Calculate the transformation of the partial sum.
 *
 * @param n The number of terms.
 * @param order The order of transformation.
 * @return Default value for undefined transformation.
 */
template <typename T, typename K>
T series_acceleration<T, K>::operator()(const K n, const int order) const
{
	return DEF_NO_TRANSFORM;
}