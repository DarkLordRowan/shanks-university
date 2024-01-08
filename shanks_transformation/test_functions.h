/**
 * @file test_functions.h
 * @brief This file contains the testing functions
 */
#pragma once
#include <exception>
#include "test_functions.h"
#include "series_acceleration.h"
#include "series.h"

/*
* @brief Function that prints out comparesment between transformed and untransformed partial sums
* At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out partial sum of first n terms of the series
* After that it prints out transformed partial sum of first n terms of the series of order order 
* At last it prints out the difference between the last two
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms
* @param order The order of the transformation
* @param series The series class object to be accelerated
*/
template <typename series_templ, typename transform_type>
void cmp_sum_and_transform(const int n, const int order, const series_templ& series)
{
	transform_type test(series);
	test.print_info();
	try
	{
		test.print_s_n(n);
		test.print_t_n(n, order);
		test.print_diff_t_s(n, order);
	}
	catch (std::domain_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (std::overflow_error& e)
	{
		std::cout << e.what() << std::endl;
	}
}

/**
* @brief Function that prints out the remainders 
* At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out remainders of the series from 1 to n
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms for the last remainder
* @param order The order of the transformation
* @param series The series class object to be accelerated
*/
template <typename series_templ, typename transform_type>
void transformation_remainders(const int n, const int order, const series_templ& series)
{
	transform_type test(series);
	std::cout << "Tranformation of order " << order << " remainders from i = 1 to " << n << std::endl;
	test.print_info();
	for (int i = 0; i <= n; ++i)
	{
		try
		{
			std::cout << "S - T_" << i << " : " << series.get_sum() - test(i, order) << std::endl;
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}