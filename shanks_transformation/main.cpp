#include "shanks_transformation.h"

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
	shanks_transform<float> test(&exp_x, 2.5);
	test.print_t_n(5,1);
	test.print_s_n(5);
	shanks_transform<float> test1(&four_arctan_x, 1);
	test1.print_t_n(5,1);
	test1.print_s_n(5);
	shanks_transform<float> test2(&ch_x, 2.00001);
	test2.print_t_n(3,1);
	test2.print_s_n(3);
	shanks_transform<long double> test3(&four_arctan_x, 1);
	test3.print_t_n(3, 1);
	test3.print_t_n(2, 2);
	test3.print_t_n(3, 2);
	test3.print_t_n(2, 3);
	test3.print_t_n(3, 4);
	test3.print_t_n(6, 2);
	//test1.print_t_n(6, 3); //TO DO - THIS EXPRESSION GIVES NAN (HOW UNEXPECTED...) - BORYA WE NEED TO DO SMTH ABOUT IT
	return 0;
}