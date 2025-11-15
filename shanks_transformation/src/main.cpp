
#include <math.h> // is needed

#include "../libs/arbitrary_arithmetics/complexprecision.h"
#include "../libs/arbitrary_arithmetics/fprecision.h"
#include "../libs/arbitrary_arithmetics/precisioncore.cpp"

#include "../include/test_framework/test_framework.hpp"


int main()
{

    while(true){
		try
		{
			//main_testing_function<complex_precision<float_precision>, unsigned long long int>();
			//main_testing_function<float_precision, unsigned long long int >();
			//main_testing_function<long double, unsigned long long int>();
			main_testing_function<double, unsigned int>();
			//main_testing_function<float, unsigned short int>();
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << "\n";
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << "\n";
		}
		catch (float_precision::divide_by_zero& e){
			std::cout << "float_precision::division by zero\n";
		} catch (complex_precision<float_precision>::divide_by_zero& e){
			std::cout << "float_precision::division by zero\n";
		}
		catch (float_precision::domain_error& e){
			std::cout << "float_precision::domain error\n";
		}
	}
	return 0;
}
