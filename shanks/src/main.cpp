
//#include "../libs/arbitrary_arithmetics/complexprecision.h"
//#include "../libs/arbitrary_arithmetics/fprecision.h"
//#include "../libs/arbitrary_arithmetics/intervalprecision.h"
//#include "../libs/arbitrary_arithmetics/precisioncore.cpp"

#include "../include/test_framework/test_framework.hpp"

#include "../libs/mpfr/mpreal.h"
#include "../include/filters/savgol.hpp"
#include "../include/filters/kolzur.hpp"

int main()
{

    while(true){
		try
		{
			//main_testing_function<long double, unsigned long long int>();
			main_testing_function<std::complex<long double>, unsigned long long int>();
			//main_testing_function<std::complex<mpfr::mpreal>, unsigned short int>();
			//main_testing_function<mpfr::mpreal, unsigned short int>();
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << "\n";
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << "\n";
		}
	}
	return 0;
}
