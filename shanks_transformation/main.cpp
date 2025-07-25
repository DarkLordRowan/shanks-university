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
//#include <gmpxx.h>
#include "../libs/arb_arithm/precisioncore.cpp"
#include "../libs/arb_arithm/complexprecision.h"
#include <cmath>
#include <iostream>

std::string to_string(const float_precision& value){
	return value.toString();
}

std::string to_string(const complex_precision<float_precision>& value){
	return value.real().toString() + " + i * " + value.imag().toString();
}

float_precision hypot(float_precision value1, float_precision value2){
	return sqrt(value1.square() + value2.square());
}

complex_precision<float_precision> hypot(complex_precision<float_precision> value1, complex_precision<float_precision> value2){
	return sqrt(norm(value1-value2));
}

complex_precision<float_precision> erf(complex_precision<float_precision> value){
	//complex_precision<float_precision> res(0);
	//complex_precision<float_precision> member = complex_precision<float_precision>(2)/sqrt(complex_precision<float_precision>(_PI))*value;
	//res+=member;
	//for(size_t n = 1; n <= 1000; n++){
	//	member *= value * value / complex_precision<float_precision>(n);
	//	res += complex_precision<float_precision>(pow(-1,n)) / complex_precision<float_precision>(2*n+1) * member;
	//}
	return complex_precision<float_precision>(erf(value.real()));
}

complex_precision<float_precision> cyl_bessel_j(complex_precision<float_precision> value, complex_precision<float_precision> mu){
	return complex_precision<float_precision>(cyl_bessel_j(value.real(), mu.real()));
}

bool operator>(const complex_precision<float_precision>& a, const complex_precision<float_precision>& b){
	return a.real() > b.real();
}

bool operator<=(const complex_precision<float_precision>& a, const complex_precision<float_precision>& b){
	return a.real() <= b.real();
}

complex_precision<float_precision> tgamma(const complex_precision<float_precision> value){
	if(value.real().sign() > 0) return complex_precision<float_precision>(tgamma(value.real()));

}

float_precision cyl_bessel_j(float_precision nu, float_precision x){
	return float_precision(std::cyl_bessel_j((double)nu,(double)x));
}

bool isfinite(complex_precision<float_precision> value){
	return isfinite(value.real()) || isfinite(value.imag());
}

complex_precision<float_precision> fma(complex_precision<float_precision> value1, complex_precision<float_precision> value2, complex_precision<float_precision> value3){
	return value1 * value2 - value3;

}
int main(void)
{
	complex_precision<float_precision> test(0,6);
	std::cout << exp(test) << "\n";

	while(true){
		try
		{
			main_testing_function<complex_precision<float_precision>, unsigned long int>();
			//main_testing_function<float_precision, unsigned long int>();
			main_testing_function<long double, unsigned long long int>();
			main_testing_function<double, unsigned int>();
			main_testing_function<float, unsigned short int>();
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return 0;
}