/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G., Maximov A.K.
 */

#pragma once

#include "custom_concepts.hpp"
#include <cstdio>
#include <linux/limits.h>
#include <memory>
#include <set>
#include <unordered_map>

#ifndef INC_FPRECISION
	#include "libs/arbitrary_arithmetics/fprecision.h"
#endif

#ifndef INC_COMPLEXPRECISION
	#include "libs/arbitrary_arithmetics/complexprecision.h"
#endif

#include "methods.hpp"
#include "series.hpp"
#include "test_functions.hpp"
#include "noise/noise_generator.hpp"


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
	series_id_t getId() const override { return one_twelfth_x2_pi2_series_id; }
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
	series_id_t getId() const override { return pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series_id; }
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
	series_id_t getId() const override { return pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series_id; }
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
	series_id_t getId() const override { return si_x_series_id; }
	std::string getName() const override { return "Si_x_series"; }
};

class Ci_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return ci_x_series_id; }
	std::string getName() const override { return "Ci_x_series"; }
};

class Riemann_zeta_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return riemann_zeta_func_series_id; }
	std::string getName() const override { return "Riemann_zeta_func_series"; }
};

class Riemann_zeta_func_xmin1_div_Riemann_zeta_func_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id; }
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
	series_id_t getId() const override { return k_x_series_id; }
	std::string getName() const override { return "K_x_series"; }
};

class E_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return e_x_series_id; }
	std::string getName() const override { return "E_x_series"; }
};

class Sqrt_1plusxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return sqrt_1plusx_series_id; }
	std::string getName() const override { return "sqrt_1plusx_series"; }
};

class Lambert_W_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return lambert_W_func_series_id; }
	std::string getName() const override { return "Lambert_W_func_series"; }
};

class Incomplete_Gamma_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return incomplete_Gamma_func_series_id; }
	std::string getName() const override { return "Incomplete_Gamma_func_series"; }
};

class Series_with_ln_number1SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return series_with_ln_number1_series_id; }
	std::string getName() const override { return "Series_with_ln_number1_series"; }
};

class Series_with_ln_number2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return series_with_ln_number2_series_id; }
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
	series_id_t getId() const override { return arctg_x2_series_id; }
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
	series_id_t getId() const override { return arctg_x3_series_id; }
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

class Arctanh_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override { return arctanh_x_series_id; }
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
	series_id_t getId() const override { return cos3xmin1_div_xsquare_series_id; }
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
	transformation_id_t getId() const override { return wynn_rho_rho_transformation_id; }
	std::string getName() const override { return "Rho-Wynn transformation"; }
};

class BrezinskiThetaTransformationInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return brezinski_theta_transformation_id; }
	std::string getName() const override { return "Brezinski Theta transformation"; }
};

class EpsilonAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return wynn_epsilon_1_transforamtion_id; }
	std::string getName() const override { return "Epsilon algorithm"; }
};

class EpsilonAlgorithm2Info : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return wynn_epsilon_2_transforamtion_id; }
	std::string getName() const override { return "Epsilon v2 algorithm"; }
};

class EpsilonAlgorithm3Info : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return wynn_epsilon_3_transforamtion_id; }
	std::string getName() const override { return "Epsilon v3 algorithm"; }
};

class ChangEpsilonAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return chang_wynn_transformation_id; }
	std::string getName() const override { return "Chang epsilon algorithm"; }
};

class LAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return levin_sidi_l_u_transformation_id; }
	std::string getName() const override { return "L algorithm"; }
};

class SAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return levin_sidi_s_u_transformation_id; }
	std::string getName() const override { return "S algorithm"; }
};

class DAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return drummond_d_u_transformation_id; }
	std::string getName() const override { return "D algorithm"; }
};

class MAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return levin_sidi_m_u_transformation_id; }
	std::string getName() const override { return "M algorithm"; }
};

class WAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return lubkin_w_transformation_id; }
	std::string getName() const override { return "W algorithm"; }
};

class RichardsonAlgorithmInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return richardson_transformation_id; }
	std::string getName() const override { return "Richardson Algorithm"; }
};

class FordSidiAlgorithmTwoInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return ford_sidi_2_transformation_id; }
	std::string getName() const override { return "Ford-Sidi Algorithm V-2"; }
};

class FordSidiAlgorithmThreeInfo : public ITransformationInfo {
public:
	transformation_id_t getId() const override { return ford_sidi_3_transformation_id; }
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
		std::make_unique<Abs_sin_x_minus_2_div_piSeriesInfo>(),
		std::make_unique<Arcsinh_xSeriesInfo>(),
		std::make_unique<Arcsin_x2SeriesInfo>(),
		std::make_unique<Arcsin_x_minus_xSeriesInfo>(),
		std::make_unique<Arcsin_xSeriesInfo>(),
		std::make_unique<Arctanh_x2SeriesInfo>(),
		std::make_unique<Arctanh_xSeriesInfo>(),
		std::make_unique<Arctan_x2SeriesInfo>(),
		std::make_unique<Arctan_x3SeriesInfo>(),
		std::make_unique<Arctg_xSeriesInfo>(),
		std::make_unique<BinSeriesInfo>(),
		std::make_unique<Ci_xSeriesInfo>(),
		std::make_unique<Cos3xmin1_div_xsqareSeriesInfo>(),
		std::make_unique<CoshSeriesInfo>(),
		std::make_unique<CosSeriesInfo>(),
		std::make_unique<Cos_sqrt_xSeriesInfo>(),
		std::make_unique<Cos_x2SeriesInfo>(),
		std::make_unique<Eighth_pi_m_one_thirdSeriesInfo>(),
		std::make_unique<ErfSeriesInfo>(),
		std::make_unique<Exp_m_cos_x_sinsin_xSeriesInfo>(),
        std::make_unique<ExpSeriesInfo>(),
		std::make_unique<Exp_squared_erfSeriesInfo>(),
		std::make_unique<E_xSeriesInfo>(),
		std::make_unique<Five_pi_twelveSeriesInfo>(),
		std::make_unique<Four_arctanSeriesInfo>(),
		std::make_unique<Four_ln2_m_3SeriesInfo>(),
		std::make_unique<Half_asin_two_xSeriesInfo>(),
		std::make_unique<Half_minus_sinx_multi_pi_4SeriesInfo>(),
		std::make_unique<Half_multi_ln_1div2multi1minuscosxSeriesInfo>(),
		std::make_unique<Incomplete_Gamma_funcSeriesInfo>(),
		std::make_unique<Inverse_1mxSeriesInfo>(),
		std::make_unique<Inverse_sqrt_1m4xSeriesInfo>(),
		std::make_unique<K_xSeriesInfo>(),
		std::make_unique<Lambert_W_funcSeriesInfo>(),
		std::make_unique<Ln13_min_ln7_div_7SeriesInfo>(),
		std::make_unique<Ln1_m_x2SeriesInfo>(),
		std::make_unique<Ln1mxSeriesInfo>(),
		std::make_unique<Ln_1plussqrt1plusxsquare_minus_ln_2SeriesInfo>(),
		std::make_unique<Ln_1_plus_x3SeriesInfo>(),
		std::make_unique<Ln_1plusx_div_1plusx2SeriesInfo>(),
		std::make_unique<Ln1px4SeriesInfo>(),
		std::make_unique<Ln2SeriesInfo>(),
		std::make_unique<Ln_cosxSeriesInfo>(),
		std::make_unique<Ln_sinx_minus_ln_xSeriesInfo>(),
		std::make_unique<Ln_x_plus_one_x_minus_one_halfedSeriesInfo>(),
		std::make_unique<Mean_sinh_sinSeriesInfo>(),
		std::make_unique<M_fact_1mx_mp1_inverseSeriesInfo>(),
		std::make_unique<Minus_3_div_4_or_x_minus_3_div_4SeriesInfo>(),
		std::make_unique<Minus_one_ned_in_nSeriesInfo>(),
		std::make_unique<Minus_one_n_fact_n_in_nSeriesInfo>(),
		std::make_unique<Minus_one_quarterSeriesInfo>(),
		std::make_unique<Minus_three_plus_ln3_three_devided_two_plus_two_ln2SeriesInfo>(),
		std::make_unique<Minus_x_minus_pi_4_or_minus_pi_4SeriesInfo>(),
		std::make_unique<One_div_sqrt2_sin_xdivsqrt2SeriesInfo>(),
		std::make_unique<One_div_two_minus_x_multi_three_plus_xSeriesInfo>(),
		std::make_unique<One_minus_sqrt_1minus4x_div_2xSeriesInfo>(),
		std::make_unique<OneSeriesInfo>(),
		std::make_unique<One_third_pi_squared_m_nineSeriesInfo>(),
		std::make_unique<One_twelfth_3x2_pi2SeriesInfo>(),
		std::make_unique<One_twelfthSeriesInfo>(),
		std::make_unique<Pi_3SeriesInfo>(),
		std::make_unique<Pi_4SeriesInfo>(),
		std::make_unique<Pi_8_cosx_square_minus_1_div_3_cosxSeriesInfo>(),
		std::make_unique<Pi_cubed_32SeriesInfo>(),
		std::make_unique<Pi_four_minus_ln2_halfedSeriesInfo>(),
		std::make_unique<Pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4SeriesInfo>(),
		std::make_unique<Pi_minus_x_2SeriesInfo>(),
		std::make_unique<PiSeriesInfo>(),
		std::make_unique<Pi_six_min_halfSeriesInfo>(),
		std::make_unique<Pi_squared_6_minus_oneSeriesInfo>(),
		std::make_unique<Pi_squared_twelveSeriesInfo>(),
		std::make_unique<Pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_squareSeriesInfo>(),
		std::make_unique<Pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_oneSeriesInfo>(),
		std::make_unique<Riemann_zeta_funcSeriesInfo>(),
		std::make_unique<Riemann_zeta_func_xmin1_div_Riemann_zeta_func_xSeriesInfo>(),
		std::make_unique<Series_with_ln_number1SeriesInfo>(),
		std::make_unique<Series_with_ln_number2SeriesInfo>(),
		std::make_unique<SinhSeriesInfo>(),
		std::make_unique<Sinh_x2SeriesInfo>(),
		std::make_unique<SinSeriesInfo>(),
		std::make_unique<Sin_x2SeriesInfo>(),
		std::make_unique<Si_xSeriesInfo>(),
		std::make_unique<Sqrt_1plusx_min_1_min_x_div_2SeriesInfo>(),
		std::make_unique<Sqrt_1plusxSeriesInfo>(),
		std::make_unique<Sqrt_oneminussqrtoneminusx_div_xSeriesInfo>(),
		std::make_unique<Ten_minus_xSeriesInfo>(),
		std::make_unique<Three_minus_piSeriesInfo>(),
		std::make_unique<Two_arcsin_square_x_halfedSeriesInfo>(),
		std::make_unique<Two_degree_xSeriesInfo>(),
		std::make_unique<Two_ln2SeriesInfo>(),
		std::make_unique<X_1mx_squaredSeriesInfo>(),
		std::make_unique<X_div_1minx2SeriesInfo>(),
		std::make_unique<X_div_1minxSeriesInfo>(),
		std::make_unique<X_min_sqrt_xSeriesInfo>(),
		std::make_unique<XSeriesInfo>(),
		std::make_unique<Xsquareplus3_div_xsquareplus2multix_minus_1SeriesInfo>(),
		std::make_unique<X_twelfth_x2_pi2SeriesInfo>(),
		std::make_unique<X_twoSeriesInfo>(),
		std::make_unique<X_two_throught_squaresSeriesInfo>(),
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
template <AcceptedLike T, std::unsigned_integral K>
inline static std::unique_ptr<series_base<T, K>> create_series_by_id(series_id_t id) {
	// This function replace huge switch-case
	auto all_series = create_series_info();

	// Здесь нужно будет сделать mapping от ID к конструктору
	// Пока оставляю старый switch, но можно оптимизировать дальше
	switch (id) {
	case exp_series_id:
		return std::make_unique<exp_series<T, K>>();
	case cos_series_id:
		return std::make_unique<cos_series<T, K>>();
	case sin_series_id:
		return std::make_unique<sin_series<T, K>>();
	case cosh_series_id:
		return std::make_unique<cosh_series<T, K>>();
	case sinh_series_id:
		return std::make_unique<sinh_series<T, K>>();
	case bin_series_id:
		return std::make_unique<bin_series<T, K>>();
	case four_arctan_series_id:
		return std::make_unique<four_arctan_series<T, K>>();
	case ln1mx_series_id:
		return std::make_unique<ln1mx_series<T, K>>();
	case mean_sinh_sin_series_id:
		return std::make_unique<mean_sinh_sin_series<T, K>>();
	case exp_squared_erf_series_id:
		return std::make_unique<exp_squared_erf_series<T, K>>();
	case half_asin_two_x_series_id:
		return std::make_unique<half_asin_two_x_series<T, K>>();
	case inverse_1mx_series_id:
		return std::make_unique<inverse_1mx_series<T, K>>();
	case x_1mx_squared_series_id:
		return std::make_unique<x_1mx_squared_series<T, K>>();
	case erf_series_id:
		return std::make_unique<erf_series<T, K>>();
	case m_fact_1mx_mp1_inverse_series_id:
		return std::make_unique<m_fact_1mx_mp1_inverse_series<T, K>>();
	case inverse_sqrt_1m4x_series_id:
		return std::make_unique<inverse_sqrt_1m4x_series<T, K>>();
	case one_twelfth_3x2_pi2_series_id:
		return std::make_unique<one_twelfth_3x2_pi2_series<T, K>>();
	case one_twelfth_x2_pi2_series_id:
		return std::make_unique<one_twelfth_x2_pi2_series<T, K>>();
	case ln2_series_id:
		return std::make_unique<ln2_series<T, K>>();
	case one_series_id:
		return std::make_unique<one_series<T, K>>();
	case minus_one_quarter_series_id:
		return std::make_unique<minus_one_quarter_series<T, K>>();
	case pi_3_series_id:
		return std::make_unique<pi_3_series<T, K>>();
	case pi_4_series_id:
		return std::make_unique<pi_4_series<T, K>>();
	case pi_squared_6_minus_one_series_id:
		return std::make_unique<pi_squared_6_minus_one_series<T, K>>();
	case three_minus_pi_series_id:
		return std::make_unique<three_minus_pi_series<T, K>>();
	case one_twelfth_series_id:
		return std::make_unique<one_twelfth_series<T, K>>();
	case eighth_pi_m_one_third_series_id:
		return std::make_unique<eighth_pi_m_one_third_series<T, K>>();
	case one_third_pi_squared_m_nine_series_id:
		return std::make_unique<one_third_pi_squared_m_nine_series<T, K>>();
	case four_ln2_m_3_series_id:
		return std::make_unique<four_ln2_m_3_series<T, K>>();
	case exp_m_cos_x_sinsin_x_series_id:
		return std::make_unique<exp_m_cos_x_sinsin_x_series<T, K>>();
	case pi_four_minus_ln2_halfed_series_id:
		return std::make_unique<pi_four_minus_ln2_halfed_series<T, K>>();
	case five_pi_twelve_series_id:
		return std::make_unique<five_pi_twelve_series<T, K>>();
	case x_two_series_id:
		return std::make_unique<x_two_series<T, K>>();
	case pi_six_min_half_series_id:
		return std::make_unique<pi_six_min_half_series<T, K>>();
	case x_two_throught_squares_series_id:
		return std::make_unique<x_two_throught_squares_series<T, K>>();
	case minus_one_ned_in_n_series_id:
		return std::make_unique<minus_one_ned_in_n_series<T, K>>();
	case minus_one_n_fact_n_in_n_series_id:
		return std::make_unique<minus_one_n_fact_n_in_n_series<T, K>>();
	case ln_x_plus_one_x_minus_one_halfed_series_id:
		return std::make_unique<ln_x_plus_one_x_minus_one_halfed_series<T, K>>();
	case two_arcsin_square_x_halfed_series_id:
		return std::make_unique<two_arcsin_square_x_halfed_series<T, K>>();
	case pi_squared_twelve_series_id:
		return std::make_unique<pi_squared_twelve_series<T, K>>();
	case pi_cubed_32_series_id:
		return std::make_unique<pi_cubed_32_series<T, K>>();
	case minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id:
		return std::make_unique<minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>>();
	case two_ln2_series_id:
		return std::make_unique<two_ln2_series<T, K>>();
	case pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series_id:
		return std::make_unique<pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series<T, K>>();
	case pi_minus_x_2_series_id:
		return std::make_unique<pi_minus_x_2_series<T, K>>();
	case half_multi_ln_1div2multi1minuscosx_series_id:
		return std::make_unique<half_multi_ln_1div2multi1minuscosx_series<T, K>>();
	case half_minus_sinx_multi_pi_4_series_id:
		return std::make_unique<half_minus_sinx_multi_pi_4_series<T, K>>();
	case ln_1plussqrt1plusxsquare_minus_ln_2_series_id:
		return std::make_unique<ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>>();
	case ln_cosx_series_id:
		return std::make_unique<ln_cosx_series<T, K>>();
	case ln_sinx_minus_ln_x_series_id:
		return std::make_unique<ln_sinx_minus_ln_x_series<T, K>>();
	case pi_8_cosx_square_minus_1_div_3_cosx_series_id:
		return std::make_unique<pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>>();
	case sqrt_oneminussqrtoneminusx_div_x_series_id:
		return std::make_unique<sqrt_oneminussqrtoneminusx_div_x_series<T, K>>();
	case one_minus_sqrt_1minus4x_div_2x_series_id:
		return std::make_unique<one_minus_sqrt_1minus4x_div_2x_series<T, K>>();
	case arcsin_x_minus_x_series_id:
		return std::make_unique<arcsin_x_minus_x_series<T, K>>();
	case pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series_id:
		return std::make_unique<pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<T, K>>();
	case abs_sin_x_minus_2_div_pi_series_id:
		return std::make_unique<abs_sin_x_minus_2_div_pi_series<T, K>>();
	case pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id:
		return std::make_unique<pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>>();
	case minus_3_div_4_or_x_minus_3_div_4_series_id:
		return std::make_unique<minus_3_div_4_or_x_minus_3_div_4_series<T, K>>();
	case ten_minus_x_series_id:
		return std::make_unique<ten_minus_x_series<T, K>>();
	case x_series_id:
		return std::make_unique<x_series<T, K>>();
	case minus_x_minus_pi_4_or_minus_pi_4_series_id:
		return std::make_unique<minus_x_minus_pi_4_or_minus_pi_4_series<T, K>>();
	case one_div_two_minus_x_multi_three_plus_x_series_id:
		return std::make_unique<one_div_two_minus_x_multi_three_plus_x_series<T, K>>();
	case si_x_series_id:
		return std::make_unique<si_x_series<T, K>>();
	case ci_x_series_id:
		return std::make_unique<ci_x_series<T, K>>();
	case riemann_zeta_func_series_id:
		return std::make_unique<riemann_zeta_func_series<T, K>>();
	case riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id:
		return std::make_unique<riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>>();
	case xsquareplus3_div_xsquareplus2multix_minus_1_series_id:
		return std::make_unique<xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>>();
	case arcsin_x_series_id:
		return std::make_unique<arcsin_x_series<T, K>>();
	case arctg_x_series_id:
		return std::make_unique<arctg_x_series<T, K>>();
	case k_x_series_id:
		return std::make_unique<k_x_series<T, K>>();
	case e_x_series_id:
		return std::make_unique<e_x_series<T, K>>();
	case sqrt_1plusx_series_id:
		return std::make_unique<sqrt_1plusx_series<T, K>>();
	case lambert_W_func_series_id:
		return std::make_unique<lambert_W_func_series<T, K>>();
	case incomplete_Gamma_func_series_id:
		return std::make_unique<incomplete_Gamma_func_series<T, K>>();
	case series_with_ln_number1_series_id:
		return std::make_unique<series_with_ln_number1_series<T, K>>();
	case series_with_ln_number2_series_id:
		return std::make_unique<series_with_ln_number2_series<T, K>>();
	case pi_series_id:
		return std::make_unique<pi_series<T, K>>();
	case x_min_sqrt_x_series_id:
		return std::make_unique<x_min_sqrt_x_series<T, K>>();
	case arctg_x2_series_id:
		return std::make_unique<arctg_x2_series<T, K>>();
	case ln1px4_series_id:
		return std::make_unique<ln1px4_series<T, K>>();
	case sin_x2_series_id:
		return std::make_unique<sin_x2_series<T, K>>();
	case arctg_x3_series_id:
		return std::make_unique<arctg_x3_series<T, K>>();
	case arcsin_x2_series_id:
		return std::make_unique<arcsin_x2_series<T, K>>();
	case ln1_m_x2_series_id:
		return std::make_unique<ln1_m_x2_series<T, K>>();
	case arctanh_x_series_id:
		return std::make_unique<arctanh_x_series<T, K>>();
	case arcsinh_x_series_id:
		return std::make_unique<arcsinh_x_series<T, K>>();
	case cos_x2_series_id:
		return std::make_unique<cos_x2_series<T, K>>();
	case sinh_x2_series_id:
		return std::make_unique<sinh_x2_series<T, K>>();
	case arctanh_x2_series_id:
		return std::make_unique<arctanh_x2_series<T, K>>();
	case cos3xmin1_div_xsquare_series_id:
		return std::make_unique<cos3xmin1_div_xsquare_series<T, K>>();
	case two_degree_x_series_id:
		return std::make_unique<two_degree_x_series<T, K>>();
	case sqrt_1plusx_min_1_min_x_div_2_series_id:
		return std::make_unique<sqrt_1plusx_min_1_min_x_div_2_series<T, K>>();
	case ln13_min_ln7_div_7_series_id:
		return std::make_unique<ln13_min_ln7_div_7_series<T, K>>();
	case one_div_sqrt2_sin_xdivsqrt2_series_id:
		return std::make_unique<one_div_sqrt2_sin_xdivsqrt2_series<T, K>>();
	case ln_1plusx_div_1plusx2_series_id:
		return std::make_unique<ln_1plusx_div_1plusx2_series<T, K>>();
	case cos_sqrt_x_series_id:
		return std::make_unique<cos_sqrt_x_series<T, K>>();
	case ln_1_plus_x3_series_id:
		return std::make_unique<ln_1_plus_x3_series<T, K>>();
	case x_div_1minx_series_id:
		return std::make_unique<x_div_1minx_series<T, K>>();
	case x_div_1minx2_series_id:
		return std::make_unique<x_div_1minx2_series<T, K>>();

	default: throw std::domain_error("Series not implemented");
	}
}

/**
 * @brief Helper function to get transformation by ID
 */
template <AcceptedLike T, std::unsigned_integral K>
inline static std::unique_ptr<series_acceleration<T, K>>
create_transformation_by_id(transformation_id_t id) {

	switch (id) {
		case  		       brezinski_theta_transformation_id:
		{
			return std::make_unique<brezinski_theta_algorithm<T,K>>(brezinski_theta_algorithm<T,K>());
		}
		case                    chang_wynn_transformation_id:
		{
			return std::make_unique<chang_wynn_algorithm<T,K>>(chang_wynn_algorithm<T,K>());
		}
		case                  drummond_d_u_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::u_variant, false
			));
		}
		case                  drummond_d_t_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::t_variant, false
			));
		}
		case             drummond_d_t_wave_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::t_wave_variant, false
			));
		}
		case                  drummond_d_v_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::v_variant, false
			));
		}
		case             drummond_d_v_wave_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::v_wave_variant, false
			));
		}
		case        recurrent_drummond_d_u_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::u_variant, true
			));
		}
		case        recurrent_drummond_d_t_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::t_variant, true
			));
		}
		case        recurrent_drummond_d_v_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::v_variant, true
			));
		}
		case   recurrent_drummond_d_t_wave_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::t_wave_variant, true
			));
		}
		case   recurrent_drummond_d_v_wave_transformation_id:
		{
			return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>(
				remainder_type::v_wave_variant, true
			));
		}
		case                   ford_sidi_2_transformation_id:
		{
			return std::make_unique<ford_sidi_2_algorithm<T,K>>(ford_sidi_2_algorithm<T,K>());
		}
		case                   ford_sidi_3_transformation_id:
		{
			return std::make_unique<ford_sidi_3_algorithm<T,K>>(ford_sidi_3_algorithm<T,K>());
		}
		case                levin_sidi_l_u_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::u_variant, false
			));
		}
		case                levin_sidi_l_t_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::t_variant, false
			));
		}
		case           levin_sidi_l_t_wave_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::t_wave_variant, false
			));
		}
		case                levin_sidi_l_v_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::v_variant, false
			));
		}
		case           levin_sidi_l_v_wave_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::v_wave_variant, false
			));
		}
		case      recurrent_levin_sidi_l_u_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::u_variant, true
			));
		}
		case      recurrent_levin_sidi_l_t_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::t_variant, true
			));
		}
		case      recurrent_levin_sidi_l_v_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::v_variant, true
			));
		}
		case recurrent_levin_sidi_l_t_wave_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::t_wave_variant, true
			));
		}
		case recurrent_levin_sidi_l_v_wave_transformation_id:
		{
			return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>(
				remainder_type::v_wave_variant, true
			));
		}
		case                levin_sidi_m_u_transformation_id:
		{
			return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>(
				remainder_type::u_variant, false
			));
		}
		case                levin_sidi_m_t_transformation_id:
		{
			return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>(
				remainder_type::t_variant, false
			));
		}
		case           levin_sidi_m_t_wave_transformation_id:
		{
			return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>(
				remainder_type::t_wave_variant, false
			));
		}
		case                levin_sidi_m_v_transformation_id:
		{
			return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>(
				remainder_type::v_variant, false
			));
		}
		case           levin_sidi_m_v_wave_transformation_id:
		{
			return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>(
				remainder_type::v_wave_variant, false
			));
		}
		case                levin_sidi_s_u_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::u_variant, false
			));
		}
		case                levin_sidi_s_t_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::t_variant, false
			));
		}
		case           levin_sidi_s_t_wave_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::t_wave_variant, false
			));
		}
		case                levin_sidi_s_v_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::v_variant, false
			));
		}
		case           levin_sidi_s_v_wave_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::v_wave_variant, false
			));
		}
		case      recurrent_levin_sidi_s_u_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::u_variant, true
			));
		}
		case      recurrent_levin_sidi_s_t_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::t_variant, true
			));
		}
		case      recurrent_levin_sidi_s_v_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::v_variant, true
			));
		}
		case recurrent_levin_sidi_s_t_wave_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::t_wave_variant, true
			));
		}
		case recurrent_levin_sidi_s_v_wave_transformation_id:
		{
			return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>(
				remainder_type::v_wave_variant, true
			));
		}
		case                      lubkin_w_transformation_id:
		{
			return std::make_unique<lubkin_w_algorithm<T,K>>(lubkin_w_algorithm<T,K>());
		}
		case                    richardson_transformation_id:
		{
			return std::make_unique<richardson_algorithm<T,K>>(richardson_algorithm<T,K>());
		}
		case                        shanks_transformation_id:
		{
			return std::make_unique<richardson_algorithm<T,K>>(richardson_algorithm<T,K>());
		}
		case            shanks_alternating_transformation_id:
		{
			return std::make_unique<richardson_algorithm<T,K>>(richardson_algorithm<T,K>());
		}
		case                       weniger_transformation_id:
		{
			return std::make_unique<weniger_algorithm<T,K>>(weniger_algorithm<T,K>());
		}
		case                wynn_epsilon_1_transforamtion_id:
		{
			return std::make_unique<wynn_epsilon_1_algorithm<T,K>>(wynn_epsilon_1_algorithm<T,K>());
		}
		case                wynn_epsilon_2_transforamtion_id:
		{
			return std::make_unique<wynn_epsilon_2_algorithm<T,K>>(wynn_epsilon_2_algorithm<T,K>());
		}
		case                wynn_epsilon_3_transforamtion_id:
		{
			return std::make_unique<wynn_epsilon_3_algorithm<T,K>>(wynn_epsilon_3_algorithm<T,K>());
		}
		case                  wynn_rho_rho_transformation_id:
		{
			return std::make_unique<wynn_rho_algorithm<T,K>>(wynn_rho_algorithm<T,K>(
				numerator_type::rho_variant
			));
		}
		case          wynn_rho_generalized_transformation_id:
		{
			return std::make_unique<wynn_rho_algorithm<T,K>>(wynn_rho_algorithm<T,K>(
				numerator_type::generalized_variant
			));
		}
		case            wynn_rho_gamma_rho_transformation_id:
		{
			return std::make_unique<wynn_rho_algorithm<T,K>>(wynn_rho_algorithm<T,K>(
				numerator_type::gamma_rho_variant
			));
		}
		default:
			throw std::domain_error("Invalid transformation ID");
	}
}

/**
 * @brief The main testing function
 * This function provides a convenient and interactive way to test out the convergence acceleration of various series
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <AcceptedLike T, std::unsigned_integral K>
inline static void main_testing_function()
{


	
	//choosing series
	print_series_info();
	std::unique_ptr<series_base<T, K>> series;
	K series_id = read_input<K>();

	std::cout << "\nChosen series id " <<  series_id << "\n";

	// Create series using helper function
	series = create_series_by_id<T, K>(static_cast<series_id_t>(series_id));


	std::cout << series->get_name() << "\n";
	//choosing x
	std::cout << "Enter x - the argument for the functional series" << '\n';
	T x = inputCustomType<T>();

	T tParam = static_cast<T>(1);
	K kParam = static_cast<K>(1);

	switch(series_id){
		case bin_series_id:{
			std::cout << "for bin series must initialise variable alpha\n";
			tParam = inputCustomType<T>("alpha");
		}
		case incomplete_Gamma_func_series_id:{
			std::cout << "for incomplete Gamma function must initialise variable alpha\n";
			tParam = inputCustomType<T>("alpha");
		}
		case m_fact_1mx_mp1_inverse_series_id:{
			std::cout << "for m factorial 1mx mp1 ... series must initialise variable alpha\n";
			kParam = inputCustomType<K>("m");
		}
		default:
		 break;
	};

	//choosing transformation
	print_transformation_info();
	K transformation_id = read_input<K>();

	std::unique_ptr<series_acceleration<T, K>> transform;

	// Create transformation using helper function
	transform = create_transformation_by_id<T, K>(static_cast<transformation_id_t>(transformation_id));

	transform->print_info();
	print_test_function_info();
	K function_id = read_input<K>();
	std::cout << "Enter n and order:" << '\n';
	K n = read_input<K>();
	K order = read_input<K>();
	SeriesResult<T> result = series->generateSeries(x, n + 3 * order + 1, tParam, kParam);

	std::string answer = "ok";
	while(answer != "Y" && answer != "Yes" && answer != "N" && answer != "No"){
		std::cout << "Appply noise [Y]es, [N]o: "; std::cin >> answer;
	};

	if (answer == "Y" || answer == "Yes"){
		size_t noise_type;
		std::cout << "Noise type: uniform[0], normal[1], poisson[2]\n";
		std::cin >> noise_type; std::cout << "\n";
		NoiseGenerator<T> gen = NoiseGenerator<T>(static_cast<NoiseType>(noise_type), 42);
		result = gen.jitter(result, 0.05, 1e-6);
	}

	//result = applyNose(result, noise_type);

	switch (function_id)
	{
	case test_function_id_t::cmp_sum_and_transform_id:
		cmp_sum_and_transform<T,K>(
			n, order, 
			series.get(),
			result,
			transform.get()
		);
		break;
	case test_function_id_t::cmp_a_n_and_transform_id:
		cmp_a_n_and_transform<T,K>(
			n, order,
			series.get(),
			result,
			transform.get()
		);
		break;
	case test_function_id_t::transformation_remainder_id:
		transformation_remainders<T,K>(
			n, order, 
			series.get(),
			result,
			transform.get()
		);
		break;
	case test_function_id_t::cmp_transformations_id:
	{
		print_transformation_info();
		K cmp_transformation_id = read_input<K>();
		std::unique_ptr<series_acceleration<T, K>> transform2;
		transform2 = create_transformation_by_id<T, K>( static_cast<transformation_id_t>(cmp_transformation_id));
		cmp_transformations<T,K>(
			n, order, 
			series.get(),
			result,
			transform.get(),
			transform2.get()
		);
		break;
	}
	case test_function_id_t::eval_transform_time_id:
		eval_transform_time<T,K>(
			n, order, 
			series.get(),
			result,
			transform.get()
		);
		break;
	case test_function_id_t::test_all_transforms_id:
	{
		test_all_transforms<T,K>(
			n, order, 
			series.get(),
			result
		);
		break;
	}
	default:
		throw std::domain_error("wrong function_id");
	}

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	#ifdef __unix__
		//it magically works
		//system("read");
		//system("clear");
	#endif
	#ifdef _WIN32
		system("pause"); //pause console till input
		system("cls"); //clear console
	#endif
}