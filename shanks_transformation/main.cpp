/**
 * @file main.cpp
 */

#include <string> 
#include "shanks_transformation.h"
#include "epsilon_algorithm.h"
#include "test_functions.h"


 /**
  * @brief Calculate the factorial of a given integer.
  * @authors Pashkov B.B., Bolshakov M.P.
  *
  * This function takes in an integer n as input and returns the factorial of n as an integer value.
  * It first checks if the input integer n is greater than 0, and if so, it calculates the factorial using a for loop.
  * If n is negative, it throws a domain_error exception with the message "negative integer in the input".
  *
  * @param n The input integer
  * @return The factorial of n
  *
  * @throw std::domain_error if n is negative
  */
int fact(int n)
{
	int f = 1;
	for (int i = 2; i <= n; i++)
		f *= i;
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return f;
}

/*!
	 * @param  x - a real number for which the exponential function needs to be calculated
	 * @authors Bolshakov M.P.
	 * @param  n - an integer representing the power for the exponential calculation
	 * @return the result of the calculation of x^n / n!
	 * @note  this is the expx function, which calculates the value of x^n / n! (exponential) for the given values of x and n.
	 * @throw std::domain_error if the input n is a negative integer
	 */
float exp_x(float x, int n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(x, n) / fact(n);
}

/*!
* @brief This is the four_arctan_x function, which calculates the value of 4 * arctan(x) for the given values of x and n.
* @authors Bolshakov M.P.
* 		 Parameter x - a real number for which the arctan function needs to be calculated.
* 		 Parameter n - an integer representing the power used in the calculation.
* 		 Return value - the result of the calculation of 4 * arctan(x).
* 		 The function calculates the value of 4 * arctan(x) using the formula: 4 * (-1)^n * x^(2*n+1) / (2*n+1).
* 		 If the input parameter n is a negative integer, a std::domain_error exception is thrown with the message "negative integer in the input"

   * @param  x a real number
   * @param  n an integer representing the power for the calculation
   * @return the result of calculating 4 * arctan(x) using the given values of x and n
   * @note calculates the value of 4 * arctan(x) by using the formula: 4 * (-1)^n * x^(2*n+1) / (2*n+1)
   * @throw std::domain_error if the input n is a negative integer
   */
float four_arctan_x(float x, unsigned int n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	if (std::abs(x) > 1)
		throw std::domain_error("the arctan series diverge at x = " + std::to_string(x));
	return 4 * (n % 2 ? -1 : 1) * pow(x, 2 * n + 1) / (2 * n + 1);
}


/**
 * @brief Calculate the value of x raised to the power of 2*n divided by the factorial of 2*n.
 * @authors Pashkov B.B.
 *
 * This function takes in a float x and an integer n as input and returns a float value.
 * It first checks if the input integer n is negative, and if so, it throws a domain_error exception with the message "negative integer in the input".
 * If n is not negative, the function calculates the value of x raised to the power of 2*n divided by the factorial of 2*n using the pow() and fact() functions.
 *
 * @param x The base value
 * @param n The exponent value
 * @return The result of x^(2n) / (2n)!
 *
 * @throw std::domain_error if n is negative
 */
float ch_x(float x, int n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(x, 2 * n) / fact(2 * n);
}

int main(void)
{

	/*
		DEBUGGING
	*/

#if DEBUGGING_MODE
	/*
	std::cout << "Which transformation would you like to test?" << std::endl << "0 - shanks_transformation, 1 - epsilon_algorithm";
	int transformation_id;
	std::cin >> transformation_id;
	switch(transformation_id)
	{
		case 0:
		shanks_transform<long double, long long int, exp_series<long double, long long int>> test2_1();
		break;
		case 1:
		epsilon_algorithm<long double, long long int, exp_series<long double, long long int>> test2_2();
		break;
		default:
		throw std::exception(); //todo
	}*/

#if 0
	shanks_transform<long double, int> test1_1(exp_x, 2.38);
	epsilon_algorithm<long double, int> test1_2(exp_x, 2.38);
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
		{
			std::cout << "SHANKS TRANSFORMATION of order " << i << std::endl;
			test1_1.print_t_n(j, i);
			test1_1.print_s_n(j);
			test1_1.print_diff_t_s(j, i);
		}

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
		{
			std::cout << "EPSILON ALGORITHM of order " << i << std::endl;
			test1_2.print_t_n(j, i);
			test1_2.print_s_n(j);
			test1_2.print_diff_t_s(j, i);
		}
#elif 1
	exp_series<long double, long long int> exp_1(1);
	shanks_transform<long double, long long int, exp_series<long double, long long int>> test2_1(exp_1);
	epsilon_algorithm<long double, long long int, exp_series<long double, long long int>> test2_2(exp_1);
	for (int i = 1; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
		{
			cmp_sum_and_transform<exp_series<long double, long long int>, epsilon_algorithm<long double, long long int, exp_series<long double, long long int>>>(j, i, exp_1);
			cmp_sum_and_transform<exp_series<long double, long long int>, shanks_transform<long double, long long int, exp_series<long double, long long int>>>(j, i, exp_1);
		}
	transformation_remainders<exp_series<long double, long long int>, shanks_transform<long double, long long int, exp_series<long double, long long int>>>(5, 2, exp_1);
	

#elif 0
	shanks_transform<long double, int> test3_1(ch_x, 2.00001);
	epsilon_algorithm<long double, int> test3_2(ch_x, 2.00001);
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
		{
			std::cout << "SHANKS TRANSFORMATION of order " << i << std::endl;
			test3_1.print_t_n(j, i);
			test3_1.print_s_n(j);
			test3_1.print_diff_t_s(j, i);
		}

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
		{
			std::cout << "EPSILON ALGORITHM of order " << i << std::endl;
			test3_2.print_t_n(j, i);
			test3_2.print_s_n(j);
			test3_2.print_diff_t_s(j, i);
		}
#endif
#endif
	return 0;
}