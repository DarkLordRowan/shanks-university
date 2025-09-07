/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 */

#pragma once
#include <memory>
#include <set>

#include "test_functions.h"
#include "./series_acceleration/series_acceleration_all.hpp"
#include "series_acceleration/series_acceleration.hpp"
#include "series/series_all.hpp"

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
	Ford_Sidi_algorithm_id,
	Ford_Sidi_algorithm_two_id
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
	two_arcsin_square_x_halfed_series_id,
	pi_squared_twelve_series_id,
	pi_cubed_32_series_id,
	minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id,
	two_ln2_series_id,
	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id,
	pi_minus_x_2_id,
	half_multi_ln_1div2multi1minuscosx_id,
	half_minus_sinx_multi_pi_4_id,
	ln_1plussqrt1plusxsquare_minus_ln_2_id,
	ln_cosx_id,
	ln_sinx_minus_ln_x_id,
	pi_8_cosx_square_minus_1_div_3_cosx_id,
	sqrt_oneminussqrtoneminusx_div_x_id,
	one_minus_sqrt_1minus4x_div_2x_id,
	arcsin_x_minus_x_series_id,
	pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id,
	abs_sin_x_minus_2_div_pi_series_id,
	pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id,
	minus_3_div_4_or_x_minus_3_div_4_series_id,
	ten_minus_x_series_id,
	x_series_id,
	minus_x_minus_pi_4_or_minus_pi_4_series_id,
	one_div_two_minus_x_multi_three_plus_x_series_id,
	Si_x_series_id,
	Ci_x_series_id,
	Riemann_zeta_func_series_id,
	Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id,
	xsquareplus3_div_xsquareplus2multix_minus_1_series_id,
	arcsin_x_series_id,
	arctg_x_series_id,
	K_x_series_id,
	E_x_series_id,
	sqrt_1plusx_series_id,
	Lambert_W_func_series_id,
	Incomplete_Gamma_func_series_id,
	Series_with_ln_number1_id,
	Series_with_ln_number2_id,
	pi_series_id,
	x_min_sqrt_x_series_id,
	arctan_x2_series_id,
	ln1px4_series_id,
	sin_x2_series_id,
	arctan_x3_series_id,
	arcsin_x2_series_id,
	ln1_m_x2_series_id,
	artanh_x_series_id,
	arcsinh_x_series_id,
	cos_x2_series_id,
	sinh_x2_series_id,
	arctanh_x2_series_id,
	cos3xmin1_div_xsqare_series_id,
	two_degree_x_series_id,
	sqrt_1plusx_min_1_min_x_div_2_series_id,
	ln13_min_ln7_div_7_series_id,
	Ja_x_series_id,
	one_div_sqrt2_sin_xdivsqrt2_series_id,
	ln_1plusx_div_1plusx2_id,
	cos_sqrt_x_id,
	ln_1_plus_x3_id,
	x_div_1minx_id,
	x_div_1minx2_id,
	gamma_series_id
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
static void print_series_info()
{
	std::cout << "Which series' convergence would you like to accelerate?" << '\n' <<
		"List of currently avaiable series:" << '\n' <<
		"1 - exp_series" << '\n' <<
		"2 - cos_series" << '\n' <<
		"3 - sin_series" << '\n' <<
		"4 - cosh_series" << '\n' <<
		"5 - sinh_series" << '\n' <<
		"6 - bin_series" << '\n' <<
		"7 - four_arctan_series" << '\n' <<
		"8 - ln1mx_series" << '\n' <<
		"9 - mean_sinh_sin_series" << '\n' <<
		"10 - exp_squared_erf_series" << '\n' <<
		"11 - xmb_Jb_two_series" << '\n' <<
		"12 - half_asin_two_x_series" << '\n' <<
		"13 - inverse_1mx_series" << '\n' <<
		"14 - x_1mx_squared_series" << '\n' <<
		"15 - erf_series" << '\n' <<
		"16 - m_fact_1mx_mp1_inverse_series" << '\n' <<
		"17 - inverse_sqrt_1m4x_series" << '\n' <<
		"18 - one_twelfth_3x2_pi2_series" << '\n' <<
		"19 - x_twelfth_x2_pi2_series" << '\n' <<
		"20 - ln2_series_id" << '\n' <<
		"21 - one_series_id" << '\n' <<
		"22 - minus_one_quarter_series_id" << '\n' <<
		"23 - pi_3_series" << '\n' <<
		"24 - pi_4_series" << '\n' <<
		"25 - pi_squared_6_minus_one_series" << '\n' <<
		"26 - three_minus_pi_series" << '\n' <<
		"27 - one_twelfth_series" << '\n' <<
		"28 - eighth_pi_m_one_third_series" << '\n' <<
		"29 - one_third_pi_squared_m_nine_series" << '\n' <<
		"30 - four_ln2_m_3_series" << '\n' <<
		"31 - exp_m_cos_x_sinsin_x_series" << '\n' <<
		"32 - pi_four_minus_ln2_halfed_series" << '\n' <<
		"33 - five_pi_twelve_series" << '\n' <<
		"34 - x_two_series" << '\n' <<
		"35 - pi_six_min_half_series" << '\n' <<
		"36 - x_two_throught_squares" << '\n' <<
		"37 - minus_one_ned_in_n_series" << '\n' <<
		"38 - minus_one_n_fact_n_in_n_series" << '\n' <<
		"39 - ln_x_plus_one_x_minus_one_halfed_series" << '\n' <<
		"40 - two_arcsin_square_x_halfed_series" << '\n' <<
		"41 - pi_squared_twelve_series" << '\n' <<
		"42 - pi_cubed_32_series" << '\n' <<
		"43 - minus_three_plus_ln3_three_devided_two_plus_two_ln2_series" << '\n' <<
		"44 - two_ln2_series" << '\n' <<
		"45 - pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series" << '\n' <<
		"46 - pi_minus_x_2" << '\n' <<
		"47 - half_multi_ln_1div2multi1minuscosx" << '\n' <<
		"48 - half_minus_sinx_multi_pi_4" << '\n' <<
		"49 - ln_1plussqrt1plusxsquare_minus_ln_2" << '\n' <<
		"50 - ln_cosx" << '\n' <<
		"51 - ln_sinx_minus_ln_x" << '\n' <<
		"52 - pi_8_cosx_square_minus_1_div_3_cosx" << '\n' <<
		"53 - sqrt_oneminussqrtoneminusx_div_x" << '\n' <<
		"54 - one_minus_sqrt_1minus4x_div_2x" << '\n' <<
		"55 - arcsin_x_minus_x_series" << '\n' <<
		"56 - pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series" << '\n' <<
		"57 - abs_sin_x_minus_2_div_pi_series" << '\n' <<
		"58 - pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series" << '\n' <<
		"59 - minus_3_div_4_or_x_minus_3_div_4_series" << '\n' <<
		"60 - ten_minus_x_series" << '\n' <<
		"61 - x_series" << '\n' <<
		"62 - minus_x_minus_pi_4_or_minus_pi_4_series" << '\n' <<
		"63 - one_div_two_minus_x_multi_three_plus_x_series" << '\n' <<
		"64 - Si_x_series" << '\n' <<
		"65 - Ci_x_series" << '\n' <<
		"66 - Riemann_zeta_func_series" << '\n' <<
		"67 - Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series" << '\n' <<
		"68 - xsquareplus3_div_xsquareplus2multix_minus_1_series" << '\n' <<
		"69 - arcsin_x_series" << '\n' <<
		"70 - arctg_x_series" << '\n' <<
		"71 - K_x_series" << '\n' <<
		"72 - E_x_series" << '\n' <<
		"73 - sqrt_1plusx_series" << '\n' <<
		"74 - Lambert_W_func_series" << '\n' <<
		"75 - Incomplete_Gamma_func_series" << '\n' <<
		"76 - Series_with_ln_number1" << '\n' <<
		"77 - Series_with_ln_number2" << '\n' <<
		"78 - pi_series" << '\n' <<
		"79 - x_min_sqrt_x_series" << '\n' <<
		"80 - arctan_x2_series" << '\n' <<
		"81 - ln1px4_series" << '\n' <<
		"82 - sin_x2_series" << '\n' <<
		"83 - arctan_x3_series" << '\n' <<
		"84 - arcsin_x2_series" << '\n' <<
		"85 - ln1_m_x2_series" << '\n' <<
		"86 - artanh_x_series" << '\n' <<
		"87 - arcsinh_x_series" << '\n' <<
		"88 - cos_x2_series" << '\n' <<
		"89 - sinh_x2_series" << '\n' <<
		"90 - arctanh_x2_series" << '\n' <<
		"91 - cos3xmin1_div_xsqare_series" << '\n' <<
		"92 - two_degree_x_series" << '\n' <<
		"93 - sqrt_1plusx_min_1_min_x_div_2_series" << '\n' <<
		"94 - ln13_min_ln7_div_7_series" << '\n' <<
		"95 - Ja_x_series" << '\n' <<
		"96 - one_div_sqrt2_sin_xdivsqrt2_series" << '\n' <<
		"97 - ln_1plusx_div_1plusx2" << '\n' <<
		"98 - cos_sqrt_x" << '\n' <<
		"99 - ln_1_plus_x3" << '\n' <<
		"100 - x_div_1minx" << '\n' <<
		"101 - x_div_1minx2" << '\n' <<
		"102 - gamma_series" << '\n' <<
		'\n';
}

/**
* @brief prints out all available transformations for testing
* @authors Bolshakov M.P.
* @edited by Kreynin R.G.
*/
static void print_transformation_info()
{
	std::cout << "Which transformation would you like to test?" << '\n' <<
		"List of currently avaiable series:" << '\n' <<
		"1 - Shanks Transformation" << '\n' <<
		"2 - Epsilon Algorithm" << '\n' <<
		"3 - Levin Algorithm" << '\n' <<
		"4 - Epsilon Algorithm V-2" << '\n' <<
		"5 - S-transformation" << '\n' <<
		"6 - D-transformation" << '\n' <<
		"7 - Chang - Wynn - Epsilon Algorithm" << '\n' <<
		"8 - M-transformation" << '\n' <<
		"9 - Weniger transformation" << '\n' <<
		"10 - Rho - Wynn transformation" << '\n' <<
		"11 - Theta Brezinski transformation" << '\n' <<
		"12 - Epsilon Algorithm V-3" << '\n' <<
		"13 - Levin - Recursion Algorithm" << '\n' <<
		"14 - Lubkin W-transformation" << '\n' <<
		"15 - Richardson Algorithm" << '\n' <<
		"16 - Ford-Sidi Algorithm" << '\n' <<
		"17 - Ford-Sidi Algorithm V-2" << '\n' <<
		'\n';
}

/**
* @brief prints out all available fungus for testing
* @authors Bolshakov M.P.
* @edited by Kreynin R.G.
*/
static void print_test_function_info()
{
	std::cout << "Which function would you like to use for testing?" << '\n' <<
		"List of currently avaiable functions:" << '\n' <<
		"1 - cmp_sum_and_transform - showcases the difference between the transformed partial sum and the nontransformed one" << '\n' <<
		"2 - cmp_a_n_and_transform - showcases the difference between series' terms and transformed ones" << '\n' <<
		"3 - transformation_remainders - showcases the difference between series' sum and transformed partial sum" << '\n' <<
		"4 - cmp_transformations - showcases the difference between convergence of sums accelerated by different transformations" << '\n' <<
		"5 - eval_transform_time - evaluates the time it takes to transform series" << '\n' <<
		"6 - test all algorithms on summ" << '\n'
		<< '\n';
}

/**
* @brief initialize LevinType transformations, usable for S,D,M
* @authors Naumov A.
* @edited by Yurov P.
*/
template<typename T, typename K, typename series_templ>
 void init_levin(transformation_id_t id, std::unique_ptr<series_base<T, K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{
	bool recursive = false;
	bool standart = false;
	char type;

	std::cout << '\n';
	std::cout << "|--------------------------------------|" << '\n';
	std::cout << "| choose what type of transformation u,t,d or v: "; std::cin >> type; std::cout << "|" << '\n';
	if (id != M_algorithm)
	{
		std::cout << "| Use recurrence formula? 1<-true or 0<-false : "; std::cin >> recursive; std::cout << "|" << '\n';
	}
	std::cout << "|--------------------------------------|" << '\n';

	T beta = T{};	//parameter for LevinType transformations algorithm
	T gamma = T{};	//parameter for LevinType transformations algorithm

	transform_base<T, K>* ptr = nullptr;

	if (type == 'u') ptr = new u_transform<T, K>{};
	if (type == 't') ptr = new t_transform<T, K>{};
	if (type == 'v') {
		if (id != M_algorithm)
			ptr = new v_transform<T, K>{};
		else
			ptr = new v_transform_2<T, K>{};
	}
	if (type == 'd') ptr = new d_transform<T, K>{};

	if (ptr == nullptr) throw std::domain_error("chosen wrong type of transformation");

	switch (id) {
	case S_algorithm:

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart beta value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter beta: "; std::cin >> beta;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else beta = 1;

		transform.reset(new levin_sidi_S_algorithm<T, K, decltype(series.get())>(series.get(), ptr, recursive, beta));
		return;
	case D_algorithm:
		transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), ptr, recursive));
		return;
	case M_algorithm:

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart gamma value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter gamma: "; std::cin >> gamma;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else gamma = 10;

		transform.reset(new levin_sidi_M_algorithm<T, K, decltype(series.get())>(series.get(), ptr, gamma));
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
 void init_wynn(std::unique_ptr<series_base<T, K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{

	int type;
	bool standart = false;
	T gamma = T{};	//parameter for gamma modification
	T RHO = T{};	//parameter for gamma-rho modification

	std::cout << '\n';
	std::cout << "|------------------------------------------|" << '\n';
	std::cout << "| choose transformation variant:           |" << '\n';
	std::cout << "| classic (0), gamma (1), gamma-rho (2): "; std::cin >> type;
	std::cout << "|------------------------------------------|" << '\n';

	switch (type) {
	case 0:
		transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new rho_transform<T, K>{}));
		break;
	case 1:
		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart gamma value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter gamma: "; std::cin >> gamma;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else gamma = 2;

		transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new generilized_transform<T, K>{}, gamma));
		break;
	case 2:
		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart gamma value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter gamma: "; std::cin >> gamma;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else gamma = 2;

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart RHO value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter gamma: "; std::cin >> gamma;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else RHO = 1;

		transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new gamma_rho_transform<T, K>{}, gamma, RHO));
		break;
	default:
		throw std::domain_error("wrong transform variant");
		break;
	}
}

/**
* @brief initialize levin_recursion transformation
* @authors Maximov A.K.
*/
template<typename T, typename K, typename series_templ>
 void init_levin_recursion(std::unique_ptr<series_base<T, K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{
	bool standart = false;
	T beta = T{};	//parameter for levin_recursion algorithm

	std::cout << '\n';
	std::cout << "|------------------------------------------|" << '\n';
	std::cout << "| Use standart beta value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
	std::cout << "|------------------------------------------|" << '\n';

	if (!standart) {
		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Enter parameter beta: "; std::cin >> beta;
		std::cout << "|------------------------------------------|" << '\n';
	}
	else beta = -1.5;

	transform.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get(), beta));
}

/**
* @brief initialize epsilon_algorithm_3_id transformation
* @authors Maximov A.K.
*/
template<typename T, typename K, typename series_templ>
 void init_epsilon_3(std::unique_ptr<series_base<T, K>>& series, std::unique_ptr<series_acceleration<T, K, series_templ>>& transform)
{

	bool standart = false;
	T epsilon = T{};	//parameter for epsilon 3 algorithm

	std::cout << '\n';
	std::cout << "|------------------------------------------|" << '\n';
	std::cout << "| Use standart epsilon value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
	std::cout << "|------------------------------------------|" << '\n';

	if (!standart) {
		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Enter parameter epsilon: "; std::cin >> epsilon;
		std::cout << "|------------------------------------------|" << '\n';
	}
	else epsilon = T(1e-3);

	transform.reset(new epsilon_three_algorithm<T, K, decltype(series.get())>(series.get(), epsilon));
}

/**
* @brief The main testing function
* This function provides a convenient and interactive way to test out the convergence acceleration of various series
* @tparam T The type of the elements in the series, K The type of enumerating integer
* @authors Bolshakov M.P
* @edited by Kreynin R.G.
*/
template <typename T, typename K>
 static void main_testing_function()
{

	//choosing series
	print_series_info();
	std::unique_ptr<series_base<T, K>> series;
	int series_id = 0;
	std::cin >> series_id;

	//choosing x
	std::cout << "Enter x - the argument for the functional series" << '\n';
	T x = 0;
	std::cin >> x;

	//choosing series (cont.)
	const std::set alternating_series = { 2, 3, 7, 11, 15, 18, 19, 20, 21, 24, 26, 28, 30, 31 };
	switch (series_id)
	{
	case exp_series_id:
		series.reset(new exp_series<T, K>(x));
		break;
	case cos_series_id:
		series.reset(new cos_series<T, K>(x));
		break;
	case sin_series_id:
		series.reset(new sin_series<T, K>(x));
		break;
	case cosh_series_id:
		series.reset(new cosh_series<T, K>(x));
		break;
	case sinh_series_id:
		series.reset(new sinh_series<T, K>(x));
		break;
	case bin_series_id:
		T alpha;
		std::cout << "Enter the value for constant alpha for the series" << '\n';
		std::cin >> alpha;
		series.reset(new bin_series<T, K>(x, alpha));
		break;
	case four_arctan_series_id:
		series.reset(new four_arctan_series<T, K>(x));
		break;
	case ln1mx_series_id:
		series.reset(new ln1mx_series<T, K>(x));
		break;
	case mean_sinh_sin_series_id:
		series.reset(new mean_sinh_sin_series<T, K>(x));
		break;
	case exp_squared_erf_series_id:
		series.reset(new exp_squared_erf_series<T, K>(x));
		break;
	case xmb_Jb_two_series_id:
		K b;
		std::cout << "Enter the value for constant b for the series" << '\n';
		std::cin >> b;
		series.reset(new xmb_Jb_two_series<T, K>(x, b));
		break;
	case half_asin_two_x_series_id:
		series.reset(new half_asin_two_x_series<T, K>(x));
		break;
	case inverse_1mx_series_id:
		series.reset(new inverse_1mx_series<T, K>(x));
		break;
	case x_1mx_squared_series_id:
		series.reset(new x_1mx_squared_series<T, K>(x));
		break;
	case erf_series_id:
		series.reset(new erf_series<T, K>(x));
		break;
	case m_fact_1mx_mp1_inverse_series_id:
		K m;
		std::cout << "Enter the value for constant m for the series" << '\n';
		std::cin >> m;
		series.reset(new m_fact_1mx_mp1_inverse_series<T, K>(x, m));
		break;
	case inverse_sqrt_1m4x_series_id:
		series.reset(new inverse_sqrt_1m4x_series<T, K>(x));
		break;
	case one_twelfth_3x2_pi2_series_id:
		series.reset(new one_twelfth_3x2_pi2_series<T, K>(x));
		break;
	case x_twelfth_x2_pi2_series_id:
		series.reset(new x_twelfth_x2_pi2_series<T, K>(x));
		break;
	case ln2_series_id:
		series.reset(new ln2_series<T, K>());
		break;
	case one_series_id:
		series.reset(new one_series<T, K>());
		break;
	case minus_one_quarter_series_id:
		series.reset(new minus_one_quarter_series<T, K>());
		break;
	case pi_3_series_id:
		series.reset(new pi_3_series<T, K>());
		break;
	case pi_4_series_id:
		series.reset(new pi_4_series<T, K>());
		break;
	case pi_squared_6_minus_one_series_id:
		series.reset(new pi_squared_6_minus_one_series<T, K>());
		break;
	case three_minus_pi_series_id:
		series.reset(new three_minus_pi_series<T, K>());
		break;
	case one_twelfth_series_id:
		series.reset(new one_twelfth_series<T, K>());
		break;
	case eighth_pi_m_one_third_series_id:
		series.reset(new eighth_pi_m_one_third_series<T, K>());
		break;
	case one_third_pi_squared_m_nine_series_id:
		series.reset(new one_third_pi_squared_m_nine_series<T, K>());
		break;
	case four_ln2_m_3_series_id:
		series.reset(new four_ln2_m_3_series<T, K>());
		break;
	case exp_m_cos_x_sinsin_x_series_id:
		series.reset(new exp_m_cos_x_sinsin_x_series<T, K>(x));
		break;
	case pi_four_minus_ln2_halfed_series_id:
		series.reset(new pi_four_minus_ln2_halfed_series<T, K>(x));
		break;
	case five_pi_twelve_series_id:
		series.reset(new five_pi_twelve_series<T, K>(x));
		break;
	case x_two_series_id:
		series.reset(new x_two_series<T, K>(x));
		break;
	case pi_six_min_half_series_id:
		series.reset(new pi_six_min_half_series<T, K>(x));
		break;
	case x_two_throught_squares_id:
		series.reset(new x_two_throught_squares_series<T, K>(x));
		break;
	case minus_one_ned_in_n_series_id:
		series.reset(new minus_one_ned_in_n_series<T, K>(x));
		break;
	case minus_one_n_fact_n_in_n_series_id:
		series.reset(new minus_one_n_fact_n_in_n_series<T, K>(x));
		break;
	case ln_x_plus_one_x_minus_one_halfed_series_id:
		series.reset(new ln_x_plus_one_x_minus_one_halfed_series<T, K>(x));
		break;
	case two_arcsin_square_x_halfed_series_id:
		series.reset(new two_arcsin_square_x_halfed_series<T, K>(x));
		break;
	case pi_squared_twelve_series_id:
		series.reset(new pi_squared_twelve_series<T, K>());
		break;
	case pi_cubed_32_series_id:
		series.reset(new pi_cubed_32_series<T, K>());
		break;
	case minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id:
		series.reset(new minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>());
		break;
	case two_ln2_series_id:
		series.reset(new two_ln2_series<T, K>());
		break;
	case pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id:
		series.reset(new pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>(x));
		break;
	case pi_minus_x_2_id:
		series.reset(new pi_minus_x_2<T, K>(x));
		break;
	case half_multi_ln_1div2multi1minuscosx_id:
		series.reset(new half_multi_ln_1div2multi1minuscosx<T, K>(x));
		break;
	case half_minus_sinx_multi_pi_4_id:
		series.reset(new half_minus_sinx_multi_pi_4<T, K>(x));
		break;
	case ln_1plussqrt1plusxsquare_minus_ln_2_id:
		series.reset(new ln_1plussqrt1plusxsquare_minus_ln_2<T, K>(x));
		break;
	case ln_cosx_id:
		series.reset(new ln_cosx<T, K>(x));
		break;
	case ln_sinx_minus_ln_x_id:
		series.reset(new ln_sinx_minus_ln_x<T, K>(x));
		break;
	case pi_8_cosx_square_minus_1_div_3_cosx_id:
		series.reset(new pi_8_cosx_square_minus_1_div_3_cosx<T, K>(x));
		break;
	case sqrt_oneminussqrtoneminusx_div_x_id:
		series.reset(new sqrt_oneminussqrtoneminusx_div_x<T, K>(x));
		break;
	case one_minus_sqrt_1minus4x_div_2x_id:
		series.reset(new one_minus_sqrt_1minus4x_div_2x<T, K>(x));
		break;
	case arcsin_x_minus_x_series_id:
		series.reset(new arcsin_x_minus_x_series<T, K>(x));
		break;
	case pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id:
		series.reset(new pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>(x));
		break;
	case abs_sin_x_minus_2_div_pi_series_id:
		series.reset(new abs_sin_x_minus_2_div_pi_series<T, K>(x));
		break;
	case pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id:
		series.reset(new pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>(x));
		break;
	case minus_3_div_4_or_x_minus_3_div_4_series_id:
		series.reset(new minus_3_div_4_or_x_minus_3_div_4_series<T, K>(x));
		break;
	case ten_minus_x_series_id:
		series.reset(new ten_minus_x_series<T, K>(x));
		break;
	case x_series_id:
		series.reset(new x_series<T, K>(x));
		break;
	case minus_x_minus_pi_4_or_minus_pi_4_series_id:
		series.reset(new minus_x_minus_pi_4_or_minus_pi_4_series<T, K>(x));
		break;
	case one_div_two_minus_x_multi_three_plus_x_series_id:
		series.reset(new one_div_two_minus_x_multi_three_plus_x_series<T, K>(x));
		break;
	case Si_x_series_id:
		series.reset(new Si_x_series<T, K>(x));
		break;
	case Ci_x_series_id:
		series.reset(new Ci_x_series<T, K>(x));
		break;
	case Riemann_zeta_func_series_id:
		series.reset(new Riemann_zeta_func_series<T, K>(x));
		break;
	case Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id:
		series.reset(new Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>(x));
		break;
	case xsquareplus3_div_xsquareplus2multix_minus_1_series_id:
		series.reset(new xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>(x));
		break;
	case arcsin_x_series_id:
		series.reset(new arcsin_x_series<T, K>(x));
		break;
	case arctg_x_series_id:
		series.reset(new arctg_x_series<T, K>(x));
		break;
	case K_x_series_id:
		series.reset(new K_x_series<T, K>(x));
		break;
	case E_x_series_id:
		series.reset(new E_x_series<T, K>(x));
		break;
	case sqrt_1plusx_series_id:
		series.reset(new sqrt_1plusx_series<T, K>(x));
		break;
	case Lambert_W_func_series_id:
		series.reset(new Lambert_W_func_series<T, K>(x));
		break;
	case Incomplete_Gamma_func_series_id:
		T s;
		std::cout << "Enter the value for constant s for the series" << '\n';
		std::cin >> s;
		series.reset(new Incomplete_Gamma_func_series<T, K>(x, s));
		break;
	case Series_with_ln_number1_id:
		series.reset(new Series_with_ln_number1<T, K>());
		break;
	case Series_with_ln_number2_id:
		series.reset(new Series_with_ln_number2<T, K>());
		break;
	case pi_series_id:
		series.reset(new pi_series<T, K>());
		break;
	case x_min_sqrt_x_series_id:
		series.reset(new x_min_sqrt_x_series<T, K>(x));
		break;
	case arctan_x2_series_id:
		series.reset(new arctan_x2_series<T, K>(x));
		break;
	case ln1px4_series_id:
		series.reset(new ln1px4_series<T, K>(x));
		break;
	case sin_x2_series_id:
		series.reset(new sin_x2_series<T, K>(x));
		break;
	case arctan_x3_series_id:
		series.reset(new arctan_x3_series<T, K>(x));
		break;
	case arcsin_x2_series_id:
		series.reset(new arcsin_x2_series<T, K>(x));
		break;
	case ln1_m_x2_series_id:
		series.reset(new ln1_m_x2_series<T, K>(x));
		break;
	case artanh_x_series_id:
		series.reset(new artanh_x_series<T, K>(x));
		break;
	case arcsinh_x_series_id:
		series.reset(new arcsinh_x_series<T, K>(x));
		break;
	case cos_x2_series_id:
		series.reset(new cos_x2_series<T, K>(x));
		break;
	case sinh_x2_series_id:
		series.reset(new sinh_x2_series<T, K>(x));
		break;
	case arctanh_x2_series_id:
		series.reset(new arctanh_x2_series<T, K>(x));
		break;
	case cos3xmin1_div_xsqare_series_id:
		series.reset(new cos3xmin1_div_xsqare_series<T, K>(x));
		break;
	case two_degree_x_series_id:
		series.reset(new two_degree_x_series<T, K>(x));
		break;
	case sqrt_1plusx_min_1_min_x_div_2_series_id:
		series.reset(new sqrt_1plusx_min_1_min_x_div_2_series<T, K>(x));
		break;
	case ln13_min_ln7_div_7_series_id:
		series.reset(new ln13_min_ln7_div_7_series<T, K>());
		break;
	case Ja_x_series_id:
		T a;
		std::cout << "Enter the value for constant a for the series" << '\n';
		std::cin >> a;
		series.reset(new Ja_x_series<T, K>(x, a));
		break;
	case one_div_sqrt2_sin_xdivsqrt2_series_id:
		series.reset(new one_div_sqrt2_sin_xdivsqrt2_series<T, K>(x));
		break;
	case ln_1plusx_div_1plusx2_id:
		series.reset(new ln_1plusx_div_1plusx2<T, K>(x));
		break;
	case cos_sqrt_x_id:
		series.reset(new cos_sqrt_x<T, K>(x));
		break;
	case ln_1_plus_x3_id:
		series.reset(new ln_1_plus_x3<T, K>(x));
		break;
	case x_div_1minx_id:
		series.reset(new x_div_1minx<T, K>(x));
		break;
	case x_div_1minx2_id:
		series.reset(new x_div_1minx2<T, K>(x));
		break;
	case gamma_series_id:
		T t;
		std::cout << "Enter the parameter t in the gamma series" << '\n';
		std::cin >> t;
		series.reset(new gamma_series<T, K>(t, x));
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
	case shanks_transformation_id:
		if (alternating_series.contains(series_id))
			transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
		else
			transform.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
		break;
	case epsilon_algorithm_id:
		transform.reset(new epsilon_one_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case levin_algorithm_id:
		transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case epsilon_algorithm_2_id:
		transform.reset(new epsilon_two_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case S_algorithm:
		init_levin(S_algorithm, series, transform);
		break;
	case D_algorithm:
		init_levin(D_algorithm, series, transform);
		break;
	case chang_epsilon_algorithm:
		transform.reset(new chang_wynn_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case M_algorithm:
		init_levin(M_algorithm, series, transform);
		break;
	case weniger_transformation:
		transform.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case rho_wynn_transformation_id:
		init_wynn(series, transform);
		break;
	case brezinski_theta_transformation_id:
		transform.reset(new brezinski_theta_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case epsilon_algorithm_3_id:
		init_epsilon_3(series, transform);
		break;
	case levin_recursion_id:
		init_levin_recursion(series, transform);
		break;
	case W_algorithm_id:
		transform.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case richardson_algorithm_id:
		transform.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case Ford_Sidi_algorithm_id:
		transform.reset(new ford_sidi_one_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case Ford_Sidi_algorithm_two_id:
		transform.reset(new ford_sidi_two_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	default:
		throw std::domain_error("wrong transformation_id");
	}

	//choosing testing function

	bool standart = false;

	T beta_Levin_S_algorithm = T{};			//parameter for LevinType transformations algorithm
	T gamma_Levin_M_algorithm = T{};		//parameter for LevinType transformations algorithm
	T gamma_rho_Wynn_algorithm = T{};		//parameter for gamma modification
	T RHO_rho_Wynn_algorithm = T{};			//parameter for gamma-rho modification
	T beta_levin_recursion_algorithm = T{};	//parameter for levin_recursion algorithm
	T epsilon_algorithm_3 = T{};			//parameter for levin_recursion algorithm

	print_test_function_info();
	int function_id = 0;
	std::cin >> function_id;
	int n = 0;
	int order = 0;
	std::cout << "Enter n and order:" << '\n';
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
		case shanks_transformation_id:
			if (alternating_series.contains(series_id))
				transform2.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform2.reset(new shanks_transform<T, K, decltype(series.get())>(series.get()));
			break;
		case epsilon_algorithm_id:
			transform2.reset(new epsilon_one_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case levin_algorithm_id:
			transform2.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case epsilon_algorithm_2_id:
			transform2.reset(new epsilon_two_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case S_algorithm:
			init_levin(S_algorithm, series, transform2);
			break;
		case D_algorithm:
			init_levin(D_algorithm, series, transform2);
			break;
		case chang_epsilon_algorithm:
			transform2.reset(new chang_wynn_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case M_algorithm:
			init_levin(M_algorithm, series, transform2);
			break;
		case weniger_transformation:
			transform2.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case rho_wynn_transformation_id:
			init_wynn(series, transform2);
			break;
		case brezinski_theta_transformation_id:
			transform2.reset(new brezinski_theta_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case epsilon_algorithm_3_id:
			init_epsilon_3(series, transform);
			break;
		case levin_recursion_id:
			init_levin_recursion(series, transform);
			break;
		case W_algorithm_id:
			transform2.reset(new W_lubkin_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case richardson_algorithm_id:
			transform2.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case Ford_Sidi_algorithm_id:
			transform2.reset(new ford_sidi_one_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case Ford_Sidi_algorithm_two_id:
			transform2.reset(new ford_sidi_two_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		default:
			throw std::domain_error("wrong algorithm id");
		}

		cmp_transformations(n, order, std::move(series.get()), std::move(transform.get()), std::move(transform2.get()));
		break;
	}
	case eval_transform_time_id:
		eval_transform_time(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_all_transforms_id: //Testing all functions for series

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart beta_Levin_S_algorithm value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter beta_Levin_S_algorithm: "; std::cin >> beta_Levin_S_algorithm;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else beta_Levin_S_algorithm = 1;

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart gamma_Levin_M_algorithm value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter gamma_Levin_M_algorithm: "; std::cin >> gamma_Levin_M_algorithm;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else gamma_Levin_M_algorithm = 10;

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart gamma_rho_Wynn_algorithm value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter gamma_rho_Wynn_algorithm: "; std::cin >> gamma_rho_Wynn_algorithm;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else gamma_rho_Wynn_algorithm = 2;

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart RHO_rho_Wynn_algorithm value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter RHO_rho_Wynn_algorithm: "; std::cin >> RHO_rho_Wynn_algorithm;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else RHO_rho_Wynn_algorithm = 1;

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart beta_levin_recursion_algorithm value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter beta_levin_recursion_algorithm: "; std::cin >> beta_levin_recursion_algorithm;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else beta_levin_recursion_algorithm = -1.5;

		std::cout << '\n';
		std::cout << "|------------------------------------------|" << '\n';
		std::cout << "| Use standart epsilon_algorithm_3 value? 1<-true or 0<-false : "; std::cin >> standart; std::cout << "|" << '\n';
		std::cout << "|------------------------------------------|" << '\n';

		if (!standart) {
			std::cout << '\n';
			std::cout << "|------------------------------------------|" << '\n';
			std::cout << "| Enter parameter epsilon_algorithm_3: "; std::cin >> epsilon_algorithm_3;
			std::cout << "|------------------------------------------|" << '\n';
		}
		else epsilon_algorithm_3 = T(1e-3);

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
			transform.reset(new epsilon_one_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//epsilon v-2
			transform.reset(new epsilon_two_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//epsilon v-3
			transform.reset(new epsilon_three_algorithm<T, K, decltype(series.get())>(series.get(), epsilon_algorithm_3));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new rho_transform<T, K>{}));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new generilized_transform<T, K>{}, gamma_rho_Wynn_algorithm));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn
			transform.reset(new rho_Wynn_algorithm<T, K, decltype(series.get())>(series.get(), new gamma_rho_transform<T, K>{}, gamma_rho_Wynn_algorithm, RHO_rho_Wynn_algorithm));
			print_transform(i, order, std::move(transform.get()));

			//theta-brezinski
			transform.reset(new brezinski_theta_algorithm<T, K, decltype(series.get())>(series.get()));

			//chang epsilon wynn
			transform.reset(new chang_wynn_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//levin standart
			transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//levin recurcive
			transform.reset(new levin_recursion_algorithm<T, K, decltype(series.get())>(series.get(), beta_levin_recursion_algorithm));
			print_transform(i, order, std::move(transform.get()));

			//levin-sidi S U
			transform.reset(new levin_sidi_S_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}, false, beta_Levin_S_algorithm));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S T
			transform.reset(new levin_sidi_S_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}, false, beta_Levin_S_algorithm));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S D
			transform.reset(new levin_sidi_S_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}, false, beta_Levin_S_algorithm));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S V
			transform.reset(new levin_sidi_S_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform<T, K>{}, false, beta_Levin_S_algorithm));
			print_transform(i, order, std::move(transform.get()));

			//levin-sidi D U
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D T
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D D
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D V
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform<T, K>{}, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M U
			transform.reset(new levin_sidi_M_algorithm<T, K, decltype(series.get())>(series.get(), new u_transform<T, K>{}, gamma_Levin_M_algorithm));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M T
			transform.reset(new levin_sidi_M_algorithm<T, K, decltype(series.get())>(series.get(), new t_transform<T, K>{}, gamma_Levin_M_algorithm));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M D
			transform.reset(new levin_sidi_M_algorithm<T, K, decltype(series.get())>(series.get(), new d_transform<T, K>{}, gamma_Levin_M_algorithm));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M V
			transform.reset(new levin_sidi_M_algorithm<T, K, decltype(series.get())>(series.get(), new v_transform_2<T, K>{}, gamma_Levin_M_algorithm));
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
			transform.reset(new ford_sidi_one_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//Ford-Sidi v-2
			transform.reset(new ford_sidi_two_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			std::cout << '\n';
		}

		break;
	default:
		throw std::domain_error("wrong function_id");
	}
}
