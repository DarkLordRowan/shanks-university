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

#include <math.h>

#include "libs/arbitrary_arithmetics/complexprecision.h"
#include "libs/arbitrary_arithmetics/fprecision.h"
#include "libs/arbitrary_arithmetics/iprecision.h"
#include "libs/arbitrary_arithmetics/precisioncore.cpp"

#include "methods/algorithm/brezinski_theta_algorithm.hpp"
#include "series.hpp"
#include "methods.hpp"
#include "series/series_base.hpp"
#include "noise/noise_generator.hpp"

template<UnsignedIntLike K>
void test_on_types(){

	using T1 = float;
	using T2 = double;
	using T3 = long double;
	using T4 = float_precision;
	using T5 = complex_precision<float>;
	using T6 = complex_precision<double>;
	using T7 = complex_precision<long double>;
	using T8 = complex_precision<float_precision>;


	exp_series<T1, K>   test1 = exp_series<T1, K>(static_cast<T1>(2));
	SeriesResult<T1>   result1 = test1.generateSeries(10);

	exp_series<T2, K>   test2 = exp_series<T2, K>(static_cast<T2>(2));
	SeriesResult<T2>   result2 = test2.generateSeries(10);

	exp_series<T3, K>   test3 = exp_series<T3, K>(static_cast<T3>(2));
	SeriesResult<T3>   result3 = test3.generateSeries(10);

	exp_series<T4, K>   test4 = exp_series<T4, K>(static_cast<T4>(2));
	SeriesResult<T4>   result4 = test4.generateSeries(10);

	exp_series<T5, K>   test5 = exp_series<T5, K>(static_cast<T5>(2));
	SeriesResult<T1>   result5 = test1.generateSeries(10);

	exp_series<T6, K>   test6 = exp_series<T6, K>(static_cast<T6>(2));
	SeriesResult<T2>   result6 = test2.generateSeries(10);

	exp_series<T7, K>   test7 = exp_series<T7, K>(static_cast<T7>(2));
	SeriesResult<T3>   result7 = test3.generateSeries(10);

	exp_series<T8, K>   test8 = exp_series<T8, K>(static_cast<T8>(2));
	SeriesResult<T8>   result8 = test8.generateSeries(10);

}

template<AcceptedLike T, UnsignedIntLike K>
void testCompatability(){

	test_on_types<size_t>();
	test_on_types<int_precision>();

	brezinski_theta_algorithm<T, K> testAlgo{};

	exp_series<T, K> testSeries = exp_series<T, K>(static_cast<T>(6));
	SeriesResult<T> result = testSeries.generateSeries(30);

	testAlgo.reset(result.Sn, result.an);
	testAlgo.print_info();

	for(size_t j = 0; j <= 14; j+= 2){
		std::cout << "n = order = " << j << " : " << testAlgo(j,j) << "\n";
	}

}

void TestNoise() {
	Noise <float_t> noise_normal(30, -0.1, 0.1, normal);
	exp_series<float_t, size_t> series(10);
	SeriesResult<float_t> result = series.generateSeries(30);
	Noise <float_t> noise_uniform(30, -0.1, 0.1, uniform);
	SeriesResult<float_t> result_jittered_normal = result;
	SeriesResult<float_t> result_jittered_uniform = result;
	result_jittered_normal.Sn[0] += noise_normal.seq[0];
	result_jittered_uniform.Sn[0] += noise_uniform.seq[0];
	for (int j = 1; j < 30; ++j) {
		result_jittered_normal.Sn[j] += noise_normal.seq[j];
		result_jittered_normal.an[j] += noise_normal.seq[j] - noise_normal.seq[j-1];
		result_jittered_uniform.Sn[j] += noise_uniform.seq[j];
		result_jittered_uniform.an[j] += noise_uniform.seq[j] - noise_uniform.seq[j-1];
	}
	brezinski_theta_algorithm<float_t, size_t> algo{};

	algo.reset(result.Sn, result.an);
	algo.print_info();
	std::cout << "Without noise:\n";
	for(size_t j = 0; j <= 10; j+= 2) {
		std::cout << "n = order = " << j << " : " << algo(j,j) << "\n";
	}

	algo.reset(result_jittered_normal.Sn, result_jittered_normal.an);
	std::cout << "With normal noise:\n";
	for(size_t j = 0; j <= 10; j+= 2) {
		std::cout << "n = order = " << j << " : " << algo(j,j) << "\n";
	}

	algo.reset(result_jittered_uniform.Sn, result_jittered_uniform.an);
	std::cout << "With uniform noise:\n";
	for(size_t j = 0; j <= 10; j+= 2) {
		std::cout << "n = order = " << j << " : " << algo(j,j) << "\n";
	}
}


int main()
{

	using typeA = complex_precision<long double>;
	using typeB = int_precision;


	// using typeA = complex_precision<long double>;
	// using typeB = int_precision;

	//std::cout << fact<typeB>(6) << "\n";
	//std::cout << double_fact<typeB>(6) << "\n";
	//std::cout << binomial_coefficient<typeB>(12, 5) << "\n";

	//std::cout << fact<typeB>(7) << "\n";
	//std::cout << double_fact<typeB>(7) << "\n";
	//std::cout << binomial_coefficient<typeB>(13, 5) << "\n";

	// testCompatability<typeA, typeB>();
	//std::cout << fact<typeB>(6) << "\n";
	//std::cout << double_fact<typeB>(6) << "\n";
	//std::cout << binomial_coefficient<typeB>(12, 5) << "\n";

	//std::cout << fact<typeB>(7) << "\n";
	//std::cout << double_fact<typeB>(7) << "\n";
	//std::cout << binomial_coefficient<typeB>(13, 5) << "\n";


	// testCompatability<typeA, typeB>();

	/*
	while(true){
		try
		{
			float_precision_ctrl.precision(50);
			main_testing_function<complex_precision<float_precision>, unsigned long long int>();
			//main_testing_function<float_precision, unsigned long long int >();
			//main_testing_function<long double, unsigned long long int>();
			//main_testing_function<double, unsigned int>();
			//main_testing_function<float, unsigned short int>();
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (float_precision::divide_by_zero& e){
			std::cout << "float_precision::division by zero" << std::endl;
		}
		catch (float_precision::domain_error& e){
			std::cout << "float_precision::domain error" << std::endl;
		}

	}
	*/
	return 0;
}
