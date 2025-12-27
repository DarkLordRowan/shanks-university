
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
	std::vector<float> res = kolzur_filter<float>({1,2,3,4,5,6,7,8,9,10},5,3);
	for(const auto& x : res){
		std::cout << x << " ";
	}
	std::cout << "\n";
	//std::vector<mpfr::mpreal> test_data = {1,2,3,20,5,6,7,8,9,10};
	//std::vector<mpfr::mpreal> res = savgol_filter(test_data, 4, 3, 0, mpfr::mpreal(1.0, mpfr::digits2bits(100)));
	//for(const auto& x : res){
	//	std::cout << utils::to_string(x) << " ";
	//}
	//test[0] += mpfr::mpreal(3);
	//std::cout << test[0].get_prec();
	//std::cout << utils::get_precision(a) << "\n";
	//std::cout << (a/b).toString() << "\n";
	//Eigen::Matrix<mpfr::mpreal, Eigen::Dynamic, Eigen::Dynamic> A;
	//A.resize(3,3);
 	//A << 1, 2, 1, 2, 1, 0, -1, 1, 2;
 	//std::cout << "Here is the matrix A:\n" << A << std::endl;
 	//std::cout << "The determinant of A is " << A.determinant() << std::endl;
 	//std::cout << "The inverse of A is:\n" << A.inverse() << std::endl;
	//std::cout << a << "\n";

    //while(true){
	//	try
	//	{
	//		//main_testing_function<complex_precision<float_precision>, unsigned long long int>();
	//		//main_testing_function<complex_precision<double>, unsigned long long int>();
	//		//main_testing_function<float_precision, unsigned long long int >();
	//		//main_testing_function<long double, unsigned long long int>();
	//		//main_testing_function<interval<double>, unsigned int>();
	//		//main_testing_function<interval<float_precision>, unsigned int>();
	//		main_testing_function<std::complex<mpfr::mpreal>, unsigned short int>();
	//	}
	//	catch (std::domain_error& e)
	//	{
	//		std::cout << e.what() << "\n";
	//	}
	//	catch (std::overflow_error& e)
	//	{
	//		std::cout << e.what() << "\n";
	//	}
	//	#ifdef INC_FPRECISION
	//	catch (float_precision::divide_by_zero& e){
	//		std::cout << "float_precision::division by zero\n";
	//	} catch (complex_precision<float_precision>::divide_by_zero& e){
	//		std::cout << "float_precision::division by zero\n";
	//	}
	//	catch (float_precision::domain_error& e){
	//		std::cout << "float_precision::domain error\n";
	//	}
	//	#endif
	//}
	return 0;
}
