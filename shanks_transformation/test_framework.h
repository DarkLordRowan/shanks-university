/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 */

#pragma once
#include <memory>
#include <string> 
#include <set>
#include "shanks_transformation.h"
#include "epsilon_algorithm.h"
#include "test_functions.h"

enum transformation_id_t {
	null_transformation_id, 
	shanks_transformation_id, 
	epsilon_algorithm_id
};
enum series_id_t {
	null_series_id, 
	exp_series_id, 
	cos_series_id, 
	sin_series_id, 
	cosh_series_id,
	sinh_series_id, 
	bin_series_id, 
	four_arctan_series_id, 
	ln1mx_series_id, 
	mean_sinh_sin_series_id,
	exp_squared_erf_series_id, 
	xmb_Jb_two_series_id, 
	half_asin_two_x_series_id,
	inverse_1mx_series_id,
	x_1mx_squared_series_id,
	erf_series_id,
	m_fact_1mx_mp1_inverse_series_id,
	inverse_sqrt_1m4x_series_id,
	one_twelfth_3x2_pi2_series_id,
	x_twelfth_x2_pi2_series_id,
	ln2_series_id,
	one_series_id,
	minus_one_quarter_series_id

};

enum test_function_id_t {
	null_test_function_id, 
	cmp_sum_and_transform_id, 
	cmp_a_n_and_transform_id, 
	transformation_remainder_id, 
	cmp_transformations_id,
	eval_transform_time_id
};

/**
* @brief prints out all available series for testing
* @authors Bolshakov M.P.
*/
inline static void print_series_info()
{
	std::cout << "Which series' convergence would you like to accelerate?" << std::endl <<
		"List of currently avaiable series:" << std::endl <<
		"1 - exp_series" << std::endl <<
		"2 - cos_series" << std::endl <<
		"3 - sin_series" << std::endl <<
		"4 - cosh_series" << std::endl <<
		"5 - sinh_series" << std::endl <<
		"6 - bin_series" << std::endl <<
		"7 - four_arctan_series" << std::endl <<
		"8 - ln1mx_series" << std::endl <<
		"9 - mean_sinh_sin_series" << std::endl <<
		"10 - exp_squared_erf_series" << std::endl <<
		"11 - xmb_Jb_two_series" << std::endl <<
		"12 - half_asin_two_x_series" << std::endl <<
		"13 - inverse_1mx_series" << std::endl <<
		"14 - x_1mx_squared_series" << std::endl <<
		"15 - erf_series" << std::endl <<
		"16 - m_fact_1mx_mp1_inverse_series" << std::endl <<
		"17 - inverse_sqrt_1m4x_series" << std::endl <<
		"18 - one_twelfth_3x2_pi2_series" << std::endl <<
		"19 - x_twelfth_x2_pi2_series" << std::endl <<
		"20 - ln2_series_id" << std::endl <<
		"21 - one_series_id" << std::endl <<
		"22 - minus_one_quarter_series_id" << std::endl;
}

/**
* @brief prints out all available transformations for testing
* @authors Bolshakov M.P.
*/
inline static void print_transformation_info()
{
	std::cout << "Which transformation would you like to test?" << std::endl <<
		"List of currently avaiable series:" << std::endl <<
		"1 - Shanks Transformation" << std::endl <<
		"2 - Epsilon Algorithm" << std::endl;
}

/**
* @brief prints out all available fungus for testing
* @authors Bolshakov M.P.
*/
inline static void print_test_function_info()
{
	std::cout << "Which function would you like to use for testing?" << std::endl <<
		"List of currently avaiable functions:" << std::endl <<
		"1 - cmp_sum_and_transform - showcases the difference between the transformed partial sum and the nontransformed one" << std::endl <<
		"2 - cmp_a_n_and_transform - showcases the difference between series' terms and transformed ones" << std::endl <<
		"3 - transformation_remainders - showcases the difference between series' sum and transformed partial sum" << std::endl <<
		"4 - cmp_transformations - showcases the difference between convergence of sums accelerated by different transformations" << std::endl <<
		"5 - eval_transform_time - evaluates the time it takes to transform series" << std::endl;
}

/**
* @brief The main testing function
* This function provides a convenient and interactive way to test out the convergence acceleration of various series
* @tparam T The type of the elements in the series, K The type of enumerating integer
* @authors Bolshakov M.P.
*/
template <typename T, typename K>
inline static void main_testing_function()
{

	//choosing series
	print_series_info();
	std::unique_ptr<series_base<T, K>> series;
	int series_id = 0;
	std::cin >> series_id;

	//choosing x
	std::cout << "Enter x - the argument for the functional series" << std::endl;
	T x = 0;
	std::cin >> x;

	//choosing series (cont.)
	std::set<int> alternating_series = { 2, 3, 7, 11, 15, 18, 19 };
	switch (series_id)
	{
	case series_id_t::exp_series_id:
		series.reset(new exp_series<T, K>(x));
		break;
	case series_id_t::cos_series_id:
		series.reset(new cos_series<T, K>(x));
		break;
	case series_id_t::sin_series_id:
		series.reset(new sin_series<T, K>(x));
		break;
	case series_id_t::cosh_series_id:
		series.reset(new cosh_series<T, K>(x));
		break;
	case series_id_t::sinh_series_id:
		series.reset(new sinh_series<T, K>(x));
		break;
	case series_id_t::bin_series_id:
		T alpha;
		std::cout << "Enter the value for constant alpha for the series" << std::endl;
		std::cin >> alpha;
		series.reset(new bin_series<T, K>(x, alpha));
		break;
	case series_id_t::four_arctan_series_id:
		series.reset(new four_arctan_series<T, K>(x));
		break;
	case series_id_t::ln1mx_series_id:
		series.reset(new ln1mx_series<T, K>(x));
		break;
	case series_id_t::mean_sinh_sin_series_id:
		series.reset(new mean_sinh_sin_series<T, K>(x));
		break;
	case series_id_t::exp_squared_erf_series_id:
		series.reset(new exp_squared_erf_series<T, K>(x));
		break;
	case series_id_t::xmb_Jb_two_series_id:
		K b;
		std::cout << "Enter the value for constant b for the series" << std::endl;
		std::cin >> b;
		series.reset(new xmb_Jb_two_series<T, K>(x, b));
		break;
	case series_id_t::half_asin_two_x_series_id:
		series.reset(new half_asin_two_x_series<T, K>(x));
		break;
	case series_id_t::inverse_1mx_series_id:
		series.reset(new inverse_1mx_series<T, K>(x));
		break;
	case series_id_t::x_1mx_squared_series_id:
		series.reset(new x_1mx_squared_series<T, K>(x));
		break;
	case series_id_t::erf_series_id:
		series.reset(new erf_series<T, K>(x));
		break;
	case series_id_t::m_fact_1mx_mp1_inverse_series_id:
		T m;
		std::cout << "Enter the value for constant m for the series" << std::endl;
		std::cin >> m;
		series.reset(new m_fact_1mx_mp1_inverse_series<T, K>(x, m));
		break;
	case series_id_t::inverse_sqrt_1m4x_series_id:
		series.reset(new inverse_sqrt_1m4x_series<T, K>(x));
		break;
	case series_id_t::one_twelfth_3x2_pi2_series_id:
		series.reset(new one_twelfth_3x2_pi2_series<T, K>(x));
		break; 
	case series_id_t::x_twelfth_x2_pi2_series_id:
		series.reset(new x_twelfth_x2_pi2_series<T, K>(x));
		break;
	case series_id_t::ln2_series_id:
		series.reset(new ln2_series<T, K>());
		break;
	case series_id_t::one_series_id:
		series.reset(new one_series<T, K>());
		break;
	case series_id_t::minus_one_quarter_series_id:
		series.reset(new minus_one_quarter_series<T, K>());
		break;
	default:
		throw std::domain_error("wrong series_id");
	}



	//choosing transformation
	print_transformation_info();
	int transformation_id = 0;
	std::cin >> transformation_id;
	std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform;
	switch (transformation_id)
	{
	case transformation_id_t::shanks_transformation_id:
		if (alternating_series.contains(series_id))
			transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
		else
			transform.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::epsilon_algorithm_id:
		transform.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	default:
		throw std::domain_error("wrong transformation_id");
	}

	//choosing testing function
	print_test_function_info();
	int function_id = 0;
	std::cin >> function_id;
	int n = 0;
	int order = 0;
	std::cout << "Enter n and order:" << std::endl;
	std::cin >> n >> order;
	switch (function_id)
	{
	case test_function_id_t::cmp_sum_and_transform_id:
		cmp_sum_and_transform(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::cmp_a_n_and_transform_id:
		cmp_a_n_and_transform(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::transformation_remainder_id:
		transformation_remainders(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::cmp_transformations_id:
	{
		/*std::cout << "choose the type of the other";*/ //so far we've only got 2 transformations
		std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform2;
		if (transformation_id == 1)
			transform2.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
		else //transformation_id is 2
		{
			if (alternating_series.contains(series_id))
				transform2.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform2.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
		}
		cmp_transformations(n, order, std::move(series.get()), std::move(transform.get()), std::move(transform2.get()));
		break;
	}
	case test_function_id_t::eval_transform_time_id:
		eval_transform_time(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	default:
		throw std::domain_error("wrong function_id");
	}
}