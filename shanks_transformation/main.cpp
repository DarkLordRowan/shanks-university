/**
 * @file main.cpp
 * @brief testing out series_acceleration and series subclasses
 * This project contains the following:
 * 1) Series_acceleration base class in series_acceleration.h. Its subclasses are different variations of shanks transformations: shanks_transformation.h, epsilon_algorithm.h
 * 2) Series base class and its subclasses in series.h. They are the ones being accelerated
 * 3) Testing functions in test_functions.h. Functions that can be called in main to test how series_acceleration and series_base subclasses work and cooperate.
 * It is recommended you look up doxygen documentation on our repository https://katerina-evdokimova.github.io/shanks-university/ to convinently figure out what's everything for
 */
#include <memory>
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
		"12 - half_asin_two_x_series" << std::endl;
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
		"4 - cmp_transformations - showcases the difference between convergence of sums accelerated by different transformations" << std::endl;
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
	std::set<int> alternating_series = { 2, 3, 7, 11 };
	switch (series_id)
	{
	case 1:
		series.reset(new exp_series<T, K>(x));
		break;
	case 2:
		series.reset(new cos_series<T, K>(x));
		break;
	case 3:
		series.reset(new sin_series<T, K>(x));
		break;
	case 4:
		series.reset(new cosh_series<T, K>(x));
		break;
	case 5:
		series.reset(new sinh_series<T, K>(x));
		break;
	case 6:
		T alpha;
		std::cout << "Enter the value for constant alpha for the series" << std::endl;
		std::cin >> alpha;
		series.reset(new bin_series<T, K>(x, alpha));
		break;
	case 7:
		series.reset(new four_arctan_series<T, K>(x));
		break;
	case 8:
		series.reset(new ln1mx_series<T, K>(x));
		break;
	case 9:
		series.reset(new mean_sinh_sin_series<T, K>(x));
		break;
	case 10:
		series.reset(new exp_squared_erf_series<T, K>(x));
		break;
	case 11:
		K b;
		std::cout << "Enter the value for constant b for the series" << std::endl;
		std::cin >> b;
		series.reset(new xmb_Jb_two_series<T, K>(x, b));
		break;
	case 12:
		series.reset(new half_asin_two_x_series<T, K>(x));
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
	std::cout << "Enter n and order:" << std::endl;
	std::cin >> n >> order;
	switch (function_id)
	{
	case 1:
		cmp_sum_and_transform(n, order, series.get(), transform.get());
		break;
	case 2:
		cmp_a_n_and_transform(n, order, series.get(), transform.get());
		break;
	case 3:
		transformation_remainders(n, order, series.get(), transform.get());
		break;
	case 4:
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
		cmp_transformations(n, order, series.get(), transform.get(), transform2.get());
		break;
	}
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
	try
	{
		//TODO: find a succinct way to test out various digit types
		main_testing_function<long double, long long int>();
		main_testing_function<double, int>();
		main_testing_function<float, short int>();
	}
	catch (std::domain_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (std::overflow_error& e)
	{
		std::cout << e.what() << std::endl;
	}
#endif
	return 0;
}