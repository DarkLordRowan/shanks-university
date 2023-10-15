#include "shanks_transformation.h"

float exp_x(float x, int n)
{
	return pow(x, n) / fact(n);
}

int main(void)
{
	shanks_transform<float> test(&exp_x, 2.5);
	test.print_t_n(1);
	test.print_s_n(1);
	return 0;
}