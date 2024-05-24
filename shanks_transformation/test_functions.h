/**
 * @file test_functions.h
 * @brief This file contains the testing functions
 */

#pragma once
#include <exception>
#include "test_functions.h"
#include "series_acceleration.h"
//#include "series.h"
#include <chrono>

 /**
 * @brief Function that prints out comparesment between transformed and nontransformed partial sums
 * At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
 * Then it prints out partial sums of first i terms of the series where i ranges from 1 to n (!)
 * After that it prints out transformed partial sum of first i terms of the series of order order
 * At last it prints out the difference between the two
 * @authors Bolshakov M.P.
 * @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
 * @param n The number of terms
 * @param order The order of the transformation
 * @param series The series class object to be accelerated
 * @param test The type of transformation that is being used
 */
template <typename series_templ, typename transform_type>
void cmp_sum_and_transform(const int n, const int order, const series_templ&& series, const transform_type&& test)
{
	test->print_info();
	for (int i = 1; i <= n; ++i)
	{
		try
		{
			std::cout << "S_" << i << " : " << series->S_n(i) << std::endl;
			std::cout << "T_" << i << " of order " << order << " : " << test->operator()(i, order) << std::endl;
			std::cout << "T_" << i << " of order " << order << " - S_" << i
				<< " : " << test->operator()(i, order) - series->S_n(i) << std::endl;
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

/**
* @brief Function that prints out comparesment between the terms of transformed and nontransformed series
* At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out terms from the first to nth of the series
* At last it prints out terms from the first to nth of the transformed series
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test The type of transformation that is being used
*/
template <typename series_templ, typename transform_type>
void cmp_a_n_and_transform(const int n, const int order, const series_templ&& series, const transform_type&& test)
{
	test->print_info();
	for (int i = 1; i <= n; ++i)
	{
		try
		{
			std::cout << "a_" << i << " : " << (*series)(i) << std::endl;
			std::cout << "t_" << i << " : " << test->operator()(i, order) - test->operator()(i - 1, order) << std::endl;
			std::cout << "t_" << i << " of order " << order << " - a_" << i
				<< " : " << (test->operator()(i, order) - test->operator()(i - 1, order)) - (*series)(i) << std::endl;
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

/**
* @brief Function that prints out the remainders
* At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out remainders of the series from 1 to n
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms for the last remainder
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test The type of transformation that is being used
*/
template <typename series_templ, typename transform_type>
void transformation_remainders(const int n, const int order, const series_templ&& series, const transform_type&& test)
{
	std::cout << "Tranformation of order " << order << " remainders from i = 1 to " << n << std::endl;
	test->print_info();
	for (int i = 1; i <= n; ++i)
	{
		try
		{
			std::cout << "S - T_" << i << " : " << series->get_sum() - test->operator()(i, order) << std::endl;
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

/**
* @brief Function that showcases the difference between 2 transformations
* At first it prints out the type of transformations, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out remainders of the series sum from 1 to n transformed 2 different ways and also tells which one got closer to the sum of the series
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type_1 is the first type of transformation we are using, transform_type_2 is the second type of transformation we are using
* @param n The number of terms for the last remainder
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test_1 The type of the first transformation that is being used
* @param test_2 The type of the second transformation that is being used
*/
template <typename series_templ, typename transform_type_1, typename transform_type_2>
void cmp_transformations(const int n, const int order, const series_templ&& series, const transform_type_1&& test_1, const transform_type_2&& test_2)
{
	std::cout << "Tranformations of order " << order << " remainders from i = 1 to " << n << std::endl;
	std::cout << "The transformation #1 is ";
	test_1->print_info();
	std::cout << "The transformation #2 is ";
	test_2->print_info();
	auto diff_1 = (*series)(0);
	auto diff_2 = (*series)(0);
	for (int i = 1; i <= n; ++i)
	{
		try
		{
			diff_1 = series->get_sum() - test_1->operator()(i, order);
			diff_2 = series->get_sum() - test_2->operator()(i, order);
			std::cout << "The transformation #1: S - T_" << i << " : " << diff_1 << std::endl;
			std::cout << "The transformation #2: S - T_" << i << " : " << diff_2 << std::endl;
			if (std::abs(diff_1) < std::abs(diff_2))
				std::cout << "The transformation #1 is faster" << std::endl;
			else
				std::cout << "The transformation #2 is faster" << std::endl;
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

/**
* @brief Function that evaluates the time it takes to transform series
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms for the last remainder
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test The type of the first transformation that is being used
*/
template <typename series_templ, typename transform_type>
void eval_transform_time(const int n, const int order, const series_templ&& series, const transform_type&& test)
{
	const auto start_time = std::chrono::system_clock::now();
	test->print_info();
	for (int i = 1; i <= n; ++i)
	{
		try
		{
			test->operator()(i, order);
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
	const auto end_time = std::chrono::system_clock::now();
	const std::chrono::duration<double, std::milli> diff = end_time - start_time;
	std::cout << "It took " << diff.count() << " to perform these transformations" << std::endl;
}


/*
* @brief Function that prints the terms of nontransformed partial sums
* Then it prints out the nth terms  of the series
* @authors Kreynin R.G.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms
*/
template <typename series_templ>
void print_sum(const int n, const series_templ&& series)
{
	std::cout << "S_" << n << " : " << series->S_n(n) << std::endl;
}

/**
* @brief Function that prints transformed partial sums
* At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
* It prints out transformed partial sum of the n term of the series of order order
* @authors Kreynin R.G.
* @tparam transform_type is the type of transformation we are using
* @param n The number of terms
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test The type of transformation that is being used
*/
template <typename transform_type>
void print_transform(const int n, const int order, const transform_type&& test)
{
	test->print_info();
	try
	{
		std::cout << "T_" << n << " of order " << order << " : " << test->operator()(n, order) << std::endl;
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