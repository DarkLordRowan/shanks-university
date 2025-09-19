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

#include <math.h>

#include "libs/arbitrary_arithmetics/complexprecision.h"
#include "libs/arbitrary_arithmetics/fprecision.h"

#define USE_COMPLEX

#include "test_framework.h"

#include "libs/arbitrary_arithmetics/precisioncore.cpp"


int main()
{
	while(true){
		try
		{
			float_precision_ctrl.precision(100);
			//main_testing_function<complex_precision<float_precision>, unsigned long long int>();
			main_testing_function<float_precision, unsigned long long int >();
			//main_testing_function<long double, unsigned long long int>();
			//main_testing_function<double, unsigned int>();
			//main_testing_function<float, unsigned short int>();
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (float_precision::divide_by_zero& e){
			std::cout << "float_precision::division by zero" << std::endl;
		}
		catch (float_precision::domain_error& e){
			std::cout << "float_precision::domain error" << std::endl;
		}

	}

	return 0;
}