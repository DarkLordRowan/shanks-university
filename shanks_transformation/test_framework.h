/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G., Maximov A.K.
 */

#pragma once

#include <memory>
#include <set>

#ifndef INC_FPRECISION
	#include "libs/arbitrary_arithmetics/fprecision.h"
#endif

#include "methods.hpp"
#include "series.h"
#include "test_functions.h"

 /**
  * @brief Enum of series IDs
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

/**
 * @brief Enum of testing functions IDs
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
 */
template <std::unsigned_integral K>
K read_input() {
	long long input;
	std::cin >> input;

	if (input < 0)
		throw std::domain_error("Negative value in the input!");

	return static_cast<K>(input);
}

/**
 * @brief Interface for series information
 */
class ISeriesInfo {
public:
	virtual ~ISeriesInfo() = default;
	virtual series_id_t getId() const = 0;
	virtual std::string getName() const = 0;
	virtual std::string getDescription() const { return ""; }
};

class ExpSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return exp_series_id; }
	std::string getName() const override { return "exp_series"; }
};

class CosSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return cos_series_id; }
	std::string getName() const override { return "cos_series"; }
};

class SinSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sin_series_id; }
	std::string getName() const override { return "sin_series"; }
};

class CoshSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return cosh_series_id; }
	std::string getName() const override { return "cosh_series"; }
};

class SinhSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sinh_series_id; }
	std::string getName() const override { return "sinh_series"; }
};

class BinSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return bin_series_id; }
	std::string getName() const override { return "bin_series"; }
};

class Four_arctanSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return four_arctan_series_id; }
	std::string getName() const override { return "four_arctan_series"; }
};

class Ln1mxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln1mx_series_id; }
	std::string getName() const override { return "ln1mx_series"; }
};

class Mean_sinh_sinSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return mean_sinh_sin_series_id; }
	std::string getName() const override { return "mean_sinh_sin_series"; }
};

class Exp_squared_erfSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return exp_squared_erf_series_id; }
	std::string getName() const override { return "exp_squared_erf_series"; }
};

class Xmb_Jb_twoSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return xmb_Jb_two_series_id; }
	std::string getName() const override { return "xmb_Jb_two_series"; }
};

class Half_asin_two_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return half_asin_two_x_series_id; }
	std::string getName() const override { return "half_asin_two_x_series"; }
};

class Inverse_1mxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return inverse_1mx_series_id; }
	std::string getName() const override { return "inverse_1mx_series"; }
};

class X_1mx_squaredSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_1mx_squared_series_id; }
	std::string getName() const override { return "x_1mx_squared_series"; }
};

class ErfSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return erf_series_id; }
	std::string getName() const override { return "erf_series"; }
};

class M_fact_1mx_mp1_inverseSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return m_fact_1mx_mp1_inverse_series_id; }
	std::string getName() const override { return "m_fact_1mx_mp1_inverse_series"; }
};

class Inverse_sqrt_1m4xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return inverse_sqrt_1m4x_series_id; }
	std::string getName() const override { return "inverse_sqrt_1m4x_series"; }
};

class One_twelfth_3x2_pi2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return one_twelfth_3x2_pi2_series_id; }
	std::string getName() const override { return "one_twelfth_3x2_pi2_series"; }
};

class X_twelfth_x2_pi2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_twelfth_x2_pi2_series_id; }
	std::string getName() const override { return "x_twelfth_x2_pi2_series"; }
};

class Ln2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln2_series_id; }
	std::string getName() const override { return "ln2_series"; }
};

class OneSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return one_series_id; }
	std::string getName() const override { return "one_series"; }
};

class Minus_one_quarterSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return minus_one_quarter_series_id; }
	std::string getName() const override { return "minus_one_quarter_series"; }
};

class Pi_3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_3_series_id; }
	std::string getName() const override { return "pi_3_series"; }
};

class Pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_4_series_id; }
	std::string getName() const override { return "pi_4_series"; }
};

class Pi_squared_6_minus_oneSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_squared_6_minus_one_series_id; }
	std::string getName() const override { return "pi_squared_6_minus_one_series"; }
};

class Three_minus_piSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return three_minus_pi_series_id; }
	std::string getName() const override { return "three_minus_pi_series"; }
};

class One_twelfthSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return one_twelfth_series_id; }
	std::string getName() const override { return "one_twelfth_series"; }
};

class Eighth_pi_m_one_thirdSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return eighth_pi_m_one_third_series_id; }
	std::string getName() const override { return "eighth_pi_m_one_third_series"; }
};

class One_third_pi_squared_m_nineSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return one_third_pi_squared_m_nine_series_id; }
	std::string getName() const override { return "one_third_pi_squared_m_nine_series"; }
};

class Four_ln2_m_3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return four_ln2_m_3_series_id; }
	std::string getName() const override { return "four_ln2_m_3_series"; }
};

class Exp_m_cos_x_sinsin_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return exp_m_cos_x_sinsin_x_series_id; }
	std::string getName() const override { return "exp_m_cos_x_sinsin_x_series"; }
};

class Pi_four_minus_ln2_halfedSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_four_minus_ln2_halfed_series_id; }
	std::string getName() const override { return "pi_four_minus_ln2_halfed_series"; }
};

class Five_pi_twelveSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return five_pi_twelve_series_id; }
	std::string getName() const override { return "five_pi_twelve_series"; }
};

class X_twoSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_two_series_id; }
	std::string getName() const override { return "x_two_series"; }
};

class Pi_six_min_halfSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_six_min_half_series_id; }
	std::string getName() const override { return "pi_six_min_half_series"; }
};

class X_two_throught_squaresSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_two_throught_squares_series_id; }
	std::string getName() const override { return "x_two_throught_squares_series"; }
};

class Minus_one_ned_in_nSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return minus_one_ned_in_n_series_id; }
	std::string getName() const override { return "minus_one_ned_in_n_series"; }
};

class Minus_one_n_fact_n_in_nSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return minus_one_n_fact_n_in_n_series_id; }
	std::string getName() const override { return "minus_one_n_fact_n_in_n_series"; }
};

class Ln_x_plus_one_x_minus_one_halfedSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln_x_plus_one_x_minus_one_halfed_series_id; }
	std::string getName() const override { return "ln_x_plus_one_x_minus_one_halfed_series"; }
};

class Two_arcsin_square_x_halfedSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return two_arcsin_square_x_halfed_series_id; }
	std::string getName() const override { return "two_arcsin_square_x_halfed_series"; }
};

class Pi_squared_twelveSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_squared_twelve_series_id; }
	std::string getName() const override { return "pi_squared_twelve_series"; }
};

class Pi_cubed_32SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_cubed_32_series_id; }
	std::string getName() const override { return "pi_cubed_32_series"; }
};

class Minus_three_plus_ln3_three_devided_two_plus_two_ln2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id; }
	std::string getName() const override { return "minus_three_plus_ln3_three_devided_two_plus_two_ln2_series"; }
};

class Two_ln2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return two_ln2_series_id; }
	std::string getName() const override { return "two_ln2_series"; }
};

class Pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_oneSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id; }
	std::string getName() const override { return "pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series"; }
};

class Pi_minus_x_2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_minus_x_2_series_id; }
	std::string getName() const override { return "pi_minus_x_2_series"; }
};

class Half_multi_ln_1div2multi1minuscosxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return half_multi_ln_1div2multi1minuscosx_series_id; }
	std::string getName() const override { return "half_multi_ln_1div2multi1minuscosx_series"; }
};

class Half_minus_sinx_multi_pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return half_minus_sinx_multi_pi_4_series_id; }
	std::string getName() const override { return "half_minus_sinx_multi_pi_4_series"; }
};

class Ln_1plussqrt1plusxsquare_minus_ln_2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln_1plussqrt1plusxsquare_minus_ln_2_series_id; }
	std::string getName() const override { return "ln_1plussqrt1plusxsquare_minus_ln_2_series"; }
};

class Ln_cosxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln_cosx_series_id; }
	std::string getName() const override { return "ln_cosx_series"; }
};

class Ln_sinx_minus_ln_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln_sinx_minus_ln_x_series_id; }
	std::string getName() const override { return "ln_sinx_minus_ln_x_series"; }
};

class Pi_8_cosx_square_minus_1_div_3_cosxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_8_cosx_square_minus_1_div_3_cosx_series_id; }
	std::string getName() const override { return "pi_8_cosx_square_minus_1_div_3_cosx_series"; }
};

class Sqrt_oneminussqrtoneminusx_div_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sqrt_oneminussqrtoneminusx_div_x_series_id; }
	std::string getName() const override { return "sqrt_oneminussqrtoneminusx_div_x_series"; }
};

class One_minus_sqrt_1minus4x_div_2xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return one_minus_sqrt_1minus4x_div_2x_series_id; }
	std::string getName() const override { return "one_minus_sqrt_1minus4x_div_2x_series"; }
};

class Arcsin_x_minus_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arcsin_x_minus_x_series_id; }
	std::string getName() const override { return "arcsin_x_minus_x_series"; }
};

class Pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_squareSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id; }
	std::string getName() const override { return "pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series"; }
};

class Abs_sin_x_minus_2_div_piSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return abs_sin_x_minus_2_div_pi_series_id; }
	std::string getName() const override { return "abs_sin_x_minus_2_div_pi_series"; }
};

class Pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id; }
	std::string getName() const override { return "pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series"; }
};

class Minus_3_div_4_or_x_minus_3_div_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return minus_3_div_4_or_x_minus_3_div_4_series_id; }
	std::string getName() const override { return "minus_3_div_4_or_x_minus_3_div_4_series"; }
};

class Ten_minus_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ten_minus_x_series_id; }
	std::string getName() const override { return "ten_minus_x_series"; }
};

class XSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_series_id; }
	std::string getName() const override { return "x_series"; }
};

class Minus_x_minus_pi_4_or_minus_pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return minus_x_minus_pi_4_or_minus_pi_4_series_id; }
	std::string getName() const override { return "minus_x_minus_pi_4_or_minus_pi_4_series"; }
};

class One_div_two_minus_x_multi_three_plus_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return one_div_two_minus_x_multi_three_plus_x_series_id; }
	std::string getName() const override { return "one_div_two_minus_x_multi_three_plus_x_series"; }
};

class Si_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Si_x_series_id; }
	std::string getName() const override { return "Si_x_series"; }
};

class Ci_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Ci_x_series_id; }
	std::string getName() const override { return "Ci_x_series"; }
};

class Riemann_zeta_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Riemann_zeta_func_series_id; }
	std::string getName() const override { return "Riemann_zeta_func_series"; }
};

class Riemann_zeta_func_xmin1_div_Riemann_zeta_func_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id; }
	std::string getName() const override { return "Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series"; }
};

class Xsquareplus3_div_xsquareplus2multix_minus_1SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return xsquareplus3_div_xsquareplus2multix_minus_1_series_id; }
	std::string getName() const override { return "xsquareplus3_div_xsquareplus2multix_minus_1_series"; }
};

class Arcsin_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arcsin_x_series_id; }
	std::string getName() const override { return "arcsin_x_series"; }
};

class Arctg_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arctg_x_series_id; }
	std::string getName() const override { return "arctg_x_series"; }
};

class K_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return K_x_series_id; }
	std::string getName() const override { return "K_x_series"; }
};

class E_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return E_x_series_id; }
	std::string getName() const override { return "E_x_series"; }
};

class Sqrt_1plusxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sqrt_1plusx_series_id; }
	std::string getName() const override { return "sqrt_1plusx_series"; }
};

class Lambert_W_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Lambert_W_func_series_id; }
	std::string getName() const override { return "Lambert_W_func_series"; }
};

class Incomplete_Gamma_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Incomplete_Gamma_func_series_id; }
	std::string getName() const override { return "Incomplete_Gamma_func_series"; }
};

class Series_with_ln_number1SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Series_with_ln_number1_series_id; }
	std::string getName() const override { return "Series_with_ln_number1_series"; }
};

class Series_with_ln_number2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Series_with_ln_number2_series_id; }
	std::string getName() const override { return "Series_with_ln_number2_series"; }
};

class PiSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return pi_series_id; }
	std::string getName() const override { return "pi_series"; }
};

class X_min_sqrt_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_min_sqrt_x_series_id; }
	std::string getName() const override { return "x_min_sqrt_x_series"; }
};

class Arctan_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arctan_x2_series_id; }
	std::string getName() const override { return "arctan_x2_series"; }
};

class Ln1px4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln1px4_series_id; }
	std::string getName() const override { return "ln1px4_series"; }
};

class Sin_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sin_x2_series_id; }
	std::string getName() const override { return "sin_x2_series"; }
};

class Arctan_x3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arctan_x3_series_id; }
	std::string getName() const override { return "arctan_x3_series"; }
};

class Arcsin_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arcsin_x2_series_id; }
	std::string getName() const override { return "arcsin_x2_series"; }
};

class Ln1_m_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln1_m_x2_series_id; }
	std::string getName() const override { return "ln1_m_x2_series"; }
};

class Artanh_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return artanh_x_series_id; }
	std::string getName() const override { return "artanh_x_series"; }
};

class Arcsinh_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arcsinh_x_series_id; }
	std::string getName() const override { return "arcsinh_x_series"; }
};

class Cos_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return cos_x2_series_id; }
	std::string getName() const override { return "cos_x2_series"; }
};

class Sinh_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sinh_x2_series_id; }
	std::string getName() const override { return "sinh_x2_series"; }
};

class Arctanh_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arctanh_x2_series_id; }
	std::string getName() const override { return "arctanh_x2_series"; }
};

class Cos3xmin1_div_xsqareSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return cos3xmin1_div_xsqare_series_id; }
	std::string getName() const override { return "cos3xmin1_div_xsqare_series"; }
};

class Two_degree_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return two_degree_x_series_id; }
	std::string getName() const override { return "two_degree_x_series"; }
};

class Sqrt_1plusx_min_1_min_x_div_2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sqrt_1plusx_min_1_min_x_div_2_series_id; }
	std::string getName() const override { return "sqrt_1plusx_min_1_min_x_div_2_series"; }
};

class Ln13_min_ln7_div_7SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln13_min_ln7_div_7_series_id; }
	std::string getName() const override { return "ln13_min_ln7_div_7_series"; }
};

class Ja_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return Ja_x_series_id; }
	std::string getName() const override { return "Ja_x_series"; }
};

class One_div_sqrt2_sin_xdivsqrt2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return one_div_sqrt2_sin_xdivsqrt2_series_id; }
	std::string getName() const override { return "one_div_sqrt2_sin_xdivsqrt2_series"; }
};

class Ln_1plusx_div_1plusx2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln_1plusx_div_1plusx2_series_id; }
	std::string getName() const override { return "ln_1plusx_div_1plusx2_series"; }
};

class Cos_sqrt_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return cos_sqrt_x_series_id; }
	std::string getName() const override { return "cos_sqrt_x_series"; }
};

class Ln_1_plus_x3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ln_1_plus_x3_series_id; }
	std::string getName() const override { return "ln_1_plus_x3_series"; }
};

class X_div_1minxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_div_1minx_series_id; }
	std::string getName() const override { return "x_div_1minx_series"; }
};

class X_div_1minx2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return x_div_1minx2_series_id; }
	std::string getName() const override { return "x_div_1minx2_series"; }
};

class GammaSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return gamma_series_id; }
	std::string getName() const override { return "gamma_series"; }
};


/**
 * @brief Interface for transformation information
 */
class ITransformationInfo {
public:
	virtual ~ITransformationInfo() = default;
	virtual transformation_id_t getId() const = 0;
	virtual std::string getName() const = 0;
};

/**
 * brief Implementations for all transformations
 */
class ShanksTransformationInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return shanks_transformation_id; }
	std::string getName() const override { return "Shanks Transformation"; }
};

class WenigerTransformationInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return weniger_transformation_id; }
	std::string getName() const override { return "Weniger transformation"; }
};

class RhoWynnTransformationInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return rho_wynn_transformation_id; }
	std::string getName() const override { return "Rho-Wynn transformation"; }
};

class BrezinskiThetaTransformationInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return brezinski_theta_transformation_id; }
	std::string getName() const override { return "Brezinski Theta transformation"; }
};

class EpsilonAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return epsilon_algorithm_id; }
	std::string getName() const override { return "Epsilon algorithm"; }
};

class EpsilonAlgorithm2Info : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return epsilon_algorithm_2_id; }
	std::string getName() const override { return "Epsilon v2 algorithm"; }
};

class EpsilonAlgorithm3Info : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return epsilon_algorithm_3_id; }
	std::string getName() const override { return "Epsilon v3 algorithm"; }
};

class ChangEpsilonAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return chang_epsilon_algorithm_id; }
	std::string getName() const override { return "Chang epsilon algorithm"; }
};

class LAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return L_algorithm_id; }
	std::string getName() const override { return "L algorithm"; }
};

class SAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return S_algorithm_id; }
	std::string getName() const override { return "S algorithm"; }
};

class DAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return D_algorithm_id; }
	std::string getName() const override { return "D algorithm"; }
};

class MAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return M_algorithm_id; }
	std::string getName() const override { return "M algorithm"; }
};

class WAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return W_algorithm_id; }
	std::string getName() const override { return "W algorithm"; }
};

class RichardsonAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return richardson_algorithm_id; }
	std::string getName() const override { return "Richardson Algorithm"; }
};

class FordSidiAlgorithmTwoInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return Ford_Sidi_algorithm_two_id; }
	std::string getName() const override { return "Ford-Sidi Algorithm V-2"; }
};

class FordSidiAlgorithmThreeInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return Ford_Sidi_algorithm_three_id; }
	std::string getName() const override { return "Ford-Sidi Algorithm V-3"; }
};


/**
 * @brief Interface for test function information
 */
class ITestFunctionInfo {
public:
	virtual ~ITestFunctionInfo() = default;
	virtual test_function_id_t getId() const = 0;
	virtual std::string getName() const = 0;
	virtual std::string getDescription() const = 0;
};

/**
 * @brief Implementations for all test functions
 */
class CmpSumAndTransformInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override { return cmp_sum_and_transform_id; }
	std::string getName() const override { return "cmp_sum_and_transform"; }
	std::string getDescription() const override {
		return "showcases the difference between the transformed partial sum and the nontransformed one";
	}
};

class CmpANAndTransformInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override { return cmp_a_n_and_transform_id; }
	std::string getName() const override { return "cmp_a_n_and_transform"; }
	std::string getDescription() const override {
		return "showcases the difference between series' terms and transformed ones";
	}
};

class TransformationRemainderInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override { return transformation_remainder_id; }
	std::string getName() const override { return "transformation_remainder"; }
	std::string getDescription() const override {
		return "showcases the difference between series' sum and transformed partial sum";
	}
};

class CmpTransformationsInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override { return cmp_transformations_id; }
	std::string getName() const override { return "cmp_transformations"; }
	std::string getDescription() const override {
		return "showcases the difference between convergence of sums accelerated by different transformations";
	}
};

class EvalTransformTimeInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override { return eval_transform_time_id; }
	std::string getName() const override { return "eval_transform_time"; }
	std::string getDescription() const override {
		return "evaluates the time it takes to transform series";
	}
};

class TestAllTransformsInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override { return test_all_transforms_id; }
	std::string getName() const override { return "test_all_transforms"; }
	std::string getDescription() const override {
		return "test all algorithms on sum";
	}
};

/**
 * @brief Factory functions to create all available items
 */
inline static std::vector<std::unique_ptr<ISeriesInfo>> create_series_info() {
	std::unique_ptr<ISeriesInfo> temp[] = {
		std::make_unique<ExpSeriesInfo>(),
		std::make_unique<CosSeriesInfo>(),
		std::make_unique<SinSeriesInfo>(),
		std::make_unique<CoshSeriesInfo>(),
		std::make_unique<SinhSeriesInfo>(),
		std::make_unique<BinSeriesInfo>(),
		std::make_unique<Four_arctanSeriesInfo>(),
		std::make_unique<Ln1mxSeriesInfo>(),
		std::make_unique<Mean_sinh_sinSeriesInfo>(),
		std::make_unique<Exp_squared_erfSeriesInfo>(),
		std::make_unique<Xmb_Jb_twoSeriesInfo>(),
		std::make_unique<Half_asin_two_xSeriesInfo>(),
		std::make_unique<Inverse_1mxSeriesInfo>(),
		std::make_unique<X_1mx_squaredSeriesInfo>(),
		std::make_unique<ErfSeriesInfo>(),
		std::make_unique<M_fact_1mx_mp1_inverseSeriesInfo>(),
		std::make_unique<Inverse_sqrt_1m4xSeriesInfo>(),
		std::make_unique<One_twelfth_3x2_pi2SeriesInfo>(),
		std::make_unique<X_twelfth_x2_pi2SeriesInfo>(),
		std::make_unique<Ln2SeriesInfo>(),
		std::make_unique<OneSeriesInfo>(),
		std::make_unique<Minus_one_quarterSeriesInfo>(),
		std::make_unique<Pi_3SeriesInfo>(),
		std::make_unique<Pi_4SeriesInfo>(),
		std::make_unique<Pi_squared_6_minus_oneSeriesInfo>(),
		std::make_unique<Three_minus_piSeriesInfo>(),
		std::make_unique<One_twelfthSeriesInfo>(),
		std::make_unique<Eighth_pi_m_one_thirdSeriesInfo>(),
		std::make_unique<One_third_pi_squared_m_nineSeriesInfo>(),
		std::make_unique<Four_ln2_m_3SeriesInfo>(),
		std::make_unique<Exp_m_cos_x_sinsin_xSeriesInfo>(),
		std::make_unique<Pi_four_minus_ln2_halfedSeriesInfo>(),
		std::make_unique<Five_pi_twelveSeriesInfo>(),
		std::make_unique<X_twoSeriesInfo>(),
		std::make_unique<Pi_six_min_halfSeriesInfo>(),
		std::make_unique<X_two_throught_squaresSeriesInfo>(),
		std::make_unique<Minus_one_ned_in_nSeriesInfo>(),
		std::make_unique<Minus_one_n_fact_n_in_nSeriesInfo>(),
		std::make_unique<Ln_x_plus_one_x_minus_one_halfedSeriesInfo>(),
		std::make_unique<Two_arcsin_square_x_halfedSeriesInfo>(),
		std::make_unique<Pi_squared_twelveSeriesInfo>(),
		std::make_unique<Pi_cubed_32SeriesInfo>(),
		std::make_unique<Minus_three_plus_ln3_three_devided_two_plus_two_ln2SeriesInfo>(),
		std::make_unique<Two_ln2SeriesInfo>(),
		std::make_unique<Pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_oneSeriesInfo>(),
		std::make_unique<Pi_minus_x_2SeriesInfo>(),
		std::make_unique<Half_multi_ln_1div2multi1minuscosxSeriesInfo>(),
		std::make_unique<Half_minus_sinx_multi_pi_4SeriesInfo>(),
		std::make_unique<Ln_1plussqrt1plusxsquare_minus_ln_2SeriesInfo>(),
		std::make_unique<Ln_cosxSeriesInfo>(),
		std::make_unique<Ln_sinx_minus_ln_xSeriesInfo>(),
		std::make_unique<Pi_8_cosx_square_minus_1_div_3_cosxSeriesInfo>(),
		std::make_unique<Sqrt_oneminussqrtoneminusx_div_xSeriesInfo>(),
		std::make_unique<One_minus_sqrt_1minus4x_div_2xSeriesInfo>(),
		std::make_unique<Arcsin_x_minus_xSeriesInfo>(),
		std::make_unique<Pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_squareSeriesInfo>(),
		std::make_unique<Abs_sin_x_minus_2_div_piSeriesInfo>(),
		std::make_unique<Pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4SeriesInfo>(),
		std::make_unique<Minus_3_div_4_or_x_minus_3_div_4SeriesInfo>(),
		std::make_unique<Ten_minus_xSeriesInfo>(),
		std::make_unique<XSeriesInfo>(),
		std::make_unique<Minus_x_minus_pi_4_or_minus_pi_4SeriesInfo>(),
		std::make_unique<One_div_two_minus_x_multi_three_plus_xSeriesInfo>(),
		std::make_unique<Si_xSeriesInfo>(),
		std::make_unique<Ci_xSeriesInfo>(),
		std::make_unique<Riemann_zeta_funcSeriesInfo>(),
		std::make_unique<Riemann_zeta_func_xmin1_div_Riemann_zeta_func_xSeriesInfo>(),
		std::make_unique<Xsquareplus3_div_xsquareplus2multix_minus_1SeriesInfo>(),
		std::make_unique<Arcsin_xSeriesInfo>(),
		std::make_unique<Arctg_xSeriesInfo>(),
		std::make_unique<K_xSeriesInfo>(),
		std::make_unique<E_xSeriesInfo>(),
		std::make_unique<Sqrt_1plusxSeriesInfo>(),
		std::make_unique<Lambert_W_funcSeriesInfo>(),
		std::make_unique<Incomplete_Gamma_funcSeriesInfo>(),
		std::make_unique<Series_with_ln_number1SeriesInfo>(),
		std::make_unique<Series_with_ln_number2SeriesInfo>(),
		std::make_unique<PiSeriesInfo>(),
		std::make_unique<X_min_sqrt_xSeriesInfo>(),
		std::make_unique<Arctan_x2SeriesInfo>(),
		std::make_unique<Ln1px4SeriesInfo>(),
		std::make_unique<Sin_x2SeriesInfo>(),
		std::make_unique<Arctan_x3SeriesInfo>(),
		std::make_unique<Arcsin_x2SeriesInfo>(),
		std::make_unique<Ln1_m_x2SeriesInfo>(),
		std::make_unique<Artanh_xSeriesInfo>(),
		std::make_unique<Arcsinh_xSeriesInfo>(),
		std::make_unique<Cos_x2SeriesInfo>(),
		std::make_unique<Sinh_x2SeriesInfo>(),
		std::make_unique<Arctanh_x2SeriesInfo>(),
		std::make_unique<Cos3xmin1_div_xsqareSeriesInfo>(),
		std::make_unique<Two_degree_xSeriesInfo>(),
		std::make_unique<Sqrt_1plusx_min_1_min_x_div_2SeriesInfo>(),
		std::make_unique<Ln13_min_ln7_div_7SeriesInfo>(),
		std::make_unique<Ja_xSeriesInfo>(),
		std::make_unique<One_div_sqrt2_sin_xdivsqrt2SeriesInfo>(),
		std::make_unique<Ln_1plusx_div_1plusx2SeriesInfo>(),
		std::make_unique<Cos_sqrt_xSeriesInfo>(),
		std::make_unique<Ln_1_plus_x3SeriesInfo>(),
		std::make_unique<X_div_1minxSeriesInfo>(),
		std::make_unique<X_div_1minx2SeriesInfo>(),
		std::make_unique<GammaSeriesInfo>()
	};

	return std::vector<std::unique_ptr<ISeriesInfo>>(
		std::make_move_iterator(std::begin(temp)),
		std::make_move_iterator(std::end(temp))
	);
}

inline static std::vector<std::unique_ptr<ITransformationInfo>> create_transformation_info() {
	std::unique_ptr<ITransformationInfo> temp[] = {
		std::make_unique<ShanksTransformationInfo>(),
		std::make_unique<WenigerTransformationInfo>(),
		std::make_unique<RhoWynnTransformationInfo>(),
		std::make_unique<BrezinskiThetaTransformationInfo>(),
		std::make_unique<EpsilonAlgorithmInfo>(),
		std::make_unique<EpsilonAlgorithm2Info>(),
		std::make_unique<EpsilonAlgorithm3Info>(),
		std::make_unique<ChangEpsilonAlgorithmInfo>(),
		std::make_unique<LAlgorithmInfo>(),
		std::make_unique<SAlgorithmInfo>(),
		std::make_unique<DAlgorithmInfo>(),
		std::make_unique<MAlgorithmInfo>(),
		std::make_unique<WAlgorithmInfo>(),
		std::make_unique<RichardsonAlgorithmInfo>(),
		std::make_unique<FordSidiAlgorithmTwoInfo>(),
		std::make_unique<FordSidiAlgorithmThreeInfo>()
	};

	return std::vector<std::unique_ptr<ITransformationInfo>>(
		std::make_move_iterator(std::begin(temp)),
		std::make_move_iterator(std::end(temp))
	);
}

inline static std::vector<std::unique_ptr<ITestFunctionInfo>> create_test_function_info() {
	std::unique_ptr<ITestFunctionInfo> temp[] = {
		std::make_unique<CmpSumAndTransformInfo>(),
		std::make_unique<CmpANAndTransformInfo>(),
		std::make_unique<TransformationRemainderInfo>(),
		std::make_unique<CmpTransformationsInfo>(),
		std::make_unique<EvalTransformTimeInfo>(),
		std::make_unique<TestAllTransformsInfo>()
	};

	return std::vector<std::unique_ptr<ITestFunctionInfo>>(
		std::make_move_iterator(std::begin(temp)),
		std::make_move_iterator(std::end(temp))
	);
}

/**
 * @brief prints out all available series for testing
 */
inline static void print_series_info() {
	auto all_series = create_series_info();

	std::cout <<
		"Which series' convergence would you like to accelerate?" << '\n' <<
		"List of currently available series:" << '\n';

	for (size_t i = 0; i < all_series.size(); i++) {
		std::cout << i + 1 << " - " << all_series[i]->getName() << '\n';
	}
	std::cout << '\n';
}

/**
 * @brief prints out all available transformations for testing
 */
inline static void print_transformation_info() {
	auto all_transformations = create_transformation_info();

	std::cout <<
		"Which transformation would you like to test?" << '\n' <<
		"List of currently available transformations:" << '\n';

	for (size_t i = 0; i < all_transformations.size(); i++) {
		std::cout << i + 1 << " - " << all_transformations[i]->getName() << '\n';
	}
	std::cout << '\n';
}

/**
 * @brief prints out all available functions for testing
 */
inline static void print_test_function_info() {
	auto all_functions = create_test_function_info();

	std::cout <<
		"Which function would you like to use for testing?" << '\n' <<
		"List of currently available functions:" << '\n';

	for (size_t i = 0; i < all_functions.size(); i++) {
		std::cout <<
			i + 1 << " - " << all_functions[i]->getName() <<
			" - " << all_functions[i]->getDescription() << '\n';
	}
	std::cout << '\n';
}

/**
 * @brief Helper function to get series by ID
 */
template <FloatLike T, std::unsigned_integral K>
inline static std::unique_ptr<series::series_base<T, K>> create_series_by_id(series_id_t id, T x) {
	// This function replace huge switch-case
	auto all_series = create_series_info();
	if (static_cast<int>(id) < 1 || static_cast<int>(id) > all_series.size()) {
		throw std::domain_error("Invalid series ID");
	}

	// Здесь нужно будет сделать mapping от ID к конструктору
	// Пока оставляю старый switch, но можно оптимизировать дальше
	switch (id) {
	case exp_series_id:
		return std::make_unique<series::exp_series<T, K>>(x);
	case cos_series_id:
		return std::make_unique<series::cos_series<T, K>>(x);
	case sin_series_id:
		return std::make_unique<series::sin_series<T, K>>(x);
	case cosh_series_id:
		return std::make_unique<series::cosh_series<T, K>>(x);
	case sinh_series_id:
		return std::make_unique<series::sinh_series<T, K>>(x);
	case bin_series_id: {
		T alpha;
		std::cout << "Enter the value for constant alpha for the bin series: ";
		std::cin >> alpha;
		return std::make_unique<series::bin_series<T, K>>(x, alpha);
	}
	case four_arctan_series_id:
		return std::make_unique<series::four_arctan_series<T, K>>(x);
	case ln1mx_series_id:
		return std::make_unique<series::ln1mx_series<T, K>>(x);
	case mean_sinh_sin_series_id:
		return std::make_unique<series::mean_sinh_sin_series<T, K>>(x);
	case exp_squared_erf_series_id:
		return std::make_unique<series::exp_squared_erf_series<T, K>>(x);
	case xmb_Jb_two_series_id: {
		K b;
		std::cout << "Enter the value for constant b for the xmb_Jb_two series: ";
		b = read_input<K>();
		return std::make_unique<series::xmb_Jb_two_series<T, K>>(x, b);
	}
	case half_asin_two_x_series_id:
		return std::make_unique<series::half_asin_two_x_series<T, K>>(x);
	case inverse_1mx_series_id:
		return std::make_unique<series::inverse_1mx_series<T, K>>(x);
	case x_1mx_squared_series_id:
		return std::make_unique<series::x_1mx_squared_series<T, K>>(x);
	case erf_series_id:
		return std::make_unique<series::erf_series<T, K>>(x);
	case m_fact_1mx_mp1_inverse_series_id: {
		K m;
		std::cout << "Enter the value for constant m for the m_fact series: ";
		m = read_input<K>();
		return std::make_unique<series::m_fact_1mx_mp1_inverse_series<T, K>>(x, m);
	}
	case inverse_sqrt_1m4x_series_id:
		return std::make_unique<series::inverse_sqrt_1m4x_series<T, K>>(x);
	case one_twelfth_3x2_pi2_series_id:
		return std::make_unique<series::one_twelfth_3x2_pi2_series<T, K>>(x);
	case x_twelfth_x2_pi2_series_id:
		return std::make_unique<series::x_twelfth_x2_pi2_series<T, K>>(x);
	case ln2_series_id:
		return std::make_unique<series::ln2_series<T, K>>(x);
	case one_series_id:
		return std::make_unique<series::one_series<T, K>>(x);
	case minus_one_quarter_series_id:
		return std::make_unique<series::minus_one_quarter_series<T, K>>(x);
	case pi_3_series_id:
		return std::make_unique<series::pi_3_series<T, K>>(x);
	case pi_4_series_id:
		return std::make_unique<series::pi_4_series<T, K>>(x);
	case pi_squared_6_minus_one_series_id:
		return std::make_unique<series::pi_squared_6_minus_one_series<T, K>>(x);
	case three_minus_pi_series_id:
		return std::make_unique<series::three_minus_pi_series<T, K>>(x);
	case one_twelfth_series_id:
		return std::make_unique<series::one_twelfth_series<T, K>>(x);
	case eighth_pi_m_one_third_series_id:
		return std::make_unique<series::eighth_pi_m_one_third_series<T, K>>(x);
	case one_third_pi_squared_m_nine_series_id:
		return std::make_unique<series::one_third_pi_squared_m_nine_series<T, K>>(x);
	case four_ln2_m_3_series_id:
		return std::make_unique<series::four_ln2_m_3_series<T, K>>(x);
	case exp_m_cos_x_sinsin_x_series_id:
		return std::make_unique<series::exp_m_cos_x_sinsin_x_series<T, K>>(x);
	case pi_four_minus_ln2_halfed_series_id:
		return std::make_unique<series::pi_four_minus_ln2_halfed_series<T, K>>(x);
	case five_pi_twelve_series_id:
		return std::make_unique<series::five_pi_twelve_series<T, K>>(x);
	case x_two_series_id:
		return std::make_unique<series::x_two_series<T, K>>(x);
	case pi_six_min_half_series_id:
		return std::make_unique<series::pi_six_min_half_series<T, K>>(x);
	case x_two_throught_squares_series_id:
		return std::make_unique<series::x_two_throught_squares_series<T, K>>(x);
	case minus_one_ned_in_n_series_id:
		return std::make_unique<series::minus_one_ned_in_n_series<T, K>>(x);
	case minus_one_n_fact_n_in_n_series_id:
		return std::make_unique<series::minus_one_n_fact_n_in_n_series<T, K>>(x);
	case ln_x_plus_one_x_minus_one_halfed_series_id:
		return std::make_unique<series::ln_x_plus_one_x_minus_one_halfed_series<T, K>>(x);
	case two_arcsin_square_x_halfed_series_id:
		return std::make_unique<series::two_arcsin_square_x_halfed_series<T, K>>(x);
	case pi_squared_twelve_series_id:
		return std::make_unique<series::pi_squared_twelve_series<T, K>>(x);
	case pi_cubed_32_series_id:
		return std::make_unique<series::pi_cubed_32_series<T, K>>(x);
	case minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id:
		return std::make_unique<series::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>>(x);
	case two_ln2_series_id:
		return std::make_unique<series::two_ln2_series<T, K>>(x);
	case pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series_id:
		return std::make_unique<series::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>>(x);
	case pi_minus_x_2_series_id:
		return std::make_unique<series::pi_minus_x_2_series<T, K>>(x);
	case half_multi_ln_1div2multi1minuscosx_series_id:
		return std::make_unique<series::half_multi_ln_1div2multi1minuscosx_series<T, K>>(x);
	case half_minus_sinx_multi_pi_4_series_id:
		return std::make_unique<series::half_minus_sinx_multi_pi_4_series<T, K>>(x);
	case ln_1plussqrt1plusxsquare_minus_ln_2_series_id:
		return std::make_unique<series::ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>>(x);
	case ln_cosx_series_id:
		return std::make_unique<series::ln_cosx_series<T, K>>(x);
	case ln_sinx_minus_ln_x_series_id:
		return std::make_unique<series::ln_sinx_minus_ln_x_series<T, K>>(x);
	case pi_8_cosx_square_minus_1_div_3_cosx_series_id:
		return std::make_unique<series::pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>>(x);
	case sqrt_oneminussqrtoneminusx_div_x_series_id:
		return std::make_unique<series::sqrt_oneminussqrtoneminusx_div_x_series<T, K>>(x);
	case one_minus_sqrt_1minus4x_div_2x_series_id:
		return std::make_unique<series::one_minus_sqrt_1minus4x_div_2x_series<T, K>>(x);
	case arcsin_x_minus_x_series_id:
		return std::make_unique<series::arcsin_x_minus_x_series<T, K>>(x);
	case pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series_id:
		return std::make_unique<series::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>>(x);
	case abs_sin_x_minus_2_div_pi_series_id:
		return std::make_unique<series::abs_sin_x_minus_2_div_pi_series<T, K>>(x);
	case pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id:
		return std::make_unique<series::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>>(x);
	case minus_3_div_4_or_x_minus_3_div_4_series_id:
		return std::make_unique<series::minus_3_div_4_or_x_minus_3_div_4_series<T, K>>(x);
	case ten_minus_x_series_id:
		return std::make_unique<series::ten_minus_x_series<T, K>>(x);
	case x_series_id:
		return std::make_unique<series::x_series<T, K>>(x);
	case minus_x_minus_pi_4_or_minus_pi_4_series_id:
		return std::make_unique<series::minus_x_minus_pi_4_or_minus_pi_4_series<T, K>>(x);
	case one_div_two_minus_x_multi_three_plus_x_series_id:
		return std::make_unique<series::one_div_two_minus_x_multi_three_plus_x_series<T, K>>(x);
	case Si_x_series_id:
		return std::make_unique<series::Si_x_series<T, K>>(x);
	case Ci_x_series_id:
		return std::make_unique<series::Ci_x_series<T, K>>(x);
	case Riemann_zeta_func_series_id:
		return std::make_unique<series::Riemann_zeta_func_series<T, K>>(x);
	case Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id:
		return std::make_unique<series::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>>(x);
	case xsquareplus3_div_xsquareplus2multix_minus_1_series_id:
		return std::make_unique<series::xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>>(x);
	case arcsin_x_series_id:
		return std::make_unique<series::arcsin_x_series<T, K>>(x);
	case arctg_x_series_id:
		return std::make_unique<series::arctg_x_series<T, K>>(x);
	case K_x_series_id:
		return std::make_unique<series::K_x_series<T, K>>(x);
	case E_x_series_id:
		return std::make_unique<series::E_x_series<T, K>>(x);
	case sqrt_1plusx_series_id:
		return std::make_unique<series::sqrt_1plusx_series<T, K>>(x);
	case Lambert_W_func_series_id:
		return std::make_unique<series::Lambert_W_func_series<T, K>>(x);
	case Incomplete_Gamma_func_series_id: {
		T s;
		std::cout << "Enter the value for constant s for the Incomplete Gamma series: ";
		std::cin >> s;
		return std::make_unique<series::Incomplete_Gamma_func_series<T, K>>(x, s);
	}
	case Series_with_ln_number1_series_id:
		return std::make_unique<series::Series_with_ln_number1_series<T, K>>(x);
	case Series_with_ln_number2_series_id:
		return std::make_unique<series::Series_with_ln_number2_series<T, K>>(x);
	case pi_series_id:
		return std::make_unique<series::pi_series<T, K>>(x);
	case x_min_sqrt_x_series_id:
		return std::make_unique<series::x_min_sqrt_x_series<T, K>>(x);
	case arctan_x2_series_id:
		return std::make_unique<series::arctan_x2_series<T, K>>(x);
	case ln1px4_series_id:
		return std::make_unique<series::ln1px4_series<T, K>>(x);
	case sin_x2_series_id:
		return std::make_unique<series::sin_x2_series<T, K>>(x);
	case arctan_x3_series_id:
		return std::make_unique<series::arctan_x3_series<T, K>>(x);
	case arcsin_x2_series_id:
		return std::make_unique<series::arcsin_x2_series<T, K>>(x);
	case ln1_m_x2_series_id:
		return std::make_unique<series::ln1_m_x2_series<T, K>>(x);
	case artanh_x_series_id:
		return std::make_unique<series::artanh_x_series<T, K>>(x);
	case arcsinh_x_series_id:
		return std::make_unique<series::arcsinh_x_series<T, K>>(x);
	case cos_x2_series_id:
		return std::make_unique<series::cos_x2_series<T, K>>(x);
	case sinh_x2_series_id:
		return std::make_unique<series::sinh_x2_series<T, K>>(x);
	case arctanh_x2_series_id:
		return std::make_unique<series::arctanh_x2_series<T, K>>(x);
	case cos3xmin1_div_xsqare_series_id:
		return std::make_unique<series::cos3xmin1_div_xsqare_series<T, K>>(x);
	case two_degree_x_series_id:
		return std::make_unique<series::two_degree_x_series<T, K>>(x);
	case sqrt_1plusx_min_1_min_x_div_2_series_id:
		return std::make_unique<series::sqrt_1plusx_min_1_min_x_div_2_series<T, K>>(x);
	case ln13_min_ln7_div_7_series_id:
		return std::make_unique<series::ln13_min_ln7_div_7_series<T, K>>(x);
	case Ja_x_series_id: {
		T a;
		std::cout << "Enter the value for constant a for the Ja_x series: ";
		std::cin >> a;
		return std::make_unique<series::Ja_x_series<T, K>>(x, a);
	}
	case one_div_sqrt2_sin_xdivsqrt2_series_id:
		return std::make_unique<series::one_div_sqrt2_sin_xdivsqrt2_series<T, K>>(x);
	case ln_1plusx_div_1plusx2_series_id:
		return std::make_unique<series::ln_1plusx_div_1plusx2_series<T, K>>(x);
	case cos_sqrt_x_series_id:
		return std::make_unique<series::cos_sqrt_x_series<T, K>>(x);
	case ln_1_plus_x3_series_id:
		return std::make_unique<series::ln_1_plus_x3_series<T, K>>(x);
	case x_div_1minx_series_id:
		return std::make_unique<series::x_div_1minx_series<T, K>>(x);
	case x_div_1minx2_series_id:
		return std::make_unique<series::x_div_1minx2_series<T, K>>(x);
	case gamma_series_id: {
		T t;
		std::cout << "Enter the parameter t for the gamma series: ";
		std::cin >> t;
		return std::make_unique<series::gamma_series<T, K>>(t, x);
	}

	default: throw std::domain_error("Series not implemented");
	}
}

/**
 * @brief Helper function to get transformation by ID
 */
template <FloatLike T, std::unsigned_integral K, typename SeriesType>
inline static std::unique_ptr<series_acceleration<T, K, SeriesType>>
create_transformation_by_id(transformation_id_t id, SeriesType series, bool is_alternating = false) {

	if (id == shanks_transformation_id) {
		if (is_alternating)
			return std::make_unique<shanks_transform_alternating<T, K, SeriesType>>(series);
		else
			return std::make_unique<shanks_algorithm<T, K, SeriesType>>(series);
	}
	switch (id) {
	case epsilon_algorithm_id:
		return std::make_unique<wynn_epsilon_1_algorithm<T, K, SeriesType>>(series);
	case epsilon_algorithm_2_id:
		return std::make_unique<wynn_epsilon_2_algorithm<T, K, SeriesType>>(series);
	case S_algorithm_id:
		return std::make_unique<levin_sidi_s_algorithm<T, K, SeriesType>>(series);
	case D_algorithm_id:
		return std::make_unique<drummond_d_algorithm<T, K, SeriesType>>(series, remainder_type::t_variant);
	case chang_epsilon_algorithm_id:
		return std::make_unique<chang_wynn_algorithm<T, K, SeriesType>>(series);
	case M_algorithm_id:
		return std::make_unique<levin_sidi_m_algorithm<T, K, SeriesType>>(series);
	case weniger_transformation_id:
		return std::make_unique<weniger_algorithm<T, K, SeriesType>>(series);
	case rho_wynn_transformation_id:
		return std::make_unique<wynn_rho_algorithm<T, K, SeriesType>>(series);
	case brezinski_theta_transformation_id:
		return std::make_unique<brezinski_theta_algorithm<T, K, SeriesType>>(series);
	case epsilon_algorithm_3_id:
		return std::make_unique<wynn_epsilon_3_algorithm<T, K, SeriesType>>(series);
	case W_algorithm_id:
		return std::make_unique<lubkin_w_algorithm<T, K, SeriesType>>(series);
	case richardson_algorithm_id:
		return std::make_unique<richardson_algorithm<T, K, SeriesType>>(series);
	case L_algorithm_id:
		return std::make_unique<levin_algorithm<T, K, SeriesType>>(series);
	case Ford_Sidi_algorithm_two_id:
		return std::make_unique<ford_sidi_2_algorithm<T, K, SeriesType>>(series);
	case Ford_Sidi_algorithm_three_id:
		return std::make_unique<ford_sidi_3_algorithm<T, K, SeriesType>>(series);
	default:
		throw std::domain_error("Invalid transformation ID");
	}
}

/**
 * @brief The main testing function
 * This function provides a convenient and interactive way to test out the convergence acceleration of various series
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <FloatLike T, std::unsigned_integral K>
inline static void main_testing_function()
{
	//choosing series
	print_series_info();
	std::unique_ptr<series::series_base<T, K>> series;
	K series_id = read_input<K>();

	//choosing x
	std::cout << "Enter x - the argument for the functional series" << '\n';
	T x = static_cast<T>(0);
	if constexpr (std::is_same<T, float_precision>::value){
		std::string kostil;
		std::cin >> kostil;

		x=T(kostil);

	} else {
		std::cin >> x;
	}

	// Create series using helper function
	series = create_series_by_id<T, K>(static_cast<series_id_t>(series_id), x);

	//choosing transformation
	print_transformation_info();
	K transformation_id = read_input<K>();

	std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform;
	std::set<K> alternating_series = { 2, 3, 7, 11, 15, 18, 19, 20, 21, 24, 26, 28, 30, 31 };
	bool is_alternating = alternating_series.contains(series_id);

	// Create transformation using helper function
	transform = create_transformation_by_id<T, K, decltype(series.get())>(
		static_cast<transformation_id_t>(transformation_id),
		series.get(),
		is_alternating
	);

	//choosing testing function
	T beta_Levin_S_algorithm = T{};
	T gamma_Levin_M_algorithm = T{};
	T gamma_rho_Wynn_algorithm = T{};
	T RHO_rho_Wynn_algorithm = T{};
	T beta_levin_recursion_algorithm = T{};
	T epsilon_algorithm_3 = T{};

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
		K cmp_transformation_id = read_input<K>();

		std::unique_ptr<series_acceleration<T, K, decltype(series.get())>> transform2;
		transform2 = create_transformation_by_id<T, K, decltype(series.get())>(
			static_cast<transformation_id_t>(cmp_transformation_id),
			series.get(),
			is_alternating
		);

		cmp_transformations(n, order, std::move(series.get()), std::move(transform.get()), std::move(transform2.get()));
		break;
	}
	case test_function_id_t::eval_transform_time_id:
		eval_transform_time(n, order, std::move(series.get()), std::move(transform.get()));
		break;
	case test_function_id_t::test_all_transforms_id:
	{
		// Testing all functions for series
		auto create_all_transformations = [&]() {
			std::vector<std::unique_ptr<series_acceleration<T, K, decltype(series.get())>>> transforms;

			// shanks
			if (alternating_series.contains(series_id))
				transforms.push_back(std::make_unique<shanks_transform_alternating<T, K, decltype(series.get())>>(series.get()));
			else
				transforms.push_back(std::make_unique<shanks_algorithm<T, K, decltype(series.get())>>(series.get()));

			// epsilon v-1
			transforms.push_back(std::make_unique<wynn_epsilon_1_algorithm<T, K, decltype(series.get())>>(series.get()));

			// epsilon v-2
			transforms.push_back(std::make_unique<wynn_epsilon_2_algorithm<T, K, decltype(series.get())>>(series.get()));

			// epsilon v-3
			transforms.push_back(std::make_unique<wynn_epsilon_3_algorithm<T, K, decltype(series.get())>>(series.get(), epsilon_algorithm_3));

			// rho-wynn classic
			transforms.push_back(std::make_unique<wynn_rho_algorithm<T, K, decltype(series.get())>>(series.get(), numerator_type::rho_variant));

			// rho-wynn generalized
			transforms.push_back(std::make_unique<wynn_rho_algorithm<T, K, decltype(series.get())>>(series.get(), numerator_type::generalized_variant));

			// rho-wynn gamma-rho
			transforms.push_back(std::make_unique<wynn_rho_algorithm<T, K, decltype(series.get())>>(series.get(), numerator_type::gamma_rho_variant));

			// brezinski-theta
			transforms.push_back(std::make_unique<brezinski_theta_algorithm<T, K, decltype(series.get())>>(series.get()));

			// chang epsilon wynn
			transforms.push_back(std::make_unique<chang_wynn_algorithm<T, K, decltype(series.get())>>(series.get()));

			// levin standart
			transforms.push_back(std::make_unique<levin_algorithm<T, K, decltype(series.get())>>(series.get()));

			// levin recurcive
			transforms.push_back(std::make_unique<levin_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::u_variant, true));

			// levin-sidi S U
			transforms.push_back(std::make_unique<levin_sidi_s_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::u_variant, false));

			// levin-sidi S T
			transforms.push_back(std::make_unique<levin_sidi_s_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::t_variant, false));

			// levin-sidi S T-WAVE
			transforms.push_back(std::make_unique<levin_sidi_s_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::t_wave_variant, false));

			// levin-sidi S V
			transforms.push_back(std::make_unique<levin_sidi_s_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::v_variant, false));

			// levin-sidi D U
			transforms.push_back(std::make_unique<drummond_d_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::u_variant, false));

			// levin-sidi D T
			transforms.push_back(std::make_unique<drummond_d_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::t_variant, false));

			// levin-sidi D T-WAVE
			transforms.push_back(std::make_unique<drummond_d_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::t_wave_variant, false));

			// levin-sidi D V
			transforms.push_back(std::make_unique<drummond_d_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::v_variant, false));

			// levin-sidi M U
			transforms.push_back(std::make_unique<levin_sidi_m_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::u_variant));

			// levin-sidi M T
			transforms.push_back(std::make_unique<levin_sidi_m_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::t_variant));

			// levin-sidi M T-WAVE
			transforms.push_back(std::make_unique<levin_sidi_m_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::t_wave_variant));

			// levin-sidi M V-WAVE
			transforms.push_back(std::make_unique<levin_sidi_m_algorithm<T, K, decltype(series.get())>>(series.get(), remainder_type::v_wave_variant));

			// weniger
			transforms.push_back(std::make_unique<weniger_algorithm<T, K, decltype(series.get())>>(series.get()));

			// lubkin W
			transforms.push_back(std::make_unique<lubkin_w_algorithm<T, K, decltype(series.get())>>(series.get()));

			// Richardson
			transforms.push_back(std::make_unique<richardson_algorithm<T, K, decltype(series.get())>>(series.get()));

			// Ford-Sidi v-2
			transforms.push_back(std::make_unique<ford_sidi_2_algorithm<T, K, decltype(series.get())>>(series.get()));

			// Ford-Sidi v-3
			transforms.push_back(std::make_unique<ford_sidi_3_algorithm<T, K, decltype(series.get())>>(series.get()));

			return transforms;
			};

		auto all_transforms = create_all_transformations();

		for (K i = 0; i <= n; i++)
		{
			print_sum(i, std::move(series.get()));

			for (auto& current_transform : all_transforms)
			{
				print_transform(i, order, std::move(current_transform.get()));
			}

			std::cout << '\n';
		}
		break;
	}
	default:
		throw std::domain_error("wrong function_id");
	}
}