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


	exp_series<T,K> testSeries = exp_series<T,K>();
	SeriesResult<T> result = testSeries.generateSeries(x, 300);

	std::cout << "EXP(x) = " << testSeries.get_sum() << "\n";


	for (size_t i = 0; i < algos.size(); ++i){

		algos[i]->print_info();

		for (size_t j = 0; j <= 30; ++j) {
			
			try{
				std::cout << "n = " << 1 << " order = " << j << " : " << algos[i]->operator()(j,j,result) << "\n";
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

template<AcceptedLike T, UnsignedIntLike K>
void testRows(){

	std::unordered_map<series_id_t, std::function<std::unique_ptr<series_base<T,K>>(void)>> rowsInit = {
		{									    abs_sin_x_minus_2_div_pi_series_id,[]() { return std::make_unique<abs_sin_x_minus_2_div_pi_series<T,K>>();}},
		{													   arcsinh_x_series_id,[]() { return std::make_unique<arcsinh_x_series<T,K>>();}},
		{													   arcsin_x2_series_id,[]() { return std::make_unique<arcsin_x2_series<T,K>>();}},
		{											    arcsin_x_minus_x_series_id,[]() { return std::make_unique<arcsin_x_minus_x_series<T,K>>();}},
		{													    arcsin_x_series_id,[]() { return std::make_unique<arcsin_x_series<T,K>>();}},
		{													  arctanh_x2_series_id,[]() { return std::make_unique<arctanh_x2_series<T,K>>();}},
		{													   arctanh_x_series_id,[]() { return std::make_unique<arctanh_x_series<T,K>>();}},
		{													    arctg_x2_series_id,[]() { return std::make_unique<arctg_x2_series<T,K>>();}},
		{													    arctg_x3_series_id,[]() { return std::make_unique<arctg_x3_series<T,K>>();}},
		{														 arctg_x_series_id,[]() { return std::make_unique<arctg_x_series<T,K>>();}},
		{															 bin_series_id,[]() { return std::make_unique<bin_series<T,K>>();}},
		{														    ci_x_series_id,[]() { return std::make_unique<ci_x_series<T,K>>();}},
		{										   cos3xmin1_div_xsquare_series_id,[]() { return std::make_unique<cos3xmin1_div_xsquare_series<T,K>>();}},
		{														    cosh_series_id,[]() { return std::make_unique<cosh_series<T,K>>();}},
		{															 cos_series_id,[]() { return std::make_unique<cos_series<T,K>>();}},
		{													  cos_sqrt_x_series_id,[]() { return std::make_unique<cos_sqrt_x_series<T,K>>();}},
		{														  cos_x2_series_id,[]() { return std::make_unique<cos_x2_series<T,K>>();}},
		{										   eighth_pi_m_one_third_series_id,[]() { return std::make_unique<eighth_pi_m_one_third_series<T,K>>();}},
		{															 erf_series_id,[]() { return std::make_unique<erf_series<T,K>>();}},
		{										    exp_m_cos_x_sinsin_x_series_id,[]() { return std::make_unique<exp_m_cos_x_sinsin_x_series<T,K>>();}},
		{															 exp_series_id,[]() { return std::make_unique<exp_series<T,K>>();}},
		{												 exp_squared_erf_series_id,[]() { return std::make_unique<exp_squared_erf_series<T,K>>();}},
		{															 e_x_series_id,[]() { return std::make_unique<e_x_series<T,K>>();}},
		{												  five_pi_twelve_series_id,[]() { return std::make_unique<five_pi_twelve_series<T,K>>();}},
		{												     four_arctan_series_id,[]() { return std::make_unique<four_arctan_series<T,K>>();}},
		{												    four_ln2_m_3_series_id,[]() { return std::make_unique<four_ln2_m_3_series<T,K>>();}},
		{											     half_asin_two_x_series_id,[]() { return std::make_unique<half_asin_two_x_series<T,K>>();}},
		{									  half_minus_sinx_multi_pi_4_series_id,[]() { return std::make_unique<half_minus_sinx_multi_pi_4_series<T,K>>();}},
		{							  half_multi_ln_1div2multi1minuscosx_series_id,[]() { return std::make_unique<half_multi_ln_1div2multi1minuscosx_series<T,K>>();}},
		{										   incomplete_Gamma_func_series_id,[]() { return std::make_unique<incomplete_Gamma_func_series<T,K>>();}},
		{												     inverse_1mx_series_id,[]() { return std::make_unique<inverse_1mx_series<T,K>>();}},
		{											   inverse_sqrt_1m4x_series_id,[]() { return std::make_unique<inverse_sqrt_1m4x_series<T,K>>();}},
		//{														    Ja_x_series_id,[]() { return std::make_unique<Ja_x_series<T,K>>();}},
		{															 k_x_series_id,[]() { return std::make_unique<k_x_series<T,K>>();}},
		{												  lambert_W_func_series_id,[]() { return std::make_unique<lambert_W_func_series<T,K>>();}},
		{											  ln13_min_ln7_div_7_series_id,[]() { return std::make_unique<ln13_min_ln7_div_7_series<T,K>>();}},
		{													    ln1_m_x2_series_id,[]() { return std::make_unique<ln1_m_x2_series<T,K>>();}},
		{														   ln1mx_series_id,[]() { return std::make_unique<ln1mx_series<T,K>>();}},
		{							 ln_1plussqrt1plusxsquare_minus_ln_2_series_id,[]() { return std::make_unique<ln_1plussqrt1plusxsquare_minus_ln_2_series<T,K>>();}},
		{												    ln_1_plus_x3_series_id,[]() { return std::make_unique<ln_1_plus_x3_series<T,K>>();}},
		{										   ln_1plusx_div_1plusx2_series_id,[]() { return std::make_unique<ln_1plusx_div_1plusx2_series<T,K>>();}},
		{														  ln1px4_series_id,[]() { return std::make_unique<ln1px4_series<T,K>>();}},
		{															 ln2_series_id,[]() { return std::make_unique<ln2_series<T,K>>();}},
		{														 ln_cosx_series_id,[]() { return std::make_unique<ln_cosx_series<T,K>>();}},
		{											  ln_sinx_minus_ln_x_series_id,[]() { return std::make_unique<ln_sinx_minus_ln_x_series<T,K>>();}},
		{							    ln_x_plus_one_x_minus_one_halfed_series_id,[]() { return std::make_unique<ln_x_plus_one_x_minus_one_halfed_series<T,K>>();}},
		{												   mean_sinh_sin_series_id,[]() { return std::make_unique<mean_sinh_sin_series<T,K>>();}},
		{										  m_fact_1mx_mp1_inverse_series_id,[]() { return std::make_unique<m_fact_1mx_mp1_inverse_series<T,K>>();}},
		{							    minus_3_div_4_or_x_minus_3_div_4_series_id,[]() { return std::make_unique<minus_3_div_4_or_x_minus_3_div_4_series<T,K>>();}},
		{											  minus_one_ned_in_n_series_id,[]() { return std::make_unique<minus_one_ned_in_n_series<T,K>>();}},
		{										 minus_one_n_fact_n_in_n_series_id,[]() { return std::make_unique<minus_one_n_fact_n_in_n_series<T,K>>();}},
		{											   minus_one_quarter_series_id,[]() { return std::make_unique<minus_one_quarter_series<T,K>>();}},
		{			 minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id,[]() { return std::make_unique<minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T,K>>();}},
		{							    minus_x_minus_pi_4_or_minus_pi_4_series_id,[]() { return std::make_unique<minus_x_minus_pi_4_or_minus_pi_4_series<T,K>>();}},
		{									 one_div_sqrt2_sin_xdivsqrt2_series_id,[]() { return std::make_unique<one_div_sqrt2_sin_xdivsqrt2_series<T,K>>();}},
		{						  one_div_two_minus_x_multi_three_plus_x_series_id,[]() { return std::make_unique<one_div_two_minus_x_multi_three_plus_x_series<T,K>>();}},
		{								  one_minus_sqrt_1minus4x_div_2x_series_id,[]() { return std::make_unique<one_minus_sqrt_1minus4x_div_2x_series<T,K>>();}},
		{															 one_series_id,[]() { return std::make_unique<one_series<T,K>>();}},
		{									 one_third_pi_squared_m_nine_series_id,[]() { return std::make_unique<one_third_pi_squared_m_nine_series<T,K>>();}},
		{										     one_twelfth_3x2_pi2_series_id,[]() { return std::make_unique<one_twelfth_3x2_pi2_series<T,K>>();}},
		{													 one_twelfth_series_id,[]() { return std::make_unique<one_twelfth_series<T,K>>();}},
		{											  one_twelfth_x2_pi2_series_id,[]() { return std::make_unique<one_twelfth_x2_pi2_series<T,K>>();}},
		{														    pi_3_series_id,[]() { return std::make_unique<pi_3_series<T,K>>();}},
		{														    pi_4_series_id,[]() { return std::make_unique<pi_4_series<T,K>>();}},
		{							 pi_8_cosx_square_minus_1_div_3_cosx_series_id,[]() { return std::make_unique<pi_8_cosx_square_minus_1_div_3_cosx_series<T,K>>();}},
		{												 	 pi_cubed_32_series_id,[]() { return std::make_unique<pi_cubed_32_series<T,K>>();}},
		{									    pi_four_minus_ln2_halfed_series_id,[]() { return std::make_unique<pi_four_minus_ln2_halfed_series<T,K>>();}},
		{					   pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id,[]() { return std::make_unique<pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T,K>>();}},
		{												    pi_minus_x_2_series_id,[]() { return std::make_unique<pi_minus_x_2_series<T,K>>();}},
		{						    						 		  pi_series_id,[]() { return std::make_unique<pi_series<T,K>>();}},
		{						    				     pi_six_min_half_series_id,[]() { return std::make_unique<pi_six_min_half_series<T,K>>();}},
		{						    			  pi_squared_6_minus_one_series_id,[]() { return std::make_unique<pi_squared_6_minus_one_series<T,K>>();}},
		{						    				   pi_squared_twelve_series_id,[]() { return std::make_unique<pi_squared_twelve_series<T,K>>();}},
		{ pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series_id,[]() { return std::make_unique<pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<T,K>>();}},
		{pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series_id,[]() { return std::make_unique<pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series<T,K>>();}},
		{ 			  								   riemann_zeta_func_series_id,[]() { return std::make_unique<riemann_zeta_func_series<T,K>>();}},
		{ 			  	 riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id,[]() { return std::make_unique<riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T,K>>();}},
		{ 			  	      					  series_with_ln_number1_series_id,[]() { return std::make_unique<series_with_ln_number1_series<T,K>>();}},
		{ 			  	      					  series_with_ln_number2_series_id,[]() { return std::make_unique<series_with_ln_number2_series<T,K>>();}},
		{ 			  	      									    sinh_series_id,[]() { return std::make_unique<sinh_series<T,K>>();}},
		{ 			  	      									 sinh_x2_series_id,[]() { return std::make_unique<sinh_x2_series<T,K>>();}},
		{ 			  	      									     sin_series_id,[]() { return std::make_unique<sin_series<T,K>>();}},
		{ 			  	      									  sin_x2_series_id,[]() { return std::make_unique<sin_x2_series<T,K>>();}},
		{ 			  	      									    si_x_series_id,[]() { return std::make_unique<si_x_series<T,K>>();}},
		{ 			  	    			   sqrt_1plusx_min_1_min_x_div_2_series_id,[]() { return std::make_unique<sqrt_1plusx_min_1_min_x_div_2_series<T,K>>();}},
		{ 			  	    							 	 sqrt_1plusx_series_id,[]() { return std::make_unique<sqrt_1plusx_series<T,K>>();}},
		{ 			  	    		    sqrt_oneminussqrtoneminusx_div_x_series_id,[]() { return std::make_unique<sqrt_oneminussqrtoneminusx_div_x_series<T,K>>();}},
		{ 			  	    							     ten_minus_x_series_id,[]() { return std::make_unique<ten_minus_x_series<T,K>>();}},
		{ 			  	    							  three_minus_pi_series_id,[]() { return std::make_unique<three_minus_pi_series<T,K>>();}},
		{ 			  	    				  two_arcsin_square_x_halfed_series_id,[]() { return std::make_unique<two_arcsin_square_x_halfed_series<T,K>>();}},
		{ 			  	    							    two_degree_x_series_id,[]() { return std::make_unique<two_degree_x_series<T,K>>();}},
		{ 			  	    									 two_ln2_series_id,[]() { return std::make_unique<two_ln2_series<T,K>>();}},
		{ 			  	    							   x_1mx_squared_series_id,[]() { return std::make_unique<x_1mx_squared_series<T,K>>();}},
		{ 			  	    							    x_div_1minx2_series_id,[]() { return std::make_unique<x_div_1minx2_series<T,K>>();}},
		{ 			  	    								 x_div_1minx_series_id,[]() { return std::make_unique<x_div_1minx_series<T,K>>();}},
		//{ 			  	    								  xmb_Jb_two_series_id,[]() { return std::make_unique<xmb_Jb_two_series<T,K>>();}},
		{ 			  	    							    x_min_sqrt_x_series_id,[]() { return std::make_unique<x_min_sqrt_x_series<T,K>>();}},
		{ 			  	    								 		   x_series_id,[]() { return std::make_unique<x_series<T,K>>();}},
		{ 			  	     xsquareplus3_div_xsquareplus2multix_minus_1_series_id,[]() { return std::make_unique<xsquareplus3_div_xsquareplus2multix_minus_1_series<T,K>>();}},
		{ 			  	    					 				   x_two_series_id,[]() { return std::make_unique<x_two_series<T,K>>();}},
		{ 			  	    					  x_two_throught_squares_series_id,[]() { return std::make_unique<x_two_throught_squares_series<T,K>>();}},
	};

	T x = static_cast<T>(0.3);
	size_t length = 60;

	std::vector<std::unique_ptr<series_base<T,K>>> seriesVec;
	std::vector<SeriesResult<T>> seriesResults;

	for (auto& it : rowsInit){
		std::cout << it.second()->get_name() << "\n";
		try{
			it.second()->generateSeries(x, length);
			seriesVec.push_back(it.second());
			seriesResults.push_back(seriesVec.back()->generateSeries(x, length));
		} catch (std::domain_error& e){
			std::cout << e.what() << "\n";
		}
	}

	for (size_t j = 0 ; j < seriesResults.size(); ++j){
		std::cout << j + 1 << " : " << seriesVec[j]->get_name() << "\n";
		for (size_t k = 57; k < length; ++k){
			std::cout << seriesResults[j].Sn[k] << " ";
		}
		std::cout << " = " << seriesVec[j]->get_sum() << "\n";
	}
	
}

template<AcceptedLike T, UnsignedIntLike K>
constexpr void testAlgorithm(std::unique_ptr<series_acceleration<T,K>>& algo, const SeriesResult<T>& series){

	algo->print_info();
	for (size_t j = 0; j <= 10; ++j) {

		try{
			std::cout << "n = order = " << j << " : " << algo->operator()(j,j, series) << "\n";
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

template<AcceptedLike T, UnsignedIntLike K>
constexpr void testNoise(const T& x){

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
		{recurrent_levin_sidi_l_t_wave_transformation_id, [](){ return std::make_unique<levin_algorithm<T, K>>(remainder_type::t_wave_variant       , true, T(0.9)); }},
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

	exp_series<T,K> testSeries = exp_series<T,K>();
	SeriesResult<T> result = testSeries.generateSeries(x, 150);

	NoiseGenerator<T> noiseGen = NoiseGenerator<T>(NoiseType::normal, 42);
	SeriesResult<T> noisyResult = noiseGen.jitter(result, 0.0, 1e-5);


	// std::cout << "EXP(x) = " << testSeries.get_sum() << "\n";

	std::cout << "BASIC NOISE\n";
	for (size_t i = 0; i < algos.size(); ++i){
		testAlgorithm(algos[i], noisyResult);
	}


	std::cout << "\n--------------------------------------------------------------------------------------------\n";
	std::cout << "STRONGER NOISE\n";

	SeriesResult<T> strongerNoisyResult = noiseGen.jitter(result, 0.0, 1e-3);

	for (size_t i = 0; i < algos.size(); ++i){
		testAlgorithm(algos[i], strongerNoisyResult);
	}

	std::cout << "\n--------------------------------------------------------------------------------------------\n";
	std::cout << "EXTREME NOISE\n";

	SeriesResult<T> extremeNoisyResult = noiseGen.jitter(result, 0.0, 1e-1);

	for (size_t i = 0; i < algos.size(); ++i){
		testAlgorithm(algos[i], extremeNoisyResult);
	}

	std::cout << "\n--------------------------------------------------------------------------------------------\n";
	std::cout << "SHIFTED DOWN NOISE\n";

	SeriesResult<T> shiftedDownNoisyResult = noiseGen.jitter(result, -1.0, 1e-5);
	for (size_t i = 0; i < algos.size(); ++i){
		testAlgorithm(algos[i], shiftedDownNoisyResult);
	}

	std::cout << "\n--------------------------------------------------------------------------------------------\n";
	std::cout << "SHIFTED UP NOISE\n";

	SeriesResult<T> shiftedUpNoisyResult = noiseGen.jitter(result, 1.0, 1e-5);
	for (size_t i = 0; i < algos.size(); ++i){
		testAlgorithm(algos[i], shiftedUpNoisyResult);
	}

}
/* Вроде ошибок не возникает (Даже при шуме около +-1)
 * РезулЬтаты алгоритмов выглядят похоже на обычные
 * Надо понять какие значения брать для генерации шума для каждой серии (наверно?)
 */

int main()
{


	//using typeA = double;
	using typeA = complex_precision<float_precision>;
	//using typeA = float_precision;
	//using typeA = complex_precision<double>;
	using typeB = unsigned long long int;
	//typeA x(4, 200);
	//typeA x(1,1);
	typeA x(float_precision(1, 50), float_precision(1, 50));
	// std::cout << x << "\n";
	//typeA x(1.0);

	//std::cout << fact<typeB>(6) << "\n";
	//std::cout << double_fact<typeB>(6) << "\n";
	//std::cout << binomial_coefficient<typeB>(12, 0) << "\n";

	//std::cout << fact<typeB>(7) << "\n";
	//std::cout << double_fact<typeB>(7) << "\n";
	//std::cout << binomial_coefficient<typeB>(0, 3) << "\n";

	//testNoise<typeA, typeB>(x);

	// TestNoise();
	//testRows<typeA,typeB>();
	testCompatability<typeA, typeB>(x);

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
