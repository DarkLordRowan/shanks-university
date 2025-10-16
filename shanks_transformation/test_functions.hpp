/**
 * @file test_functions.h
 * @brief This file contains the testing functions
 */

#pragma once

#include "libs/arbitrary_arithmetics/fprecision.h"
#include "methods.hpp"
#include "methods/series_acceleration.hpp"
#include "series/series_base.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <type_traits>
#include <unordered_map>

template <typename T>
T inputCustomType(std::string varName = "x"){
	if constexpr (std::is_same<T, float_precision>::value){
		std::string xString;
		size_t precision = 20;
		std::cout << varName <<" : "; std::cin >> xString; std::cout << "\n";
		std::cout << "precision : "; std::cin >> precision; std::cout << "\n";
		return float_precision(xString, std::max(static_cast<size_t>(20), precision));
	} else if constexpr(std::is_same<T, complex_precision<float_precision>>::value){
		std::string xRealString;
		std::string xImagString;
		size_t realPrec = 20;
		size_t imagPrec = 20;

		std::cout << "Re  "<< varName <<" : "; std::cin >> xRealString; std::cout << "\n";
		std::cout << "precision : "; std::cin >> realPrec; std::cout << "\n";

		std::cout << "Im "<< varName <<" : "; std::cin >> xImagString; std::cout << "\n";
		std::cout << "precision : "; std::cin >> imagPrec; std::cout << "\n";

		return complex_precision<float_precision>(
			float_precision(xRealString, std::max(static_cast<size_t>(20), realPrec)),
			float_precision(xImagString, std::max(static_cast<size_t>(20), imagPrec))
		);
	} else if constexpr (!std::is_same<T, complex_precision<float_precision>>::value && isComplexLike<T>::value ) {
		double realX, imagX;

		std::cout << "Re "<< varName <<" : "; std::cin >> realX; std::cout << "\n";
		std::cout << "Im "<< varName <<" : "; std::cin >> imagX; std::cout << "\n";

		return T(realX, imagX);
	} else {
		T x;
		std::cout << varName << " : "; std::cin >> x; std::cout << "\n";
		return x;
	}
}

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
	SeriesResult<T>& result,
	const series_acceleration<T,K>* test)
{

	test->print_info();
	for (K i = 1; i <= n; ++i) {
		try
		{
			std::cout << "Sum of algo : " << series->get_sum() << '\n';
			std::cout << "S_" << i << " : " << result.Sn[i] << '\n';
			std::cout << "T_" << i << " of order " << order << " : " << test->operator()(i, order, result) << '\n';
			std::cout << "T_" << i << " of order " << order << " - S_" << i
			<< " : " << test->operator()(i, order, result) - result.Sn[i] << '\n';

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
	SeriesResult<T>& result,
	const series_acceleration<T,K>* test
)
{

	test->print_info();
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
	SeriesResult<T>& result,
	const series_acceleration<T,K>* test
)
{	

	std::cout << "Tranformation of order " << order << " remainders from i = 1 to " << n << '\n';
	test->print_info();
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
	SeriesResult<T>& result,
	const series_acceleration<T,K>* test_1, 
	const series_acceleration<T,K>* test_2
)
{

	using std::abs;

	std::cout << "Tranformations of order " << order << " remainders from i = 1 to " << n << '\n';
	std::cout << "The transformation #1 is ";
	test_1->print_info();
	std::cout << "The transformation #2 is ";
	test_2->print_info();

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
	SeriesResult<T>& result,
	const series_acceleration<T,K>* test
)
{

	const auto start_time = std::chrono::system_clock::now();
	test->print_info();
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
	SeriesResult<T>& result
){

	std::unordered_map<transformation_id_t, std::function<std::unique_ptr<series_acceleration<T,K>>(void)>> algoInit = {
	    {              brezinski_theta_transformation_id, [](){ return std::make_unique<brezinski_theta_algorithm<T, K>>();                                  }},
		{                   chang_wynn_transformation_id, [](){ return std::make_unique<chang_wynn_algorithm<T, K>>();                                       }},
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
		{                      weniger_transformation_id, [](){ return std::make_unique<weniger_algorithm<T,K>>();                                           }},
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
	for (size_t i = 0; i <= n; ++i){
		for (size_t j = 0; j < algos.size(); ++j) {
				try{
					algos[j]->print_info();
					std::cout << "n = " << i << " order = " << order << " : " << algos[j]->operator()(i,order,result) << "\n";
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