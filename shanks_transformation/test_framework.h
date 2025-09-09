/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 */

#pragma once

#include <memory>
#include <set>

#include "methods.hpp"
#include "series.h"
#include "test_functions.h"

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
	gamma_series_id,
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
	     test_all_transforms_id,
};

/**
 * @brief safely reads and validates unsigned integral input
 * @authors Maximov A.K.
 */

template <std::unsigned_integral K>
K read_input() {
	long long input;
	std::cin >> input;

	if (input < 0)
		throw std::domain_error("Negative value in the input!");

	return static_cast<K>(input); // ���������� ��������������
}

/**
* @brief prints out all available series for testing
* @authors Bolshakov M.P.
* @edited by Kreynin R.G.
*/
inline static void print_series_info()
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
inline static void print_transformation_info()
{
	std::cout << "Which transformation would you like to test?" << '\n' <<
		"List of currently avaiable series:" << '\n' <<
		"1 - Shanks Transformation" << '\n' <<
		"2 - Weniger transformation" << '\n' <<
		"3 - Rho - Wynn transformation" << '\n' <<
		"4 - Theta Brezinski transformation" << '\n' <<
		"5 - Epsilon algorithm" << '\n' <<
		"6 - Epsilon v2 algorithm" << '\n' <<
		"7 - Epsilon v3 algorithm" << '\n' <<
		"8 - Chang epsilon algorithm" << '\n' <<
		"9 - L algorithm" << '\n' <<
		"10 - S algorithm" << '\n' <<
		"11 - D algorithm" << '\n' <<
		"12 - M algorithm" << '\n' <<
		"13 - W algorithm" << '\n' <<
		"14 - Richardson Algorithm" << '\n' <<
		"15 - Ford-Sidi Algorithm V-2" << '\n' <<
		"16 - Ford-Sidi Algorithm V-3" << '\n' <<
		'\n';
}

/**
* @brief prints out all available fungus for testing
* @authors Bolshakov M.P.
* @edited by Kreynin R.G.
*/
inline static void print_test_function_info()
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
* @brief The main testing function
* This function provides a convenient and interactive way to test out the convergence acceleration of various series
* @tparam T The type of the elements in the series, K The type of enumerating integer
* @authors Bolshakov M.P
* @edited by Kreynin R.G.
*/
template <std::floating_point T, std::unsigned_integral K>
inline static void main_testing_function()
{

	//choosing series
	print_series_info();
	std::unique_ptr<series_base<T, K>> series;
	K series_id = read_input<K>();

	//choosing x
	std::cout << "Enter x - the argument for the functional series" << '\n';
	T x = static_cast<T>(0);
	std::cin >> x;

	//choosing series (cont.)
	std::set<K> alternating_series = { 2, 3, 7, 11, 15, 18, 19, 20, 21, 24, 26, 28, 30, 31 };
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
		std::cout << "Enter the value for constant alpha for the series" << '\n';
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
		K b;
		std::cout << "Enter the value for constant b for the series" << '\n';
		b = read_input<K>();
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
		std::cout << "Enter the value for constant m for the series" << '\n';
		m = read_input<K>();
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
		series.reset(new ln2_series<T, K>(x));
		break;
	case series_id_t::one_series_id:
		series.reset(new one_series<T, K>(x));
		break;
	case series_id_t::minus_one_quarter_series_id:
		series.reset(new minus_one_quarter_series<T, K>(x));
		break;
	case series_id_t::pi_3_series_id:
		series.reset(new pi_3_series<T, K>(x));
		break;
	case series_id_t::pi_4_series_id:
		series.reset(new pi_4_series<T, K>(x));
		break;
	case series_id_t::pi_squared_6_minus_one_series_id:
		series.reset(new pi_squared_6_minus_one_series<T, K>(x));
		break;
	case series_id_t::three_minus_pi_series_id:
		series.reset(new three_minus_pi_series<T, K>(x));
		break;
	case series_id_t::one_twelfth_series_id:
		series.reset(new one_twelfth_series<T, K>(x));
		break;
	case series_id_t::eighth_pi_m_one_third_series_id:
		series.reset(new eighth_pi_m_one_third_series<T, K>(x));
		break;
	case series_id_t::one_third_pi_squared_m_nine_series_id:
		series.reset(new one_third_pi_squared_m_nine_series<T, K>(x));
		break;
	case series_id_t::four_ln2_m_3_series_id:
		series.reset(new four_ln2_m_3_series<T, K>(x));
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
	case series_id_t::pi_squared_twelve_series_id:
		series.reset(new pi_squared_twelve_series<T, K>(x));
		break;
	case series_id_t::pi_cubed_32_series_id:
		series.reset(new pi_cubed_32_series<T, K>(x));
		break;
	case series_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id:
		series.reset(new minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>(x));
		break;
	case series_id_t::two_ln2_series_id:
		series.reset(new two_ln2_series<T, K>(x));
		break;
	case series_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id:
		series.reset(new pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>(x));
		break;
	case series_id_t::pi_minus_x_2_id:
		series.reset(new pi_minus_x_2<T, K>(x));
		break;
	case series_id_t::half_multi_ln_1div2multi1minuscosx_id:
		series.reset(new half_multi_ln_1div2multi1minuscosx<T, K>(x));
		break;
	case series_id_t::half_minus_sinx_multi_pi_4_id:
		series.reset(new half_minus_sinx_multi_pi_4<T, K>(x));
		break;
	case series_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_id:
		series.reset(new ln_1plussqrt1plusxsquare_minus_ln_2<T, K>(x));
		break;
	case series_id_t::ln_cosx_id:
		series.reset(new ln_cosx<T, K>(x));
		break;
	case series_id_t::ln_sinx_minus_ln_x_id:
		series.reset(new ln_sinx_minus_ln_x<T, K>(x));
		break;
	case series_id_t::pi_8_cosx_square_minus_1_div_3_cosx_id:
		series.reset(new pi_8_cosx_square_minus_1_div_3_cosx<T, K>(x));
		break;
	case series_id_t::sqrt_oneminussqrtoneminusx_div_x_id:
		series.reset(new sqrt_oneminussqrtoneminusx_div_x<T, K>(x));
		break;
	case series_id_t::one_minus_sqrt_1minus4x_div_2x_id:
		series.reset(new one_minus_sqrt_1minus4x_div_2x<T, K>(x));
		break;
	case series_id_t::arcsin_x_minus_x_series_id:
		series.reset(new arcsin_x_minus_x_series<T, K>(x));
		break;
	case series_id_t::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id:
		series.reset(new pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>(x));
		break;
	case series_id_t::abs_sin_x_minus_2_div_pi_series_id:
		series.reset(new abs_sin_x_minus_2_div_pi_series<T, K>(x));
		break;
	case series_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id:
		series.reset(new pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>(x));
		break;
	case series_id_t::minus_3_div_4_or_x_minus_3_div_4_series_id:
		series.reset(new minus_3_div_4_or_x_minus_3_div_4_series<T, K>(x));
		break;
	case series_id_t::ten_minus_x_series_id:
		series.reset(new ten_minus_x_series<T, K>(x));
		break;
	case series_id_t::x_series_id:
		series.reset(new x_series<T, K>(x));
		break;
	case series_id_t::minus_x_minus_pi_4_or_minus_pi_4_series_id:
		series.reset(new minus_x_minus_pi_4_or_minus_pi_4_series<T, K>(x));
		break;
	case series_id_t::one_div_two_minus_x_multi_three_plus_x_series_id:
		series.reset(new one_div_two_minus_x_multi_three_plus_x_series<T, K>(x));
		break;
	case series_id_t::Si_x_series_id:
		series.reset(new Si_x_series<T, K>(x));
		break;
	case series_id_t::Ci_x_series_id:
		series.reset(new Ci_x_series<T, K>(x));
		break;
	case series_id_t::Riemann_zeta_func_series_id:
		series.reset(new Riemann_zeta_func_series<T, K>(x));
		break;
	case series_id_t::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id:
		series.reset(new Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>(x));
		break;
	case series_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_series_id:
		series.reset(new xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>(x));
		break;
	case series_id_t::arcsin_x_series_id:
		series.reset(new arcsin_x_series<T, K>(x));
		break;
	case series_id_t::arctg_x_series_id:
		series.reset(new arctg_x_series<T, K>(x));
		break;
	case series_id_t::K_x_series_id:
		series.reset(new K_x_series<T, K>(x));
		break;
	case series_id_t::E_x_series_id:
		series.reset(new E_x_series<T, K>(x));
		break;
	case series_id_t::sqrt_1plusx_series_id:
		series.reset(new sqrt_1plusx_series<T, K>(x));
		break;
	case series_id_t::Lambert_W_func_series_id:
		series.reset(new Lambert_W_func_series<T, K>(x));
		break;
	case series_id_t::Incomplete_Gamma_func_series_id:
		T s;
		std::cout << "Enter the value for constant s for the series" << '\n';
		std::cin >> s;
		series.reset(new Incomplete_Gamma_func_series<T, K>(x, s));
		break;
	case series_id_t::Series_with_ln_number1_id:
		series.reset(new Series_with_ln_number1<T, K>(x));
		break;
	case series_id_t::Series_with_ln_number2_id:
		series.reset(new Series_with_ln_number2<T, K>(x));
		break;
	case series_id_t::pi_series_id:
		series.reset(new pi_series<T, K>(x));
		break;
	case series_id_t::x_min_sqrt_x_series_id:
		series.reset(new x_min_sqrt_x_series<T, K>(x));
		break;
	case series_id_t::arctan_x2_series_id:
		series.reset(new arctan_x2_series<T, K>(x));
		break;
	case series_id_t::ln1px4_series_id:
		series.reset(new ln1px4_series<T, K>(x));
		break;
	case series_id_t::sin_x2_series_id:
		series.reset(new sin_x2_series<T, K>(x));
		break;
	case series_id_t::arctan_x3_series_id:
		series.reset(new arctan_x3_series<T, K>(x));
		break;
	case series_id_t::arcsin_x2_series_id:
		series.reset(new arcsin_x2_series<T, K>(x));
		break;
	case series_id_t::ln1_m_x2_series_id:
		series.reset(new ln1_m_x2_series<T, K>(x));
		break;
	case series_id_t::artanh_x_series_id:
		series.reset(new artanh_x_series<T, K>(x));
		break;
	case series_id_t::arcsinh_x_series_id:
		series.reset(new arcsinh_x_series<T, K>(x));
		break;
	case series_id_t::cos_x2_series_id:
		series.reset(new cos_x2_series<T, K>(x));
		break;
	case series_id_t::sinh_x2_series_id:
		series.reset(new sinh_x2_series<T, K>(x));
		break;
	case series_id_t::arctanh_x2_series_id:
		series.reset(new arctanh_x2_series<T, K>(x));
		break;
	case series_id_t::cos3xmin1_div_xsqare_series_id:
		series.reset(new cos3xmin1_div_xsqare_series<T, K>(x));
		break;
	case series_id_t::two_degree_x_series_id:
		series.reset(new two_degree_x_series<T, K>(x));
		break;
	case series_id_t::sqrt_1plusx_min_1_min_x_div_2_series_id:
		series.reset(new sqrt_1plusx_min_1_min_x_div_2_series<T, K>(x));
		break;
	case series_id_t::ln13_min_ln7_div_7_series_id:
		series.reset(new ln13_min_ln7_div_7_series<T, K>(x));
		break;
	case series_id_t::Ja_x_series_id:
		T a;
		std::cout << "Enter the value for constant a for the series" << '\n';
		std::cin >> a;
		series.reset(new Ja_x_series<T, K>(x, a));
		break;
	case series_id_t::one_div_sqrt2_sin_xdivsqrt2_series_id:
		series.reset(new one_div_sqrt2_sin_xdivsqrt2_series<T, K>(x));
		break;
	case series_id_t::ln_1plusx_div_1plusx2_id:
		series.reset(new ln_1plusx_div_1plusx2<T, K>(x));
		break;
	case series_id_t::cos_sqrt_x_id:
		series.reset(new cos_sqrt_x<T, K>(x));
		break;
	case series_id_t::ln_1_plus_x3_id:
		series.reset(new ln_1_plus_x3<T, K>(x));
		break;
	case series_id_t::x_div_1minx_id:
		series.reset(new x_div_1minx<T, K>(x));
		break;
	case series_id_t::x_div_1minx2_id:
		series.reset(new x_div_1minx2<T, K>(x));
		break;
	case series_id_t::gamma_series_id:
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
	K transformation_id = read_input<K>();

	std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform;
	switch (transformation_id)
	{
	case transformation_id_t::shanks_transformation_id:
		if (alternating_series.contains(series_id))
			transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
		else
			transform.reset(new shanks_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::epsilon_algorithm_id:
		transform.reset(new wynn_epsilon_1_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::epsilon_algorithm_2_id:
		transform.reset(new wynn_epsilon_2_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::S_algorithm_id:
		transform.reset(new levin_sidi_s_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::D_algorithm_id:
		transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::chang_epsilon_algorithm_id:
		transform.reset(new chang_wynn_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::M_algorithm_id:
		transform.reset(new levin_sidi_m_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::weniger_transformation_id:
		transform.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::rho_wynn_transformation_id:
		transform.reset(new wynn_rho_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::brezinski_theta_transformation_id:
		transform.reset(new brezinski_theta_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::epsilon_algorithm_3_id:
		transform.reset(new wynn_epsilon_3_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::W_algorithm_id:
		transform.reset(new lubkin_w_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::richardson_algorithm_id:
		transform.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
		break;

	case transformation_id_t::Ford_Sidi_algorithm_two_id:
		transform.reset(new ford_sidi_2_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	case transformation_id_t::Ford_Sidi_algorithm_three_id:
		transform.reset(new ford_sidi_3_algorithm<T, K, decltype(series.get())>(series.get()));
		break;
	default:
		throw std::domain_error("wrong transformation_id");
	}

	//choosing testing function

	T beta_Levin_S_algorithm = T{};			//parameter for LevinType transformations algorithm
	T gamma_Levin_M_algorithm = T{};		//parameter for LevinType transformations algorithm
	T gamma_rho_Wynn_algorithm = T{};		//parameter for gamma modification
	T RHO_rho_Wynn_algorithm = T{};			//parameter for gamma-rho modification
	T beta_levin_recursion_algorithm = T{};	//parameter for levin_recursion algorithm
	T epsilon_algorithm_3 = T{};			//parameter for levin_recursion algorithm

	print_test_function_info();
	K function_id = read_input<K>();
	std::cout << "Enter n and order:" << '\n';
	K n = read_input<K>();
	K order = read_input<K>();

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
		print_transformation_info();
		K cmop_transformation_id = read_input<K>();;

		std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform2;

		switch (cmop_transformation_id)
		{
		case transformation_id_t::shanks_transformation_id:
			if (alternating_series.contains(series_id))
				transform2.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform2.reset(new shanks_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_id:
			transform2.reset(new wynn_epsilon_1_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_2_id:
			transform2.reset(new wynn_epsilon_2_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::S_algorithm_id:
			transform.reset(new levin_sidi_s_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::D_algorithm_id:
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::chang_epsilon_algorithm_id:
			transform2.reset(new chang_wynn_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::M_algorithm_id:
			transform2.reset(new levin_sidi_m_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::weniger_transformation_id:
			transform2.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::rho_wynn_transformation_id:
			transform2.reset(new wynn_rho_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::brezinski_theta_transformation_id:
			transform2.reset(new brezinski_theta_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::epsilon_algorithm_3_id:
			transform.reset(new wynn_epsilon_3_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::W_algorithm_id:
			transform2.reset(new lubkin_w_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::richardson_algorithm_id:
			transform2.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::Ford_Sidi_algorithm_two_id:
			transform2.reset(new ford_sidi_2_algorithm<T, K, decltype(series.get())>(series.get()));
			break;
		case transformation_id_t::Ford_Sidi_algorithm_three_id:
			transform2.reset(new ford_sidi_3_algorithm<T, K, decltype(series.get())>(series.get()));
			break;

		default:
			throw std::domain_error("wrong algorithm id");
		}

		cmp_transformations(n, order, std::move(series.get()), std::move(transform.get()), std::move(transform2.get()));
		break;
	}
	case test_function_id_t::eval_transform_time_id:
		eval_transform_time(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::test_all_transforms_id: //Testing all functions for series
		for (K i = 1; i <= n; i++)
		{
			print_sum(i, std::move(series.get()));

			//shanks
			if (alternating_series.contains(series_id))
				transform.reset(new shanks_transform_alternating<T, K, decltype(series.get())>(series.get()));
			else
				transform.reset(new shanks_algorithm<T, K, decltype(series.get())>(series.get()));

			print_transform(i, order, std::move(transform.get()));

			//epsilon v-1
			transform.reset(new wynn_epsilon_1_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//epsilon v-2
			transform.reset(new wynn_epsilon_2_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//epsilon v-3
			transform.reset(new wynn_epsilon_3_algorithm<T, K, decltype(series.get())>(series.get(), epsilon_algorithm_3));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn classic
			transform.reset(new wynn_rho_algorithm<T, K, decltype(series.get())>(series.get(), numerator_type::rho_variant));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn generalized
			transform.reset(new wynn_rho_algorithm<T, K, decltype(series.get())>(series.get(), numerator_type::generalized_variant));
			print_transform(i, order, std::move(transform.get()));

			//rho-wynn gamma-rho
			transform.reset(new wynn_rho_algorithm<T, K, decltype(series.get())>(series.get(), numerator_type::gamma_rho_variant));
			print_transform(i, order, std::move(transform.get()));

			//theta-brezinski
			transform.reset(new brezinski_theta_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//chang epsilon wynn
			transform.reset(new chang_wynn_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//levin standart
			transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//levin recurcive
			transform.reset(new levin_algorithm<T, K, decltype(series.get())>(series.get(), true));
			print_transform(i, order, std::move(transform.get()));

			//levin-sidi S U
			transform.reset(new levin_sidi_s_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::u_variant, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S T
			transform.reset(new levin_sidi_s_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::t_variant, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S T-WAVE
			transform.reset(new levin_sidi_s_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::t_wave_variant, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi S V
			transform.reset(new levin_sidi_s_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::v_variant, false));
			print_transform(i, order, std::move(transform.get()));

			//levin-sidi D U
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::u_variant, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D T
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::t_variant, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D T-WAVE
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::t_wave_variant, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi D V
			transform.reset(new drummond_d_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::v_variant, false));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M U
			transform.reset(new levin_sidi_m_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::u_variant));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M T
			transform.reset(new levin_sidi_m_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::t_variant));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M T-WAVE
			transform.reset(new levin_sidi_m_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::t_wave_variant));
			print_transform(i, order, std::move(transform.get()));
			//

			//levin-sidi M V-WAVE
			transform.reset(new levin_sidi_m_algorithm<T, K, decltype(series.get())>(series.get(), remainder_type::v_wave_variant));
			print_transform(i, order, std::move(transform.get()));
			//

			//weniger
			transform.reset(new weniger_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//lubkin W
			transform.reset(new lubkin_w_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//Richardson
			transform.reset(new richardson_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//Ford-Sidi v-2
			transform.reset(new ford_sidi_2_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			//Ford-Sidi v-3
			transform.reset(new ford_sidi_3_algorithm<T, K, decltype(series.get())>(series.get()));
			print_transform(i, order, std::move(transform.get()));

			std::cout << '\n';
		}

		break;
	default:
		throw std::domain_error("wrong function_id");
	}
}
