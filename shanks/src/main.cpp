
//#include "../libs/arbitrary_arithmetics/complexprecision.h"
//#include "../libs/arbitrary_arithmetics/fprecision.h"
//#include "../libs/arbitrary_arithmetics/intervalprecision.h"
//#include "../libs/arbitrary_arithmetics/precisioncore.cpp"

#include "../include/test_framework/test_framework.hpp"

#include "../libs/mpfr/mpreal.h"
#include "../include/filters/savgol.hpp"
#include "../include/filters/kolzur.hpp"


/**
 * @file main.cpp
 * @brief Main file for launching application.
 * 
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

int main()
{

	for(const auto& x : shanks::series::erf_series<mpfr::mpreal, size_t>().generate_series(mpfr::mpreal(1, mpfr::digits2bits(1000)), 1000).Sn){
		std::cout << utils::to_string(x) << "\n";
	}

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
