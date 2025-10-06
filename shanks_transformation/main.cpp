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

#include <unordered_map>

#include <math.h> // is needed

#include "libs/arbitrary_arithmetics/complexprecision.h"
#include "libs/arbitrary_arithmetics/fprecision.h"
#include "libs/arbitrary_arithmetics/precisioncore.cpp"

#include "methods.hpp"
#include "series.hpp"
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
constexpr void testCompatability(const T& x){
	
	std::unordered_map<transformation_id_t, std::function<std::unique_ptr<series_acceleration<T,K>>(void)>> algoInit = {
	    {              brezinski_theta_transformation_id, [](){ return std::make_unique<brezinski_theta_algorithm<T, K>>();                                  }},
		{                 drummond_d_u_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::u_variant       , false);}}, 
        {                 drummond_d_t_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_variant       , false);}},
        {            drummond_d_t_wave_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_wave_variant  , false);}},
        {                 drummond_d_v_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_variant       , false);}},
        {            drummond_d_v_wave_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_wave_variant  , false);}}, 
		{       recurrent_drummond_d_u_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::u_variant       ,  true);}},
    	{       recurrent_drummond_d_t_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_variant       ,  true);}},
     	{       recurrent_drummond_d_v_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_wave_variant  ,  true);}},
		{  recurrent_drummond_d_t_wave_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_variant       ,  true);}},
		{  recurrent_drummond_d_v_wave_transformation_id, [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_wave_variant  ,  true);}},
		{                   chang_wynn_transformation_id, [](){ return std::make_unique<chang_wynn_algorithm<T, K>>();                                       }},
		{                  ford_sidi_2_transformation_id, [](){ return std::make_unique<ford_sidi_2_algorithm<T, K>>();                                      }},
		{                  ford_sidi_3_transformation_id, [](){ return std::make_unique<ford_sidi_3_algorithm<T, K>>();                                      }},
		{               levin_sidi_l_u_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::u_variant            , false);}},
		{               levin_sidi_l_t_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_variant            , false);}},
		{          levin_sidi_l_t_wave_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_wave_variant       , false);}},
		{               levin_sidi_l_v_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_variant            , false);}},
		{          levin_sidi_l_v_wave_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_wave_variant       , false);}},
		{     recurrent_levin_sidi_l_u_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::u_variant            , true); }},
		{     recurrent_levin_sidi_l_t_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_variant            , true); }},
		{recurrent_levin_sidi_l_t_wave_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_wave_variant       , true); }},
		{     recurrent_levin_sidi_l_v_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_variant            , true); }},
		{recurrent_levin_sidi_l_v_wave_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_wave_variant       , true); }},
		{               levin_sidi_m_u_transformation_id, [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::u_variant     );       }},
		{               levin_sidi_m_t_transformation_id, [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::t_variant     );       }},
		{          levin_sidi_m_t_wave_transformation_id, [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::t_wave_variant);       }},
		{               levin_sidi_m_v_transformation_id, [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::v_variant     );       }},
		{          levin_sidi_m_v_wave_transformation_id, [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::v_wave_variant);       }},
		{               levin_sidi_s_u_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::u_variant     , false);}},
		{               levin_sidi_s_t_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_variant     , false);}},
		{          levin_sidi_s_t_wave_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_wave_variant, false);}},
		{               levin_sidi_s_v_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_variant     , false);}},
		{          levin_sidi_s_v_wave_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_wave_variant, false);}},
		{     recurrent_levin_sidi_s_u_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::u_variant     , true); }},
		{     recurrent_levin_sidi_s_t_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_variant     , true); }},
		{recurrent_levin_sidi_s_t_wave_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_wave_variant, true); }},
		{     recurrent_levin_sidi_s_v_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_variant     , true); }},
		{recurrent_levin_sidi_s_v_wave_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_wave_variant, true); }},
		{                     lubkin_w_transformation_id, [](){ return std::make_unique<lubkin_w_algorithm<T,K>>();                                          }},
		{                   richardson_transformation_id, [](){ return std::make_unique<richardson_algorithm<T,K>>();                                        }},
		{                       shanks_transformation_id, [](){ return std::make_unique<shanks_algorithm<T,K>>();                                            }},
		{           shanks_alternating_transformation_id, [](){ return std::make_unique<shanks_transform_alternating<T,K>>();                                }},
		{                      weniger_tramsformation_id, [](){ return std::make_unique<weniger_algorithm<T,K>>();                                           }},
		{               wynn_epsilon_1_transforamtion_id, [](){ return std::make_unique<wynn_epsilon_1_algorithm<T,K>>();                                    }},
		{               wynn_epsilon_2_transforamtion_id, [](){ return std::make_unique<wynn_epsilon_2_algorithm<T,K>>();                                    }},
		{               wynn_epsilon_3_transforamtion_id, [](){ return std::make_unique<wynn_epsilon_3_algorithm<T,K>>();                                    }},
        {                 wynn_rho_rho_transformation_id, [](){ return std::make_unique<wynn_rho_algorithm<T, K>>(numerator_type::rho_variant);              }},
  		{         wynn_rho_generalized_transformation_id, [](){ return std::make_unique<wynn_rho_algorithm<T, K>>(numerator_type::generalized_variant);      }},
    	{           wynn_rho_gamma_rho_transformation_id, [](){ return std::make_unique<wynn_rho_algorithm<T, K>>(numerator_type::gamma_rho_variant);        }},
	};

	std::vector<std::unique_ptr<series_acceleration<T,K>>> algos(algoInit.size());
	for (size_t j = 1; j <= algoInit.size(); ++j){
		algos[j-1] = algoInit[static_cast<transformation_id_t>(j)]();
	}

	exp_series<T,K> testSeries = exp_series<T,K>(x);
	SeriesResult<T> result = testSeries.generateSeries(150);

	std::cout << "EXP(x) = " << testSeries.get_sum() << "\n";

	for (size_t i = 0; i < algos.size(); ++i){

		algos[i]->print_info();

		for (size_t j = 0; j <= 10; ++j) {
			
			try{
				std::cout << "n = order = " << j << " : " << algos[i]->operator()(j,j, result, j) << "\n";
			} catch (std::overflow_error& e){
				std::cout << e.what() << "\n";
			} catch (std::domain_error& e){
				std::cout << e.what() << "\n";
			} catch (float_precision::divide_by_zero& e){
				std::cout << "Division by zero in float_precision" << "\n";
			} catch (complex_precision<float>::divide_by_zero& e){
				std::cout << e.what() << "\n";
			} catch (complex_precision<double>::divide_by_zero& e){
				std::cout << e.what() << "\n";
			} catch (complex_precision<long double>::divide_by_zero& e){
				std::cout << e.what() << "\n";
			} catch (complex_precision<float_precision>::divide_by_zero& e){
				std::cout << e.what() << "\n";
			} catch (std::out_of_range& e){
				std::cout << e.what() << "\n";
			}

		}
		std::cout << "\n";
	}

}

void TestNoise() {
	using typeA = complex_precision<float_precision>;
	using typeB = unsigned long long int;

	typeA x(float_precision(6, 50), float_precision(6, 50));
	std::cout << x << "\n";

	exp_series<typeA, typeB> testSeries = exp_series<typeA, typeB>(x);
	SeriesResult<typeA> result = testSeries.generateSeries(30);

	std::cout << "EXP(x) = " << testSeries.get_sum() << "\n";

	SeriesResult<typeA> noisyResult = jitter(result, typeA(float_precision(-1, 50), float_precision(-1, 50)), typeA(float_precision(1, 50), float_precision(1, 50)), uniform);



	std::cout << "\n";

	std::unique_ptr<series_acceleration<typeA, typeB>> algo = std::make_unique<brezinski_theta_algorithm<typeA, typeB>>();

	algo->print_info();

	for (size_t j = 0; j <= 10; ++j) {
		try{
			std::cout << "n = order = " << j << " : " << algo->operator()(j,j, result, j) << "\n";
		} catch (...){
			std::cout << "Exception caught\n";
		}
	}
	std::cout << "\n";


	for (size_t j = 0; j <= 10; ++j) {
		try{
			std::cout << "n = order = " << j << " : " << algo->operator()(j,j, result, j) << "\n";
		} catch (...){
			std::cout << "Exception caught\n";
		}
	}
}

int main()
{

	//using typeA = double;
	//using typeA = complex_precision<float_precision>;
	using typeA = float_precision;
	using typeB = unsigned long long int;

	typeA x(1, 200);
	// typeA x(float_precision(1, 50), float_precision(1, 50));
	// std::cout << x << "\n";
	//typeA x(1.0);

	//std::cout << fact<typeB>(6) << "\n";
	//std::cout << double_fact<typeB>(6) << "\n";
	//std::cout << binomial_coefficient<typeB>(12, 5) << "\n";

	//std::cout << fact<typeB>(7) << "\n";
	//std::cout << double_fact<typeB>(7) << "\n";
	//std::cout << binomial_coefficient<typeB>(13, 5) << "\n";

	testCompatability<typeA, typeB>(x);

	TestNoise();

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
