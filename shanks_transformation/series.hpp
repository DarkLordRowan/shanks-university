#pragma once

/** updated
 * @file series.hpp
 * List of series currently avaiable:
 * 1 - exp_series
 * 2 - cos_series
 * 3 - sin_series
 * 4 - cosh_series
 * 5 - sinh_series
 * 6 - bin_series
 * 7 - four_arctan_series
 * 8 - ln1mx_series
 * 9 - mean_sinh_sin_series
 * 10 - exp_squared_erf_series
 * 11 - xmb_Jb_two_series
 * 12 - half_asin_two_x_series
 * 13 - inverse_1mx_series
 * 14 - x_1mx_squared_series
 * 15 - erf_series
 * 16 - m_fact_1mx_mp1_inverse_series
 * 17 - inverse_sqrt_1m4x_series
 * 18 - one_twelfth_3x2_pi2_series
 * 19 - x_twelfth_x2_pi2_series
 * 20 - ln2_series
 * 21 - one_series
 * 22 - minus_one_quarter_series
 * 23 - pi_3_series
 * 24 - pi_4_series
 * 25 - pi_squared_6_minus_one_series
 * 26 - three_minus_pi_series
 * 27 - one_twelfth_series
 * 28 - eighth_pi_m_one_third_series
 * 29 - one_third_pi_squared_m_nine_series
 * 30 - four_ln2_m_3_series
 * 31 - exp_m_cos_x_sinsin_x_series
 * 32 - pi_four_minus_ln2_halfed_series
 * 33 - five_pi_twelve_series
 * 34 - x_two_series
 * 35 - pi_six_min_half_series
 * 36 - x_two_throught_squares_series
 * 37 - minus_one_ned_in_n_series
 * 38 - minus_one_n_fact_n_in_n_series
 * 39 - ln_x_plus_one_x_minus_one_halfed_series
 * 40 - two_arcsin_square_x_halfed_series
 * 41 - pi_squared_twelve_series
 *  42 - pi_cubed_32_series
 *  43 - minus_three_plus_ln3_three_devided_two_plus_two_ln2_series
 *  44 - two_ln2_series
 *  45 - pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series
 *  46 - pi_minus_x_2
 *  47 - half_multi_ln_1div2multi1minuscosx
 *  48 - half_minus_sinx_multi_pi_4
 *  49 - ln_1plussqrt1plusxsquare_minus_ln_2
 *  50 - ln_cosx
 *  51 - ln_sinx_minus_ln_x
 *  52 - pi_8_cosx_square_minus_1_div_3_cosx
 *  53 - sqrt_oneminussqrtoneminusx_div_x
 *  54 - one_minus_sqrt_1minus4x_div_2x
 *  55 - arcsin_x_minus_x_series
 *  56 - pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series
 *  57 - abs_sin_x_minus_2_div_pi_series
 *  58 - pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series
 *  59 - minus_3_div_4_or_x_minus_3_div_4_series
 *  60 - ten_minus_x_series
 *  61 - x_series
 *  62 - minus_x_minus_pi_4_or_minus_pi_4_series
 *  63 - one_div_two_minus_x_multi_three_plus_x_series
 *  64 - Si_x_series
 *  65 - Ci_x_series
 *  66 - Riemann_zeta_func_series
 *  67 - Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series
 *  68 - xsquareplus3_div_xsquareplus2multix_minus_1_series
 *  69 - arcsin_x_series
 *  70 - arctg_x_series
 *  71 - K_x_series
 *  72 - E_x_series
 *  73 - sqrt_1plusx_series
 *  74 - Lambert_W_func_series
 *  75 - Incomplete_Gamma_func_series
 *  76 - Series_with_ln_number1_series
 *  77 - Series_with_ln_number2_series
 *  78 - pi_series
 *  79 - x_min_sqrt_x_series
 *  80 - arctan_x2_series
 *  81 - ln1px4_series
 *  82 - sin_x2_series
 *  83 - arctan_x3_series
 *  84 - arcsin_x2_series
 *  85 - ln1_m_x2_series
 *  86 - artanh_x_series
 *  87 - arcsinh_x_series
 *  88 - cos_x2_series
 *  89 - sinh_x2_series
 *  90 - arctanh_x2_series
 *  91 - cos3xmin1_div_xsqare_series
 *  92 - two_degree_x_series
 *  93 - sqrt_1plusx_min_1_min_x_div_2_series
 *  94 - ln13_min_ln7_div_7_series
 *  95 - Ja_x_series
 *  96 - one_div_sqrt2_sin_xdivsqrt2_series
 *  97 - ln_1plusx_div_1plusx2_series
 *  98 - cos_sqrt_x_series
 *  99 - ln_1_plus_x3_series
 *  100 - x_div_1minx_series
 *  101 - x_div_1minx2_series
 *  102 - gamma_series
 * @brief This file contains series base class, recurrent_series_base class and derived classes of various serieses (e.g. exp(x), ch(x))
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
	x_two_throught_squares_series_id,
	minus_one_ned_in_n_series_id,
	minus_one_n_fact_n_in_n_series_id,
	ln_x_plus_one_x_minus_one_halfed_series_id,
	two_arcsin_square_x_halfed_series_id,
	pi_squared_twelve_series_id,
	pi_cubed_32_series_id,
	minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id,
	two_ln2_series_id,
	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id,
	pi_minus_x_2_series_id,
	half_multi_ln_1div2multi1minuscosx_series_id,
	half_minus_sinx_multi_pi_4_series_id,
	ln_1plussqrt1plusxsquare_minus_ln_2_series_id,
	ln_cosx_series_id,
	ln_sinx_minus_ln_x_series_id,
	pi_8_cosx_square_minus_1_div_3_cosx_series_id,
	sqrt_oneminussqrtoneminusx_div_x_series_id,
	one_minus_sqrt_1minus4x_div_2x_series_id,
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
	Series_with_ln_number1_series_id,
	Series_with_ln_number2_series_id,
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
	ln_1plusx_div_1plusx2_series_id,
	cos_sqrt_x_series_id,
	ln_1_plus_x3_series_id,
	x_div_1minx_series_id,
	x_div_1minx2_series_id,
	gamma_series_id,
};

#include "series/serieses/abs_sin_x_minus_2_div_pi_series.hpp"
#include "series/serieses/arcsin_x2_series.hpp"
#include "series/serieses/arcsin_x_minus_x_series.hpp"
#include "series/serieses/arcsin_x_series.hpp"
#include "series/serieses/arcsinh_x_series.hpp"
#include "series/serieses/arctan_x2_series.hpp"
#include "series/serieses/arctan_x3_series.hpp"
#include "series/serieses/arctanh_x2_series.hpp"
#include "series/serieses/arctg_x_series.hpp"
#include "series/serieses/artanh_x_series.hpp"
#include "series/serieses/bin_series.hpp"
#include "series/serieses/Ci_x_series.hpp"
#include "series/serieses/cos3xmin1_div_xsqare_series.hpp"
#include "series/serieses/cos_series.hpp"
#include "series/serieses/cos_sqrt_x_series.hpp"
#include "series/serieses/cos_x2_series.hpp"
#include "series/serieses/cosh_series.hpp"
#include "series/serieses/E_x_series.hpp"
#include "series/serieses/eighth_pi_m_one_third_series.hpp"
#include "series/serieses/erf_series.hpp"
#include "series/serieses/exp_m_cos_x_sinsin_x_series.hpp"
#include "series/serieses/exp_series.hpp"
#include "series/serieses/exp_squared_erf_series.hpp"
#include "series/serieses/five_pi_twelve_series.hpp"
#include "series/serieses/four_arctan_series.hpp"
#include "series/serieses/four_ln2_m_3_series.hpp"
#include "series/serieses/gamma_series.hpp"
#include "series/serieses/half_asin_two_x_series.hpp"
#include "series/serieses/half_minus_sinx_multi_pi_4_series.hpp"
#include "series/serieses/half_multi_ln_1div2multi1minuscosx_series.hpp"
#include "series/serieses/Incomplete_Gamma_func_series.hpp"
#include "series/serieses/inverse_1mx_series.hpp"
#include "series/serieses/inverse_sqrt_1m4x_series.hpp"
#include "series/serieses/Ja_x_series.hpp"
#include "series/serieses/K_x_series.hpp"
#include "series/serieses/Lambert_W_func_series.hpp"
#include "series/serieses/ln1_m_x2_series.hpp"
#include "series/serieses/ln1mx_series.hpp"
#include "series/serieses/ln1px4_series.hpp"
#include "series/serieses/ln2_series.hpp"
#include "series/serieses/ln13_min_ln7_div_7_series.hpp"
#include "series/serieses/ln_1_plus_x3_series.hpp"
#include "series/serieses/ln_1plussqrt1plusxsquare_minus_ln_2_series.hpp"
#include "series/serieses/ln_1plusx_div_1plusx2_series.hpp"
#include "series/serieses/ln_cosx_series.hpp"
#include "series/serieses/ln_sinx_minus_ln_x_series.hpp"
#include "series/serieses/ln_x_plus_one_x_minus_one_halfed_series.hpp"
#include "series/serieses/m_fact_1mx_mp1_inverse_series.hpp"
#include "series/serieses/mean_sinh_sin_series.hpp"
#include "series/serieses/minus_3_div_4_or_x_minus_3_div_4_series.hpp"
#include "series/serieses/minus_one_n_fact_n_in_n_series.hpp"
#include "series/serieses/minus_one_ned_in_n_series.hpp"
#include "series/serieses/minus_one_quarter_series.hpp"
#include "series/serieses/minus_three_plus_ln3_three_devided_two_plus_two_ln2_series.hpp"
#include "series/serieses/minus_x_minus_pi_4_or_minus_pi_4_series.hpp"
#include "series/serieses/one_div_sqrt2_sin_xdivsqrt2_series.hpp"
#include "series/serieses/one_div_two_minus_x_multi_three_plus_x_series.hpp"
#include "series/serieses/one_minus_sqrt_1minus4x_div_2x_series.hpp"
#include "series/serieses/one_series.hpp"
#include "series/serieses/one_third_pi_squared_m_nine_series.hpp"
#include "series/serieses/one_twelfth_3x2_pi2_series.hpp"
#include "series/serieses/one_twelfth_series.hpp"
#include "series/serieses/pi_3_series.hpp"
#include "series/serieses/pi_4_series.hpp"
#include "series/serieses/pi_8_cosx_square_minus_1_div_3_cosx_series.hpp"
#include "series/serieses/pi_cubed_32_series.hpp"
#include "series/serieses/pi_four_minus_ln2_halfed_series.hpp"
#include "series/serieses/pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series.hpp"
#include "series/serieses/pi_minus_x_2_series.hpp"
#include "series/serieses/pi_series.hpp"
#include "series/serieses/pi_six_min_half_series.hpp"
#include "series/serieses/pi_squared_6_minus_one_series.hpp"
#include "series/serieses/pi_squared_twelve_series.hpp"
#include "series/serieses/pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series.hpp"
#include "series/serieses/pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series.hpp"
#include "series/serieses/recurrent_testing_series.hpp"
#include "series/serieses/Riemann_zeta_func_series.hpp"
#include "series/serieses/Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series.hpp"
#include "series/serieses/Series_with_ln_number1_series.hpp"
#include "series/serieses/Series_with_ln_number2_series.hpp"
#include "series/serieses/Si_x_series.hpp"
#include "series/serieses/sin_series.hpp"
#include "series/serieses/sin_x2_series.hpp"
#include "series/serieses/sinh_series.hpp"
#include "series/serieses/sinh_x2_series.hpp"
#include "series/serieses/sqrt_1plusx_min_1_min_x_div_2_series.hpp"
#include "series/serieses/sqrt_1plusx_series.hpp"
#include "series/serieses/sqrt_oneminussqrtoneminusx_div_x_series.hpp"
#include "series/serieses/ten_minus_x_series.hpp"
#include "series/serieses/testing_series.hpp"
#include "series/serieses/three_minus_pi_series.hpp"
#include "series/serieses/two_arcsin_square_x_halfed_series.hpp"
#include "series/serieses/two_degree_x_series.hpp"
#include "series/serieses/two_ln2_series.hpp"
#include "series/serieses/x_1mx_squared_series.hpp"
#include "series/serieses/x_div_1minx2_series.hpp"
#include "series/serieses/x_div_1minx_series.hpp"
#include "series/serieses/x_min_sqrt_x_series.hpp"
#include "series/serieses/x_series.hpp"
#include "series/serieses/x_twelfth_x2_pi2_series.hpp"
#include "series/serieses/x_two_series.hpp"
#include "series/serieses/x_two_throught_squares_series.hpp"
#include "series/serieses/xmb_Jb_two_series.hpp"
#include "series/serieses/xsquareplus3_div_xsquareplus2multix_minus_1_series.hpp"
