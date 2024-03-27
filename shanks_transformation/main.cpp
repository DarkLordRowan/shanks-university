/**
 * @file main.cpp
 * @brief testing out series_acceleration and series subclasses
 * This project contains the following:
 * 1) Series_acceleration base class in series_acceleration.h. Its subclasses are different variations of shanks transformations: shanks_transformation.h, epsilon_algorithm.h
 * 2) Series base class and its subclasses in series.h. They are the ones being accelerated
 * 3) Testing functions in test_functions.h. Functions that can be called in main to test how series_acceleration and series_base subclasses work and cooperate.
 * 4) Framework for testing in test_framework.h
 * It is recommended you look up doxygen documentation on our repository https://katerina-evdokimova.github.io/shanks-university/ to convinently figure out what's everything for
 */
#include "test_framework.h"

int main(void)
{
	try
	{
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
	return 0;
}