/**
 * @file main.cpp
 */

#include <string> 
#include <set>
#include "shanks_transformation.h"
#include "epsilon_algorithm.h"
#include "test_functions.h"

#if DEBUGGING_MODE

 /**
 * @brief prints out all available series for testing
 * @authors Bolshakov M.P.
 */
inline void print_series_info()
{
	std::cout << "Which series' convergence would you like to accelerate?" << std::endl <<
		"List of currently avaiable series:" << std::endl <<
		"1 - exp_series" << std::endl <<
		"2 - four_arctan_series" << std::endl <<
		"3 - cosh_series" << std::endl <<
		"4 - ln1mx_series" << std::endl <<
		"5 - mean_sinh_sin_series" << std::endl <<
		"6 - exp_squared_erf_series" << std::endl <<
		"7 - xmb_Jb_two_series" << std::endl <<
		"8 - half_asin_two_x_series" << std::endl <<
		"9 - sin_series" << std::endl;
}

/**
* @brief prints out all available transformations for testing
* @authors Bolshakov M.P.
*/
inline void print_transformation_info()
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
inline void print_test_function_info()
{
	std::cout << "Which function would you like to use for testing?" << std::endl <<
		"List of currently avaiable functions:" << std::endl <<
		"1 - T_n - S_n" << std::endl <<
		"2 - T_n - S" << std::endl;
}



/**
* @brief The main testing function
* This function provides a convenient and interactive way to test out the convergence acceleration of various series 
* @tparam T The type of the elements in the series, K The type of enumerating integer
* @authors Bolshakov M.P.
*/
template <typename T, typename K>
inline void main_testing_function()
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
	std::set<int> alternating_series = { 2, 7, 9 };
	switch (series_id)
	{
	case 1:
		series.reset(new exp_series<T, K>(x));
		break;
	case 2:
		series.reset(new four_arctan_series<T, K>(x));
		break;
	case 3:
		series.reset(new cosh_series<T, K>(x));
		break;
	case 4:
		series.reset(new ln1mx_series<T, K>(x));
		break;
	case 5:
		series.reset(new mean_sinh_sin_series<T, K>(x));
		break;
	case 6:
		series.reset(new exp_squared_erf_series<T, K>(x));
		break;
	case 7:
		K b;
		std::cout << "Enter the value for constant b for the series" << std::endl;
		std::cin >> b;
		series.reset(new xmb_Jb_two_series<T, K>(x, b));
		break;
	case 8:
		series.reset(new half_asin_two_x_series<T, K>(x));
		break;
	case 9:
		series.reset(new sin_series<T, K>(x));
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
	case 1:
		if (alternating_series.contains(series_id))
			transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
		else
			transform.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
		break;
	case 2:
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
	switch (function_id)
	{
	case 1:
		std::cout << "Enter n and order:" << std::endl;
		std::cin >> n >> order;
		cmp_sum_and_transform(n, order, series.get(), transform.get());
		break;
	case 2:
		std::cout << "Enter n and order:" << std::endl;
		std::cin >> n >> order;
		transformation_remainders(n, order, series.get(), transform.get());
		break;
	default:
		throw std::domain_error("wrong function_id");
	}
}

#endif

int main(void)
{

	/*
		DEBUGGING
	*/

#if DEBUGGING_MODE
	main_testing_function<long double, long long int>();
	main_testing_function<float, short int>();
	main_testing_function<double, int>();
#endif
	return 0;
}