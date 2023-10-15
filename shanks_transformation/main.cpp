#include "shanks_transformation.h"

int fact(int n)
{
	return n > 0 ? fact(n - 1) * n : 1;
}

float exp_x(float x, int n)
{
	return pow(x, n) / fact(n);
}

float four_arctan_x(float x, int n)
{
	return 4*pow((-1), n % 2) * pow(x, 2 * n + 1) / (2 * n + 1);
}

int main(void)
{
	shanks_transform<float> test(&exp_x, 2.5);
	test.print_t_n(5);
	test.print_s_n(5);
	shanks_transform<float> test2(&four_arctan_x, 1);
	test2.print_t_n(5);
	test2.print_s_n(5);
	return 0;
}