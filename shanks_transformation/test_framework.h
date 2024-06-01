/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 */

#pragma once
#include <memory>
#include <string> 
#include <set>

#include "wynn_numerators.h"
#include "remainders.h"
#include "shanks_transformation.h"
#include "epsilon_algorithm.h"
#include "levin_algorithm.h"
#include "levin_sidi_S_algorithm.h"
#include "drummond_D_algorithm.h"
#include "epsilon_algorithm_two.h"
#include "chang_whynn_algorithm.h"
#include "test_functions.h"
#include "levin_sidi_M_algorithm.h"
#include "weniger_algorithm.h"
#include "rho_wynn_algorithm.h"
#include "brezinski_theta_algorithm.h"
#include "epsilon_algorithm_three.h"
#include "levin_recursion_algorithm.h"
#include "lubkin_W_algorithm.h"
#include "richardson_algorithm.h"
#include "FSA.h"

 /**
  * @brief Enum of transformation IDs
  * @authors Bolshakov M.P.
  * @edited by Kreynin R.G.
  */
enum transformation_id_t {
	null_transformation_id,
	shanks_transformation_id,
	epsilon_algorithm_id,
	levin_algorithm_id,
	epsilon_algorithm_2_id,
	S_algorithm,
	D_algorithm,
	chang_epsilon_algorithm,
	M_algorithm,
	weniger_transformation,
	rho_wynn_transformation_id,
	brezinski_theta_transformation_id,
	epsilon_algorithm_3_id,
	levin_recursion_id,
	W_algorithm_id,
	richardson_algorithm_id,
	Ford_Sidi_algorithm_id
};
/**
 * @brief Enum of series IDs
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G.
 */
enum series_id_t {
	null_series_id, 
	exp_series_id, 
	cos_series_id, 
	sin_series_id, 
	cosh_series_id,
	sinh_series_id, 
	bin_series_id, 
	four_arctan_series_id, 
	ln1mx_series_id, 
	mean_sinh_sin_series_id,
	exp_squared_erf_series_id, 
	xmb_Jb_two_series_id, 
	half_asin_two_x_series_id,
	inverse_1mx_series_id,
	x_1mx_squared_series_id,
	erf_series_id,
	m_fact_1mx_mp1_inverse_series_id,
	inverse_sqrt_1m4x_series_id,
	one_twelfth_3x2_pi2_series_id,
	x_twelfth_x2_pi2_series_id,
	ln2_series_id,
	one_series_id,
	minus_one_quarter_series_id,
	pi_3_series_id,
	pi_4_series_id,
	pi_squared_6_minus_one_series_id,
	three_minus_pi_series_id,
	one_twelfth_series_id,
	eighth_pi_m_one_third_series_id,
	one_third_pi_squared_m_nine_series_id,
	four_ln2_m_3_series_id,
	exp_m_cos_x_sinsin_x_series_id,
	pi_four_minus_ln2_halfed_series_id,
	five_pi_twelve_series_id,
	x_two_series_id,
	pi_six_min_half_series_id,
	x_two_throught_squares_id,
	minus_one_ned_in_n_series_id,
	minus_one_n_fact_n_in_n_series_id,
	ln_x_plus_one_x_minus_one_halfed_series_id,
	two_arcsin_square_x_halfed_series_id
};

/**
 * @brief Enum of testing functions IDs
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G.
 */
enum test_function_id_t {
	null_test_function_id, 
	cmp_sum_and_transform_id, 
	cmp_a_n_and_transform_id, 
	transformation_remainder_id, 
	cmp_transformations_id,
	eval_transform_time_id,
	test_all_transforms_id
};

/**
* @brief prints out all available series for testing
* @authors Bolshakov M.P.
* @edited by Kreynin R.G.
*/
inline static void print_series_info()
{
	std::cout << "Which series' convergence would you like to accelerate?" << std::endl <<
		"List of currently avaiable series:" << std::endl <<
		"1 - exp_series" << std::endl <<
		"2 - cos_series" << std::endl <<
		"3 - sin_series" << std::endl <<
		"4 - cosh_series" << std::endl <<
		"5 - sinh_series" << std::endl <<
		"6 - bin_series" << std::endl <<
		"7 - four_arctan_series" << std::endl <<
		"8 - ln1mx_series" << std::endl <<
		"9 - mean_sinh_sin_series" << std::endl <<
		"10 - exp_squared_erf_series" << std::endl <<
		"11 - xmb_Jb_two_series" << std::endl <<
		"12 - half_asin_two_x_series" << std::endl <<
		"13 - inverse_1mx_series" << std::endl <<
		"14 - x_1mx_squared_series" << std::endl <<
		"15 - erf_series" << std::endl <<
		"16 - m_fact_1mx_mp1_inverse_series" << std::endl <<
		"17 - inverse_sqrt_1m4x_series" << std::endl <<
		"18 - one_twelfth_3x2_pi2_series" << std::endl <<
		"19 - x_twelfth_x2_pi2_series" << std::endl <<
		"20 - ln2_series_id" << std::endl <<
		"21 - one_series_id" << std::endl <<
		"22 - minus_one_quarter_series_id" << std::endl <<
		"23 - pi_3_series" << std::endl <<
		"24 - pi_4_series" << std::endl <<
		"25 - pi_squared_6_minus_one_series" << std::endl <<
		"26 - three_minus_pi_series" << std::endl <<
		"27 - one_twelfth_series" << std::endl <<
		"28 - eighth_pi_m_one_third_series" << std::endl <<
		"29 - one_third_pi_squared_m_nine_series" << std::endl <<
		"30 - four_ln2_m_3_series" << std::endl <<
		"31 - exp_m_cos_x_sinsin_x_series" << std::endl <<
		"32 - pi_four_minus_ln2_halfed_series" << std::endl <<
		"33 - five_pi_twelve_series" << std::endl <<
		"34 - x_two_series" << std::endl <<
		"35 - pi_six_min_half_series" << std::endl <<
		"36 - x_two_throught_squares" << std::endl <<
		"37 - minus_one_ned_in_n_series" << std::endl <<
		"38 - minus_one_n_fact_n_in_n_series" << std::endl <<
		"39 - ln_x_plus_one_x_minus_one_halfed_series" << std::endl <<
		"40 - two_arcsin_square_x_halfed_series" << std::endl <<
		std::endl;
}

/**
* @brief prints out all available transformations for testing
* @authors Bolshakov M.P.
* @edited by Kreynin R.G.
*/
inline static void print_transformation_info()
{
	std::cout << "Which transformation would you like to test?" << std::endl <<
		"List of currently avaiable series:" << std::endl <<
		"1 - Shanks Transformation" << std::endl <<
		"2 - Epsilon Algorithm" << std::endl <<
		"3 - Levin Algorithm" << std::endl <<
		"4 - Epsilon Algorithm V-2" << std::endl <<
		"5 - S-transformation" << std::endl <<
		"6 - D-transformation" << std::endl <<
		"7 - Chang - Wynn - Epsilon Algorithm" << std::endl <<
		"8 - M-transformation" << std::endl <<
		"9 - Weniger transformation" << std::endl <<
		"10 - Rho - Wynn transformation" << std::endl <<
		"11 - Theta Brezinski transformation" << std::endl <<
		"12 - Epsilon Algorithm V-3" << std::endl <<
		"13 - Levin - Recursion Algorithm" << std::endl <<
		"14 - Lubkin W-transformation" << std::endl <<
		"15 - Richardson Algorithm" << std::endl <<
		"16 - Ford-Sidi Algorithm" << std::endl <<
		std::endl;
}

/**
* @brief prints out all available fungus for testing
* @authors Bolshakov M.P.
* @edited by Kreynin R.G.
*/
inline static void print_test_function_info()
{
	std::cout << "Which function would you like to use for testing?" << std::endl <<
		"List of currently avaiable functions:" << std::endl <<
		"1 - cmp_sum_and_transform - showcases the difference between the transformed partial sum and the nontransformed one" << std::endl <<
		"2 - cmp_a_n_and_transform - showcases the difference between series' terms and transformed ones" << std::endl <<
		"3 - transformation_remainders - showcases the difference between series' sum and transformed partial sum" << std::endl <<
		"4 - cmp_transformations - showcases the difference between convergence of sums accelerated by different transformations" << std::endl <<
		"5 - eval_transform_time - evaluates the time it takes to transform series" << std::endl <<
		"6 - test all algorithms on summ" << std::endl
		<< std::endl;
}

/**
* @brief initialize LevinType transformations, usable for S,D,M
* @authors Naumov A.
* @edited by Yurov P.
*/
template<typename T, typename K, typename series_templ>
inline void init_levin(transformation_id_t id, std::unique_ptr<series_base<T,K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{
	bool recursive = false;
	char type;

	std::cout << std::endl;
	std::cout << "|--------------------------------------|" << std::endl;
	std::cout << "| choose what type of transformation u,t,d or v: "; std::cin >> type; std::cout << "|" << std::endl;
	if (id != transformation_id_t::M_algorithm)
	{
		std::cout << "| Use recurrence formula? 1<-true or 0<-false : "; std::cin >> recursive; std::cout << "|" << std::endl;
	}
	std::cout << "|--------------------------------------|" << std::endl;

	transform_base<T, K>* ptr = NULL;

	if (type == 'u') ptr = new u_transform<T, K>{};
	if (type == 't') ptr = new t_transform<T, K>{}; 
	if (type == 'v') { 
		if (!id == transformation_id_t::M_algorithm)
			ptr = new v_transform<T, K>{};
		else
			ptr = new v_transform_2<T, K>{};
	}
	if (type == 'd') ptr = new d_transform<T, K>{};

	if (ptr == NULL) throw std::domain_error("chosen wrong type of transformation");

	switch (id) {
		case transformation_id_t::S_algorithm:
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), ptr, recursive));
			return;
		case transformation_id_t::D_algorithm:
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), ptr, recursive));
			return;
		case transformation_id_t::M_algorithm:
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), ptr));
			return;
		default:
			throw std::domain_error("wrong id was given");
	}	
}

/**
* @brief initialize rho-WynnType transformations, usable for basic, Gamma, Gamma-Rho
* @authors Yurov P.
*/
template<typename T, typename K, typename series_templ>
inline void init_wynn(std::unique_ptr<series_base<T, K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{

	int type;

	std::cout << std::endl;
	std::cout << "|------------------------------------------|" << std::endl;
	std::cout << "| choose transformation variant:           |" << std::endl;
	std::cout << "| classic (0), gamma (1), gamma-rho (2): "; std::cin >> type;
	std::cout << "|------------------------------------------|" << std::endl;

	switch (type) {
	case 0:
		transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new rho_transform<T, K>{}));
		break;
	case 1:
		transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new generilized_transform<T, K>{}));
		break;
	case 2:
		transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new gamma_rho_transform<T, K>{}));
		break;
	default:
		throw std::domain_error("wrong transform variant");
		break;
	}
}

/**
* @brief The main testing function
* This function provides a convenient and interactive way to test out the convergence acceleration of various series
* @tparam T The type of the elements in the series, K The type of enumerating integer
* @authors Bolshakov M.P
* @edited by Kreynin R.G.
*/
template <typename T, typename K>
inline static void main_testing_function()
{

	//choosing series
	print_series_info();
	std::unique_ptr<series_base<T, K>> series;
	int series_id = 0;
	std::cin >> series_id;

	//choosing x
	std::cout << "Enter x - the argument for the functional series" << std::endl;
	T x = 0;
	std::cin >> x;

	//choosing series (cont.)
	std::set<int> alternating_series = { 2, 3, 7, 11, 15, 18, 19, 20, 21, 24, 26, 28, 30, 31};
	switch (series_id)
	{
	case series_id_t::exp_series_id:
		series.reset(new exp_series<T, K>(x));
		break;
	case series_id_t::cos_series_id:
		series.reset(new cos_series<T, K>(x));
		break;
	case series_id_t::sin_series_id:
		series.reset(new sin_series<T, K>(x));
		break;
	case series_id_t::cosh_series_id:
		series.reset(new cosh_series<T, K>(x));
		break;
	case series_id_t::sinh_series_id:
		series.reset(new sinh_series<T, K>(x));
		break;
	case series_id_t::bin_series_id:
		T alpha;
		std::cout << "Enter the value for constant alpha for the series" << std::endl;
		std::cin >> alpha;
		series.reset(new bin_series<T, K>(x, alpha));
		break;
	case series_id_t::four_arctan_series_id:
		series.reset(new four_arctan_series<T, K>(x));
		break;
	case series_id_t::ln1mx_series_id:
		series.reset(new ln1mx_series<T, K>(x));
		break;
	case series_id_t::mean_sinh_sin_series_id:
		series.reset(new mean_sinh_sin_series<T, K>(x));
		break;
	case series_id_t::exp_squared_erf_series_id:
		series.reset(new exp_squared_erf_series<T, K>(x));
		break;
	case series_id_t::xmb_Jb_two_series_id:
		K  b;
		std::cout << "Enter the value for constant b for the series" << std::endl;
		std::cin >> b;
		series.reset(new xmb_Jb_two_series<T, K>(x, b));
		break;
	case series_id_t::half_asin_two_x_series_id:
		series.reset(new half_asin_two_x_series<T, K>(x));
		break;
	case series_id_t::inverse_1mx_series_id:
		series.reset(new inverse_1mx_series<T, K>(x));
		break;
	case series_id_t::x_1mx_squared_series_id:
		series.reset(new x_1mx_squared_series<T, K>(x));
		break;
	case series_id_t::erf_series_id:
		series.reset(new erf_series<T, K>(x));
		break;
	case series_id_t::m_fact_1mx_mp1_inverse_series_id:
		K m;
		std::cout << "Enter the value for constant m for the series" << std::endl;
		std::cin >> m;
		series.reset(new m_fact_1mx_mp1_inverse_series<T, K>(x, m));
		break;
	case series_id_t::inverse_sqrt_1m4x_series_id:
		series.reset(new inverse_sqrt_1m4x_series<T, K>(x));
		break;
	case series_id_t::one_twelfth_3x2_pi2_series_id:
		series.reset(new one_twelfth_3x2_pi2_series<T, K>(x));
		break; 
	case series_id_t::x_twelfth_x2_pi2_series_id:
		series.reset(new x_twelfth_x2_pi2_series<T, K>(x));
		break;
	case series_id_t::ln2_series_id:
		series.reset(new ln2_series<T, K>());
		break;
	case series_id_t::one_series_id:
		series.reset(new one_series<T, K>());
		break;
	case series_id_t::minus_one_quarter_series_id:
		series.reset(new minus_one_quarter_series<T, K>());
		break;
	case series_id_t::pi_3_series_id:
		series.reset(new pi_3_series<T, K>());
		break;
	case series_id_t::pi_4_series_id:
		series.reset(new pi_4_series<T, K>());
		break;
	case series_id_t::pi_squared_6_minus_one_series_id:
		series.reset(new pi_squared_6_minus_one_series<T, K>());
		break;
	case series_id_t::three_minus_pi_series_id:
		series.reset(new three_minus_pi_series<T, K>());
		break;
	case series_id_t::one_twelfth_series_id:
		series.reset(new one_twelfth_series<T, K>());
		break;
	case series_id_t::eighth_pi_m_one_third_series_id:
		series.reset(new eighth_pi_m_one_third_series<T, K>());
		break;
	case series_id_t::one_third_pi_squared_m_nine_series_id:
		series.reset(new one_third_pi_squared_m_nine_series<T, K>());
		break;
	case series_id_t::four_ln2_m_3_series_id:
		series.reset(new four_ln2_m_3_series<T, K>());
		break;
	case series_id_t::exp_m_cos_x_sinsin_x_series_id:
		series.reset(new exp_m_cos_x_sinsin_x_series<T, K>(x));
		break;
	case series_id_t::pi_four_minus_ln2_halfed_series_id:
		series.reset(new pi_four_minus_ln2_halfed_series<T, K>(x));
		break;
	case series_id_t::five_pi_twelve_series_id:
		series.reset(new five_pi_twelve_series<T, K>(x));
		break;
	case series_id_t::x_two_series_id:
		series.reset(new x_two_series<T, K>(x));
		break;
	case series_id_t::pi_six_min_half_series_id:
		series.reset(new pi_six_min_half_series<T, K>(x));
		break;
	case series_id_t::x_two_throught_squares_id:
		series.reset(new x_two_throught_squares_series<T, K>(x));
		break;
	case series_id_t::minus_one_ned_in_n_series_id:
		series.reset(new minus_one_ned_in_n_series<T, K>(x));
		break;
	case series_id_t::minus_one_n_fact_n_in_n_series_id:
		series.reset(new minus_one_n_fact_n_in_n_series<T, K>(x));
		break;
	case series_id_t::ln_x_plus_one_x_minus_one_halfed_series_id:
		series.reset(new ln_x_plus_one_x_minus_one_halfed_series<T, K>(x));
		break;
	case series_id_t::two_arcsin_square_x_halfed_series_id:
		series.reset(new two_arcsin_square_x_halfed_series<T, K>(x));
		break;
	default:
		throw std::domain_error("wrong series_id");
	}

	//choosing transformation
	print_transformation_info();
	int transformation_id = 0;
	std::cin >> transformation_id;
	std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform;
	switch (transformation_id)
	{
	case transformation_id_t::shanks_transformation_id:
		if (alternating_series.contains(series_id))
			transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
		else
			transform.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::epsilon_algorithm_id:
		transform.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::levin_algorithm_id:
		transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::epsilon_algorithm_2_id:
		transform.reset(new epsilon_algorithm_two<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::S_algorithm:
		init_levin(transformation_id_t::S_algorithm, series, transform);
		break;
	case transformation_id_t::D_algorithm:
		init_levin(transformation_id_t::D_algorithm, series, transform);
		break;
	case transformation_id_t::chang_epsilon_algorithm:
		transform.reset(new chang_whynn_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::M_algorithm:
		init_levin(transformation_id_t::M_algorithm, series, transform);
		break;
	case transformation_id_t::weniger_transformation:
		transform.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::rho_wynn_transformation_id:
		init_wynn(series, transform);
		break;
	case transformation_id_t::brezinski_theta_transformation_id:
		transform.reset(new theta_brezinski_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::epsilon_algorithm_3_id:
		transform.reset(new epsilon_algorithm_three<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::levin_recursion_id:
		transform.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::W_algorithm_id:
		transform.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::richardson_algorithm_id:
		transform.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::Ford_Sidi_algorithm_id:
		transform.reset(new ford_sidi_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	default:
		throw std::domain_error("wrong transformation_id");
	}

	//choosing testing function
	print_test_function_info();
	int function_id = 0;
	std::cin >> function_id;
	int n = 0;
	int order = 0;
	std::cout << "Enter n and order:" << std::endl;
	std::cin >> n >> order;
	switch (function_id)
	{
	case test_function_id_t::cmp_sum_and_transform_id:
		cmp_sum_and_transform(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::cmp_a_n_and_transform_id:
		cmp_a_n_and_transform(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::transformation_remainder_id:
		transformation_remainders(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::cmp_transformations_id:
	{
		int cmop_transformation_id = 0;
		print_transformation_info();
		std::cin >> cmop_transformation_id;

		std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform2;
		
		switch (cmop_transformation_id)
		{
		case transformation_id_t::shanks_transformation_id:
			if (alternating_series.contains(series_id))
				transform2.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform2.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_id:
			transform2.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::levin_algorithm_id:
			transform2.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_2_id:
			transform2.reset(new epsilon_algorithm_two<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::S_algorithm:
			init_levin(transformation_id_t::S_algorithm, series, transform2);
			break;
		case transformation_id_t::D_algorithm:
			init_levin(transformation_id_t::D_algorithm, series, transform2);
			break;
		case transformation_id_t::chang_epsilon_algorithm:
			transform2.reset(new chang_whynn_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::M_algorithm:
			init_levin(transformation_id_t::M_algorithm, series, transform2);
			break;
		case transformation_id_t::weniger_transformation:
			transform2.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::rho_wynn_transformation_id:
			init_wynn(series, transform2);
			break;
		case transformation_id_t::brezinski_theta_transformation_id:
			transform2.reset(new theta_brezinski_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_3_id:
			transform2.reset(new epsilon_algorithm_three<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::levin_recursion_id:
			transform2.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get()));
		case transformation_id_t::W_algorithm_id:
			transform2.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::richardson_algorithm_id:
			transform2.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
		case transformation_id_t::Ford_Sidi_algorithm_id:
			transform2.reset(new ford_sidi_algorithm<T, K, decltype(series.get())>(series.get()));
		default:
			throw std::domain_error("wrong algorithm id");
		}

		cmp_transformations(n, order, std::move(series.get()), std::move(transform.get()), std::move(transform2.get()));
	}
	case test_function_id_t::eval_transform_time_id:
		eval_transform_time(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::test_all_transforms_id: //Testing all functions for series

		for (int i = 1; i <= n; i++) 
		{
			print_sum(i, std::move(series.get()));
			
			//shanks
			if (alternating_series.contains(series_id))
				transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//epsilon v-1
			transform.reset(new epsilon_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//epsilon v-2
			transform.reset(new epsilon_algorithm_two<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//epsilon v-3
			transform.reset(new epsilon_algorithm_three<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new rho_transform<T, K>{}));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new generilized_transform<T, K>{}));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new gamma_rho_transform<T, K>{}));
			print_transform(i, order, std::move(transform.get()));

			//theta-brezinski
			transform.reset(new theta_brezinski_algorithm<T, K, decltype(series.get())>(series.get()));

			//chang epsilon wynn
			transform.reset(new chang_whynn_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//levin standart
			transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//levin recurcive
			transform.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//levin-sidi S U
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{},false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S T
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S D
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S V
			transform.reset(new levi_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));

			//levin-sidi D U
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D T
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D D
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D V
			transform.reset(new drummonds_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M U
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M T
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M D
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M V
			transform.reset(new M_levin_sidi_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform_2<T, K>{}));
			print_transform(i, order, std::move(transform.get()));
			//
			
			//weniger
			transform.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//lubkin W
			transform.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));
			
			//Richardson
			transform.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//Ford-Sidi
			transform.reset(new ford_sidi_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			std::cout << std::endl;
		}

		break;
	default:
		throw std::domain_error("wrong function_id");
	}
}
