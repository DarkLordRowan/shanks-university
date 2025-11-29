/**
 * @file test_functions.h
 * @brief This file contains the testing functions
 */

#pragma once

#include "../methods.hpp"
#include "../series_base.hpp"
#include "../noise/noise_generator.hpp"
#include "../console/console_IO.hpp"

#include <chrono>
#include <iostream>

 /**
 * @brief Function that prints out comparesment between transformed and nontransformed partial sums
 * At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
 * Then it prints out partial sums of first i terms of the series where i ranges from 1 to n (!)
 * After that it prints out transformed partial sum of first i terms of the series of order order
 * At last it prints out the difference between the two
 * @authors Bolshakov M.P.
 * @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
 * @param n The number of terms
 * @param order The order of the transformation
 * @param series The series class object to be accelerated
 * @param test The type of transformation that is being used
 */
template <AcceptedLike T, std::unsigned_integral K>
void cmp_sum_and_transform(const K n, const K order, 
	series_base<T,K>* series,
	series_result<T>& result,
	series_acceleration<T,K>* test)
{

	std::cout << "transformation : " << test->get_name() << "\n";
	for (K i = 1; i <= n; ++i) {
		try
		{
			const T res = test->operator()(i, order, result);
			std::cout << "Sum of algo : " << series->get_sum() << '\n';
			std::cout << "S_" << i << " : " << result.Sn[i] << "\n";
			std::cout << "T_" << i << " of order " << order << " : " << res << std::endl;
			std::cout << "T_" << i << " of order " << order << " - S_" << i << " : " << res - result.Sn[i] << '\n';

		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
		}
	}
}

/**
* @brief Function that prints out comparesment between the terms of transformed and nontransformed series
* At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out terms from the first to nth of the series
* At last it prints out terms from the first to nth of the transformed series
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test The type of transformation that is being used
*/
template <AcceptedLike T, std::unsigned_integral K>
void cmp_a_n_and_transform(const K n, const K order,
	series_base<T,K>* series, 
	series_result<T>& result,
	series_acceleration<T,K>* test
)
{

	std::cout << "transformation : " << test->get_name() << "\n";
	for (K i = 1; i <= n; ++i) {
		try
		{
			std::cout << "a_" << i << " : " << result.an[i] << '\n';
			std::cout << "t_" << i << " : " << test->operator()(i, order, result) - test->operator()(i - 1, order, result) << '\n';
			std::cout << "t_" << i << " of order " << order << " - a_" << i
				<< " : " << (test->operator()(i, order, result) - test->operator()(i - 1, order, result)) - result.an[i] << '\n';
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
		}
	}
}

/**
 * @brief Applies noise to a given series
 *
 * @tparam T The type of the elements in the series
 * @tparam ParamType The type of the noise parameters
 *
 * @param source The original series to which noise will be applied
 * Valid values: any series_result<T>.
 * @param noise_type The type of noise to apply
 * Valid values: NoiseType::uniform, NoiseType::normal, NoiseType::poisson.
 * @param param1 The first parameter for the noise generation.\n
 * Valid values: depends on noise type:
 * -For uniform noise: Any real number representing the lower bound or complex number with real and imaginary parts as lower bounds.
 * -For normal noise: Any real number representing the mean or complex number with real and imaginary parts as means.
 * -For poisson noise: Any positive real number representing the lambda parameter.
 * @param param2 The second parameter for the noise generation (optional, depending on noise type)\n
 * Valid values: depends on noise type:
 * -For uniform noise: Any real number greater than param1 representing the upper bound or complex number with real and imaginary parts greater than those of param1.
 * -For normal noise: Any positive real number representing the standard deviation or complex number with positive real and imaginary parts.
 * -For poisson noise: Not used, can be default constructed.
 * @param seed The seed for the random number generator (default is based on current time and random value)\n
 * Valid values: any positive integer.
 *
 * @return A new series_result<T> with noise applied
 */
template<AcceptedLike T, AcceptedLike ParamType>
inline series_result<T> jitter(
	series_result<T>& source,
	const NoiseType noise_type
	)
{
	ParamType param1;
	ParamType param2;
	unsigned long long seed;
	std::cout << "Enter seed (0 for random, any other positive integer will be used as seed)\n";
	seed = console_IO<unsigned long long int>::input("seed");

	if (seed == 0){
		seed = std::chrono::system_clock::now().time_since_epoch().count() + std::rand();
	}

	std::cout << "Seed : " << std::to_string(seed) << "\n";

	switch (noise_type) {
		case uniform:
			param1 = console_IO<ParamType>::input("Lower bound");
			param2 = console_IO<ParamType>::input("Upper bound");
			break;
		case normal:
			param1 = console_IO<ParamType>::input("Mean");
			param2 = console_IO<ParamType>::input("Standard deviation");
			break;
		case poisson:
			param1 = console_IO<ParamType>::input("Lambda");
			param2 = ParamType(); // Not used for poisson
			break;
		default:
			throw std::invalid_argument("Invalid noise type");
	}

	noise_generator<T> gen = noise_generator<T>(noise_type,seed);
	return gen.jitter(source, param1, param2);
}

/**
* @brief Function that prints out the remainders
* At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out remainders of the series from 1 to n
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms for the last remainder
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test The type of transformation that is being used
*/
template <AcceptedLike T, std::unsigned_integral K>
void transformation_remainders(const K n, const K order, 
	series_base<T,K>* series,
	series_result<T>& result,
	series_acceleration<T,K>* test
)
{	

	std::cout << "Tranformation of order " << order << " remainders from i = 1 to " << n << '\n';
	std::cout << "transformation : " << test->get_name() << "\n";
	for (K i = 1; i <= n; ++i) {
		try
		{
			std::cout << "S - T_" << i << " : " << series->get_sum() - test->operator()(i, order, result) << '\n';
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
		}
	}
}

/**
* @brief Function that showcases the difference between 2 transformations
* At first it prints out the type of transformations, series that are being transformed, type of enumerating integer and type of series terms
* Then it prints out remainders of the series sum from 1 to n transformed 2 different ways and also tells which one got closer to the sum of the series
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type_1 is the first type of transformation we are using, transform_type_2 is the second type of transformation we are using
* @param n The number of terms for the last remainder
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test_1 The type of the first transformation that is being used
* @param test_2 The type of the second transformation that is being used
*/
template <AcceptedLike T, std::unsigned_integral K>
void cmp_transformations(const K n, const K order, 
	series_base<T,K>* series,
	series_result<T>& result,
	series_acceleration<T,K>* test_1, 
	series_acceleration<T,K>* test_2
)
{

	using std::abs;

	std::cout << "Tranformations of order " << order << " remainders from i = 1 to " << n << '\n';
	std::cout << "The transformation #1 is ";
	std::cout << test_1->get_name() << "\n";
	std::cout << "The transformation #2 is ";
	std::cout << test_2->get_name() << "\n";

	T diff_1;
	T diff_2;
	for (K i = 1; i <= n; ++i) {
		try
		{
			diff_1 = series->get_sum() - test_1->operator()(i, order, result);
			diff_2 = series->get_sum() - test_2->operator()(i, order, result);
			std::cout << "The transformation #1: S - T_" << i << " : " << diff_1 << '\n';
			std::cout << "The transformation #2: S - T_" << i << " : " << diff_2 << '\n';
			if (abs(diff_1) < abs(diff_2))
				std::cout << "The transformation #1 is faster" << '\n';
			else
				std::cout << "The transformation #2 is faster" << '\n';
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
		}
	}
}

/**
* @brief Function that evaluates the time it takes to transform series
* @authors Bolshakov M.P.
* @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of transformation we are using
* @param n The number of terms for the last remainder
* @param order The order of the transformation
* @param series The series class object to be accelerated
* @param test The type of the first transformation that is being used
*/
template <AcceptedLike T, std::unsigned_integral K>
void eval_transform_time(const K n, const K order, 
	series_base<T,K>* series,
	series_result<T>& result,
	series_acceleration<T,K>* test
)
{

	const auto start_time = std::chrono::system_clock::now();
	std::cout << "transformation : " << test->get_name() << "\n";
	for (K i = 1; i <= n; ++i) {
		try
		{
			test->operator()(i, order, result);
		}
		catch (std::domain_error& e)
		{
			std::cout << e.what() << '\n';
		}
		catch (std::overflow_error& e)
		{
			std::cout << e.what() << '\n';
		}
	}
	const auto end_time = std::chrono::system_clock::now();
	const std::chrono::duration<double, std::milli> diff = end_time - start_time;
	std::cout << "It took " << diff.count() << " to perform these transformations" << '\n';
}

template <AcceptedLike T, std::unsigned_integral K>
void test_all_transforms(
	const K n, 
	const K order, 
	series_base<T,K>* series,
	series_result<T>& result
){

	std::unordered_map<transformation_id_t, std::function<std::unique_ptr<series_acceleration<T,K>>(void)>> algoInit = {
	    {transformation_id_t::brezinski_theta_transformation_id				 , [](){ return std::make_unique<brezinski_theta_algorithm<T, K>>();                                  }},
		{transformation_id_t::chang_wynn_transformation_id					 , [](){ return std::make_unique<chang_wynn_algorithm<T, K>>();                                       }},
		{transformation_id_t::drummond_d_u_transformation_id				 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::u_type       , false);}}, 
        {transformation_id_t::drummond_d_t_transformation_id				 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_type       , false);}},
        {transformation_id_t::drummond_d_t_wave_transformation_id			 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_wave_type  , false);}},
        {transformation_id_t::drummond_d_v_transformation_id				 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_type       , false);}},
        {transformation_id_t::drummond_d_v_wave_transformation_id			 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_wave_type  , false);}}, 
		{transformation_id_t::recurrent_drummond_d_u_transformation_id		 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::u_type       ,  true);}},
    	{transformation_id_t::recurrent_drummond_d_t_transformation_id		 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_type       ,  true);}},
     	{transformation_id_t::recurrent_drummond_d_v_transformation_id		 , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::t_wave_type  ,  true);}},
		{transformation_id_t::recurrent_drummond_d_t_wave_transformation_id  , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_type       ,  true);}},
		{transformation_id_t::recurrent_drummond_d_v_wave_transformation_id  , [](){ return std::make_unique<drummond_d_algorithm<T, K>>(remainder_type::v_wave_type  ,  true);}},
		{transformation_id_t::ford_sidi_2_transformation_id					 , [](){ return std::make_unique<ford_sidi_2_algorithm<T, K>>();                                      }},
		{transformation_id_t::ford_sidi_3_transformation_id					 , [](){ return std::make_unique<ford_sidi_3_algorithm<T, K>>();                                      }},
		{transformation_id_t::levin_sidi_l_u_transformation_id				 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::u_type            , false);}},
		{transformation_id_t::levin_sidi_l_t_transformation_id				 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_type            , false);}},
		{transformation_id_t::levin_sidi_l_t_wave_transformation_id			 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_wave_type       , false);}},
		{transformation_id_t::levin_sidi_l_v_transformation_id				 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_type            , false);}},
		{transformation_id_t::levin_sidi_l_v_wave_transformation_id			 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_wave_type       , false);}},
		{transformation_id_t::recurrent_levin_sidi_l_u_transformation_id	 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::u_type            , true); }},
		{transformation_id_t::recurrent_levin_sidi_l_t_transformation_id	 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_type            , true); }},
		{transformation_id_t::recurrent_levin_sidi_l_t_wave_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_wave_type       , true); }},
		{transformation_id_t::recurrent_levin_sidi_l_v_transformation_id	 , [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_type            , true); }},
		{transformation_id_t::recurrent_levin_sidi_l_v_wave_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::v_wave_type       , true); }},
		{transformation_id_t::levin_sidi_m_u_transformation_id				 , [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::u_type     );       }},
		{transformation_id_t::levin_sidi_m_t_transformation_id				 , [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::t_type     );       }},
		{transformation_id_t::levin_sidi_m_t_wave_transformation_id			 , [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::t_wave_type);       }},
		{transformation_id_t::levin_sidi_m_v_transformation_id				 , [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::v_type     );       }},
		{transformation_id_t::levin_sidi_m_v_wave_transformation_id			 , [](){ return std::make_unique<levin_sidi_m_algorithm<T, K>>(remainder_type::v_wave_type);       }},
		{transformation_id_t::levin_sidi_s_u_transformation_id				 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::u_type     , false);}},
		{transformation_id_t::levin_sidi_s_t_transformation_id				 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_type     , false);}},
		{transformation_id_t::levin_sidi_s_t_wave_transformation_id			 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_wave_type, false);}},
		{transformation_id_t::levin_sidi_s_v_transformation_id				 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_type     , false);}},
		{transformation_id_t::levin_sidi_s_v_wave_transformation_id			 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_wave_type, false);}},
		{transformation_id_t::recurrent_levin_sidi_s_u_transformation_id	 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::u_type     , true); }},
		{transformation_id_t::recurrent_levin_sidi_s_t_transformation_id	 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_type     , true); }},
		{transformation_id_t::recurrent_levin_sidi_s_t_wave_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::t_wave_type, true); }},
		{transformation_id_t::recurrent_levin_sidi_s_v_transformation_id	 , [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_type     , true); }},
		{transformation_id_t::recurrent_levin_sidi_s_v_wave_transformation_id, [](){ return std::make_unique<levin_sidi_s_algorithm<T, K>>(remainder_type::v_wave_type, true); }},
		{transformation_id_t::lubkin_w_transformation_id					 , [](){ return std::make_unique<lubkin_w_algorithm<T,K>>();                                          }},
		{transformation_id_t::richardson_transformation_id					 , [](){ return std::make_unique<richardson_algorithm<T,K>>();                                        }},
		{transformation_id_t::shanks_transformation_id						 , [](){ return std::make_unique<shanks_algorithm<T,K>>();                                            }},
		{transformation_id_t::shanks_alternating_transformation_id			 , [](){ return std::make_unique<shanks_transform_alternating<T,K>>();                                }},
		{transformation_id_t::weniger_transformation_id						 , [](){ return std::make_unique<weniger_algorithm<T,K>>();                                           }},
		{transformation_id_t::wynn_epsilon_1_transformation_id				 , [](){ return std::make_unique<wynn_epsilon_1_algorithm<T,K>>();                                    }},
		{transformation_id_t::wynn_epsilon_2_transformation_id				 , [](){ return std::make_unique<wynn_epsilon_2_algorithm<T,K>>();                                    }},
		{transformation_id_t::wynn_epsilon_3_transformation_id				 , [](){ return std::make_unique<wynn_epsilon_3_algorithm<T,K>>();                                    }},
        {transformation_id_t::wynn_rho_rho_transformation_id				 , [](){ return std::make_unique<wynn_rho_algorithm<T, K>>(numerator_type::rho_type);              }},
  		{transformation_id_t::wynn_rho_generalized_transformation_id		 , [](){ return std::make_unique<wynn_rho_algorithm<T, K>>(numerator_type::generalized_type);      }},
    	{transformation_id_t::wynn_rho_gamma_rho_transformation_id			 , [](){ return std::make_unique<wynn_rho_algorithm<T, K>>(numerator_type::gamma_rho_type);        }},
	};

	std::vector<std::unique_ptr<series_acceleration<T,K>>> algos(algoInit.size());
	for (size_t j = 1; j <= algoInit.size(); ++j){
		algos[j-1] = algoInit[static_cast<transformation_id_t>(j)]();
	}
	
	for (size_t i = 0; i <= n; ++i){
		for (size_t j = 0; j < algos.size(); ++j) {
				try{
					std::cout << "transformation : " << algos[j]->get_name() << "\n";
					std::cout << "n = " << i << " order = " << order << " : " << algos[j]->operator()(i,order,result) << "\n\n";
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
				} catch (std::invalid_argument& e){
					std::cout << e.what() << "\n";
				}
			}
			std::cout << "\n";
	}
}