#include "shanks_transformation.h"
#include "epsilon_algorithm.h"

int fact(int n)
{
	int f = 1;
	if (n > 0)
	{
		for (int i = 2; i <= n; i++)
		{
			f *= i;
		}
	}
	else if (n < 0)
		throw std::domain_error("negative integer in the input");
	return f;
}

float exp_x(float x, int n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(x, n) / fact(n);
}

float four_arctan_x(float x, int n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return 4*pow((-1), n % 2) * pow(x, 2 * n + 1) / (2 * n + 1);
}

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

#if 0
	shanks_transform<long double> test1_1(exp_x, 2.38);
	epsilon_algorithm<long double> test1_2(exp_x, 2.38);
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
	shanks_transform<long double> test2_1(four_arctan_x, 1);
	epsilon_algorithm<long double> test2_2(four_arctan_x, 1);
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
		{
			try 
			{
				std::cout << "SHANKS TRANSFORMATION of order " << i << std::endl;
				test2_1.print_t_n(j, i);
				test2_1.print_s_n(j);
				test2_1.print_diff_t_s(j, i);
			}
			catch (std::overflow_error& e)
			{
				std::cout << e.what() << std::endl;
			}
			catch (std::domain_error& e)
			{
				std::cout << e.what() << std::endl;
			}
		}

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
		{
			try
			{
				std::cout << "EPSILON ALGORITHM of order " << i << std::endl;
				test2_2.print_t_n(j, i);
				test2_2.print_s_n(j);
				test2_2.print_diff_t_s(j, i);
			}
			catch (std::overflow_error& e)
			{
				std::cout << e.what() << std::endl;
			}
			catch (std::domain_error& e)
			{
				std::cout << e.what() << std::endl;
			}
		}
#elif 0
	shanks_transform<long double> test3_1(ch_x, 2.00001);
	epsilon_algorithm<long double> test3_2(ch_x, 2.00001);
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
	return 0;
}