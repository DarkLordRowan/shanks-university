/**
 * @file test_functions.h
 * @brief This file contains the testing functions
 */

#pragma once

//#include "series.h"
#include <chrono>
#include <iostream>

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
template <std::unsigned_integral K, typename series_templ, typename transform_type>
void cmp_sum_and_transform(const K n, const K order, const series_templ& series, const transform_type& test)
{
	test->print_info();
	for (K i = 0; i <= n; ++i) {
		try
		{
			std::cout << "Sum of algo : " << series->get_sum() << '\n';
			std::cout << "S_" << i << " : " << series->S_n(i) << '\n';
			std::cout << "T_" << i << " of order " << order << " : " << test->operator()(i, order) << '\n';
			std::cout << "T_" << i << " of order " << order << " - S_" << i << " : " << test->operator()(i, order) - series->S_n(i) << '\n';
			
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
		}
	}
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ, typename transform_type>
void cmp_sum_difference_and_transform(const T S, const K n, const K order, const series_templ& series, const transform_type& test)
{
	test->print_info();
	//S = series->S_n(1e6);
	//T log5 = 1.6094379124341003746007593332261876395256013542685177219126478914741789877076577646301338780931796107999663030217155628997240052293246761996336166174637057275521796374971832456534928562023415250572702;
	//T log2 = 0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754200148102057068573368552023575;
	//T pi4  = 0.78539816339744830961566084581987572104929234984377645524373614807695410157155224965700870633552926699553702162832057666177346115238764555793134;
	//T pi_squared6 = 1.644934066848226436472415166646025189218949901206798437735558229370007470403201;
	//T for_shenks = 0.7853981633974483096156608458198757210492923498437764552;
	//S = for_shenks;
	for (K i = 0; i <= n; ++i) {
		try 
		{
			std::cout << "Sum of algo :"	<< series->get_sum()	<< '\n';
			std::cout << "T of order  : "	<< order	<< ", i : " << i		<< '\n';
			std::cout << "S :\t"			<< S					<< '\n';
			std::cout << "S_"				<< i					<< " :\t"	<< series->S_n(i) << '\n';
			std::cout << "T_"				<< i					<< " :\t"	<< test->operator()(i, order) << '\n';

			std::cout << "|T_"	<< i	<< "\t- S_" << i << "| :\t" << abs(test->operator()(i, order)	- series->S_n(i))				<< '\n';
			std::cout << "|S"			<< "\t- S_" << i << "| :\t" << abs(S							- series->S_n(i))				<< '\n';
			std::cout << "|S"			<< "\t- T_" << i << "| :\t" << abs(S							- test->operator()(i, order))	<< '\n';
			std::cout << '\n';
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
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
template <std::unsigned_integral K, typename series_templ, typename transform_type>
void cmp_a_n_and_transform(const K n, const K order, const series_templ& series, const transform_type& test)
{
	test->print_info();
	for (K i = 1; i <= n; ++i) {
		try
		{
			std::cout << "a_" << i << " : " << (*series)(i) << '\n';
			std::cout << "t_" << i << " : " << test->operator()(i, order) - test->operator()(i - 1, order) << '\n';
			std::cout << "t_" << i << " of order " << order << " - a_" << i
				<< " : " << (test->operator()(i, order) - test->operator()(i - 1, order)) - (*series)(i) << '\n';
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
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
template <std::unsigned_integral K, typename series_templ, typename transform_type>
void transformation_remainders(const K n, const K order, const series_templ& series, const transform_type& test)
{
	std::cout << "Tranformation of order " << order << " remainders from i = 1 to " << n << '\n';
	test->print_info();
	for (K i = 1; i <= n; ++i) {
		try
		{
			std::cout << "S - T_" << i << " : " << series->get_sum() - test->operator()(i, order) << '\n';
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
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
template <std::unsigned_integral K, typename series_templ, typename transform_type_1, typename transform_type_2>
void cmp_transformations(const K n, const K order, const series_templ& series, const transform_type_1& test_1, const transform_type_2& test_2)
{
	std::cout << "Tranformations of order " << order << " remainders from i = 1 to " << n << '\n';
	std::cout << "The transformation #1 is ";
	test_1->print_info();
	std::cout << "The transformation #2 is ";
	test_2->print_info();
	auto diff_1 = (*series)(0);
	auto diff_2 = (*series)(0);
	for (K i = 1; i <= n; ++i) {
		try
		{
			diff_1 = series->get_sum() - test_1->operator()(i, order);
			diff_2 = series->get_sum() - test_2->operator()(i, order);
			std::cout << "The transformation #1: S - T_" << i << " : " << diff_1 << '\n';
			std::cout << "The transformation #2: S - T_" << i << " : " << diff_2 << '\n';
			if (std::abs(diff_1) < std::abs(diff_2))
				std::cout << "The transformation #1 is faster" << '\n';
			else
				std::cout << "The transformation #2 is faster" << '\n';
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
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
template <std::unsigned_integral K, typename series_templ, typename transform_type>
void eval_transform_time(const K n, const K order, const series_templ& series, const transform_type& test)
{
	const auto start_time = std::chrono::system_clock::now();
	test->print_info();
	for (K i = 1; i <= n; ++i) {
		try
		{
			test->operator()(i, order);
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
		}
	}
	const auto end_time = std::chrono::system_clock::now();
	const std::chrono::duration<double, std::milli> diff = end_time - start_time;
	std::cout << "It took " << diff.count() << " to perform these transformations" << '\n';
}


/*
* @brief Function that prints the terms of nontransformed partial sums
* Then it prints out the nth terms  of the series
* @authors Kreynin R.G.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms
*/
template <std::unsigned_integral K, typename series_templ>
void print_sum(const K n, const series_templ& series)
{
	std::cout << "Sum of algo :" << series->get_sum() << '\n';
	std::cout << "S_" << n << " : " << series->S_n(n) << '\n';
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
template <std::unsigned_integral K, typename transform_type>
void print_transform(const K n, const K order, const transform_type& test)
{	

	///*long*/ double S;
	//S = 0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754200148102057068573368552023575;
	test->print_info();
	try
	{
		std::cout << "T_" << n << " of order " << order << " : " << test->operator()(n, order) << '\n';
		//std::cout << "|S" << "\t- T_" << n << "| :\t" << abs(S - test->operator()(n, order)) << '\n';
	}
	catch (std::domain_error& e)
	{
		std::cout << e.what() << '\n';
	}
	catch (std::overflow_error& e)
	{
		std::cout << e.what() << '\n';
	}
}