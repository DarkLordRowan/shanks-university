/**
 * @file test_framework.h
 * @brief This file contains the function that provides the framework for testing
 * @authors Bolshakov M.P.
 * @edited by Kreynin R.G., Maximov A.K.
 */

#pragma once

#include <cstdio>
#include <limits.h>
#include <memory>
#include <stdexcept>
#include <string>

#include "../custom_concepts.hpp"
#include "../noise/noise_generator.hpp"
#include "../console/console_IO.hpp"
#include "../console/console_effects.hpp"
#include "../interfaces/transformations_info.hpp"
#include "../interfaces/series_info.hpp"
#include "../interfaces/test_funcs_info.hpp"

#include "test_functions.hpp"


/**
 * @brief Factory functions to create all available items
 */
inline std::vector<std::unique_ptr<ISeriesInfo>> create_series_info() {
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

inline std::vector<std::unique_ptr<ITransformationInfo>> create_transformation_info() {
	std::unique_ptr<ITransformationInfo> temp[] = {
		std::make_unique<BrezinskiThetaTransformationInfo>(),
		std::make_unique<ChangWynnTransformationInfo>(),
		std::make_unique<DrummondDUTransformationInfo>(),
		std::make_unique<DrummondDTTransformationInfo>(),
		std::make_unique<DrummondDTWaveTransformationInfo>(),
		std::make_unique<DrummondDVTransformationInfo>(),
		std::make_unique<DrummondDVWaveTransformationInfo>(),
		std::make_unique<RecurrentDrummondDUTransformationInfo>(),
		std::make_unique<RecurrentDrummondDTTransformationInfo>(),
		std::make_unique<RecurrentDrummondDVTransformationInfo>(),
		std::make_unique<RecurrentDrummondDTWaveTransformationInfo>(),
		std::make_unique<RecurrentDrummondDVWaveTransformationInfo>(),
		std::make_unique<FordSidi2TransformationInfo>(),
		std::make_unique<FordSidi3TransformationInfo>(),
		std::make_unique<LevinSidiLUTransformationInfo>(),
		std::make_unique<LevinSidiLTTransformationInfo>(),
		std::make_unique<LevinSidiLTWaveTransformationInfo>(),
		std::make_unique<LevinSidiLVTransformationInfo>(),
		std::make_unique<LevinSidiLVWaveTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiLUTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiLTTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiLVTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiLTWaveTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiLVWaveTransformationInfo>(),
		std::make_unique<LevinSidiMUTransformationInfo>(),
		std::make_unique<LevinSidiMTTransformationInfo>(),
		std::make_unique<LevinSidiMTWaveTransformationInfo>(),
		std::make_unique<LevinSidiMVTransformationInfo>(),
		std::make_unique<LevinSidiMVWaveTransformationInfo>(),
		std::make_unique<LevinSidiSUTransformationInfo>(),
		std::make_unique<LevinSidiSTTransformationInfo>(),
		std::make_unique<LevinSidiSTWaveTransformationInfo>(),
		std::make_unique<LevinSidiSVTransformationInfo>(),
		std::make_unique<LevinSidiSVWaveTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiSUTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiSTTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiSVTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiSTWaveTransformationInfo>(),
		std::make_unique<RecurrentLevinSidiSVWaveTransformationInfo>(),
		std::make_unique<LubkinWTransformationInfo>(),
		std::make_unique<RichardsonTransformationInfo>(),
		std::make_unique<ShanksTransformationInfo>(),
		std::make_unique<ShanksAlternatingTransformationInfo>(),
		std::make_unique<WenigerTransformationInfo>(),
		std::make_unique<WynnEpsilon1TransformationInfo>(),
		std::make_unique<WynnEpsilon2TransformationInfo>(),
		std::make_unique<WynnEpsilon3TransformationInfo>(),
		std::make_unique<WynnRhoRhoTransformationInfo>(),
		std::make_unique<WynnRhoGeneralizedTransformationInfo>(),
		std::make_unique<WynnRhoGammaRhoTransformationInfo>(),
	};

	return std::vector<std::unique_ptr<ITransformationInfo>>(
		std::make_move_iterator(std::begin(temp)),
		std::make_move_iterator(std::end(temp))
	);
}

inline std::vector<std::unique_ptr<ITestFunctionInfo>> create_test_function_info() {
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
inline void print_series_info() {
	auto all_series = create_series_info();

	std::cout <<
		"Which series' convergence would you like to accelerate?" << '\n' <<
		"List of currently available series:" << '\n';

	for (size_t i = 0; i < all_series.size(); i++) {
		std::cout << i + 1 << " - " << all_series[i]->getName() << '\n';
	}
	std::cout << std::endl;
}

/**
 * @brief prints out all available transformations for testing
 */
inline void print_transformation_info() {
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
inline void print_test_function_info() {
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
inline std::unique_ptr<series_base<T, K>> create_series_by_id(series_id_t id) {
	// This function replace huge switch-case
	auto all_series = create_series_info();

	// Здесь нужно будет сделать mapping от ID к конструктору
	// Пока оставляю старый switch, но можно оптимизировать дальше
	switch (id) {
	case series_id_t::exp_series_id: return std::make_unique<exp_series<T, K>>();
	case series_id_t::cos_series_id: return std::make_unique<cos_series<T, K>>();
	case series_id_t::sin_series_id: return std::make_unique<sin_series<T, K>>();
	case series_id_t::cosh_series_id: return std::make_unique<cosh_series<T, K>>();
	case series_id_t::sinh_series_id: return std::make_unique<sinh_series<T, K>>();
	case series_id_t::bin_series_id: return std::make_unique<bin_series<T, K>>();
	case series_id_t::four_arctan_series_id: return std::make_unique<four_arctan_series<T, K>>();
	case series_id_t::ln1mx_series_id: return std::make_unique<ln1mx_series<T, K>>();
	case series_id_t::mean_sinh_sin_series_id: return std::make_unique<mean_sinh_sin_series<T, K>>();
	case series_id_t::exp_squared_erf_series_id: return std::make_unique<exp_squared_erf_series<T, K>>();
	case series_id_t::half_asin_two_x_series_id: return std::make_unique<half_asin_two_x_series<T, K>>();
	case series_id_t::inverse_1mx_series_id: return std::make_unique<inverse_1mx_series<T, K>>();
	case series_id_t::x_1mx_squared_series_id: return std::make_unique<x_1mx_squared_series<T, K>>();
	case series_id_t::erf_series_id: return std::make_unique<erf_series<T, K>>();
	case series_id_t::m_fact_1mx_mp1_inverse_series_id: return std::make_unique<m_fact_1mx_mp1_inverse_series<T, K>>();
	case series_id_t::inverse_sqrt_1m4x_series_id: return std::make_unique<inverse_sqrt_1m4x_series<T, K>>();
	case series_id_t::one_twelfth_3x2_pi2_series_id: return std::make_unique<one_twelfth_3x2_pi2_series<T, K>>();
	case series_id_t::one_twelfth_x2_pi2_series_id: return std::make_unique<one_twelfth_x2_pi2_series<T, K>>();
	case series_id_t::ln2_series_id: return std::make_unique<ln2_series<T, K>>();
	case series_id_t::one_series_id: return std::make_unique<one_series<T, K>>();
	case series_id_t::minus_one_quarter_series_id: return std::make_unique<minus_one_quarter_series<T, K>>();
	case series_id_t::pi_3_series_id: return std::make_unique<pi_3_series<T, K>>();
	case series_id_t::pi_4_series_id: return std::make_unique<pi_4_series<T, K>>();
	case series_id_t::pi_squared_6_minus_one_series_id: return std::make_unique<pi_squared_6_minus_one_series<T, K>>();
	case series_id_t::three_minus_pi_series_id: return std::make_unique<three_minus_pi_series<T, K>>();
	case series_id_t::one_twelfth_series_id: return std::make_unique<one_twelfth_series<T, K>>();
	case series_id_t::eighth_pi_m_one_third_series_id: return std::make_unique<eighth_pi_m_one_third_series<T, K>>();
	case series_id_t::one_third_pi_squared_m_nine_series_id: return std::make_unique<one_third_pi_squared_m_nine_series<T, K>>();
	case series_id_t::four_ln2_m_3_series_id: return std::make_unique<four_ln2_m_3_series<T, K>>();
	case series_id_t::exp_m_cos_x_sinsin_x_series_id: return std::make_unique<exp_m_cos_x_sinsin_x_series<T, K>>();
	case series_id_t::pi_four_minus_ln2_halfed_series_id: return std::make_unique<pi_four_minus_ln2_halfed_series<T, K>>();
	case series_id_t::five_pi_twelve_series_id: return std::make_unique<five_pi_twelve_series<T, K>>();
	case series_id_t::x_two_series_id: return std::make_unique<x_two_series<T, K>>();
	case series_id_t::pi_six_min_half_series_id: return std::make_unique<pi_six_min_half_series<T, K>>();
	case series_id_t::x_two_throught_squares_series_id: return std::make_unique<x_two_throught_squares_series<T, K>>();
	case series_id_t::minus_one_ned_in_n_series_id: return std::make_unique<minus_one_ned_in_n_series<T, K>>();
	case series_id_t::minus_one_n_fact_n_in_n_series_id: return std::make_unique<minus_one_n_fact_n_in_n_series<T, K>>();
	case series_id_t::ln_x_plus_one_x_minus_one_halfed_series_id: return std::make_unique<ln_x_plus_one_x_minus_one_halfed_series<T, K>>();
	case series_id_t::two_arcsin_square_x_halfed_series_id: return std::make_unique<two_arcsin_square_x_halfed_series<T, K>>();
	case series_id_t::pi_squared_twelve_series_id: return std::make_unique<pi_squared_twelve_series<T, K>>();
	case series_id_t::pi_cubed_32_series_id: return std::make_unique<pi_cubed_32_series<T, K>>();
	case series_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id: return std::make_unique<minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>>();
	case series_id_t::two_ln2_series_id: return std::make_unique<two_ln2_series<T, K>>();
	case series_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series_id: return std::make_unique<pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series<T, K>>();
	case series_id_t::pi_minus_x_2_series_id: return std::make_unique<pi_minus_x_2_series<T, K>>();
	case series_id_t::half_multi_ln_1div2multi1minuscosx_series_id: return std::make_unique<half_multi_ln_1div2multi1minuscosx_series<T, K>>();
	case series_id_t::half_minus_sinx_multi_pi_4_series_id: return std::make_unique<half_minus_sinx_multi_pi_4_series<T, K>>();
	case series_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_series_id: return std::make_unique<ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>>();
	case series_id_t::ln_cosx_series_id: return std::make_unique<ln_cosx_series<T, K>>();
	case series_id_t::ln_sinx_minus_ln_x_series_id: return std::make_unique<ln_sinx_minus_ln_x_series<T, K>>();
	case series_id_t::pi_8_cosx_square_minus_1_div_3_cosx_series_id: return std::make_unique<pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>>();
	case series_id_t::sqrt_oneminussqrtoneminusx_div_x_series_id: return std::make_unique<sqrt_oneminussqrtoneminusx_div_x_series<T, K>>();
	case series_id_t::one_minus_sqrt_1minus4x_div_2x_series_id: return std::make_unique<one_minus_sqrt_1minus4x_div_2x_series<T, K>>();
	case series_id_t::arcsin_x_minus_x_series_id: return std::make_unique<arcsin_x_minus_x_series<T, K>>();
	case series_id_t::pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series_id: return std::make_unique<pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<T, K>>();
	case series_id_t::abs_sin_x_minus_2_div_pi_series_id: return std::make_unique<abs_sin_x_minus_2_div_pi_series<T, K>>();
	case series_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id: return std::make_unique<pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>>();
	case series_id_t::minus_3_div_4_or_x_minus_3_div_4_series_id: return std::make_unique<minus_3_div_4_or_x_minus_3_div_4_series<T, K>>();
	case series_id_t::ten_minus_x_series_id: return std::make_unique<ten_minus_x_series<T, K>>();
	case series_id_t::x_series_id: return std::make_unique<x_series<T, K>>();
	case series_id_t::minus_x_minus_pi_4_or_minus_pi_4_series_id: return std::make_unique<minus_x_minus_pi_4_or_minus_pi_4_series<T, K>>();
	case series_id_t::one_div_two_minus_x_multi_three_plus_x_series_id: return std::make_unique<one_div_two_minus_x_multi_three_plus_x_series<T, K>>();
	case series_id_t::si_x_series_id: return std::make_unique<si_x_series<T, K>>();
	case series_id_t::ci_x_series_id: return std::make_unique<ci_x_series<T, K>>();
	case series_id_t::riemann_zeta_func_series_id: return std::make_unique<riemann_zeta_func_series<T, K>>();
	case series_id_t::riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id: return std::make_unique<riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>>();
	case series_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_series_id: return std::make_unique<xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>>();
	case series_id_t::arcsin_x_series_id: return std::make_unique<arcsin_x_series<T, K>>();
	case series_id_t::arctg_x_series_id: return std::make_unique<arctg_x_series<T, K>>();
	case series_id_t::k_x_series_id: return std::make_unique<k_x_series<T, K>>();
	case series_id_t::e_x_series_id: return std::make_unique<e_x_series<T, K>>();
	case series_id_t::sqrt_1plusx_series_id: return std::make_unique<sqrt_1plusx_series<T, K>>();
	case series_id_t::lambert_W_func_series_id: return std::make_unique<lambert_W_func_series<T, K>>();
	case series_id_t::incomplete_Gamma_func_series_id: return std::make_unique<incomplete_Gamma_func_series<T, K>>();
	case series_id_t::series_with_ln_number1_series_id: return std::make_unique<series_with_ln_number1_series<T, K>>();
	case series_id_t::series_with_ln_number2_series_id: return std::make_unique<series_with_ln_number2_series<T, K>>();
	case series_id_t::pi_series_id: return std::make_unique<pi_series<T, K>>();
	case series_id_t::x_min_sqrt_x_series_id: return std::make_unique<x_min_sqrt_x_series<T, K>>();
	case series_id_t::arctg_x2_series_id: return std::make_unique<arctg_x2_series<T, K>>();
	case series_id_t::ln1px4_series_id: return std::make_unique<ln1px4_series<T, K>>();
	case series_id_t::sin_x2_series_id: return std::make_unique<sin_x2_series<T, K>>();
	case series_id_t::arctg_x3_series_id: return std::make_unique<arctg_x3_series<T, K>>();
	case series_id_t::arcsin_x2_series_id: return std::make_unique<arcsin_x2_series<T, K>>();
	case series_id_t::ln1_m_x2_series_id: return std::make_unique<ln1_m_x2_series<T, K>>();
	case series_id_t::arctanh_x_series_id: return std::make_unique<arctanh_x_series<T, K>>();
	case series_id_t::arcsinh_x_series_id: return std::make_unique<arcsinh_x_series<T, K>>();
	case series_id_t::cos_x2_series_id: return std::make_unique<cos_x2_series<T, K>>();
	case series_id_t::sinh_x2_series_id: return std::make_unique<sinh_x2_series<T, K>>();
	case series_id_t::arctanh_x2_series_id: return std::make_unique<arctanh_x2_series<T, K>>();
	case series_id_t::cos3xmin1_div_xsquare_series_id: return std::make_unique<cos3xmin1_div_xsquare_series<T, K>>();
	case series_id_t::two_degree_x_series_id: return std::make_unique<two_degree_x_series<T, K>>();
	case series_id_t::sqrt_1plusx_min_1_min_x_div_2_series_id: return std::make_unique<sqrt_1plusx_min_1_min_x_div_2_series<T, K>>();
	case series_id_t::ln13_min_ln7_div_7_series_id: return std::make_unique<ln13_min_ln7_div_7_series<T, K>>();
	case series_id_t::one_div_sqrt2_sin_xdivsqrt2_series_id: return std::make_unique<one_div_sqrt2_sin_xdivsqrt2_series<T, K>>();
	case series_id_t::ln_1plusx_div_1plusx2_series_id: return std::make_unique<ln_1plusx_div_1plusx2_series<T, K>>();
	case series_id_t::cos_sqrt_x_series_id: return std::make_unique<cos_sqrt_x_series<T, K>>();
	case series_id_t::ln_1_plus_x3_series_id: return std::make_unique<ln_1_plus_x3_series<T, K>>();
	case series_id_t::x_div_1minx_series_id: return std::make_unique<x_div_1minx_series<T, K>>();
	case series_id_t::x_div_1minx2_series_id: return std::make_unique<x_div_1minx2_series<T, K>>();

	default:
		throw std::domain_error("Series not implemented");
	}
}

/**
 * @brief Helper function to get transformation by ID
 */
template <AcceptedLike T, std::unsigned_integral K>
inline std::unique_ptr<series_acceleration<T, K>>
create_transformation_by_id(transformation_id_t id) {

	switch (id) {
	case transformation_id_t::brezinski_theta_transformation_id		 	  		: { return std::make_unique<brezinski_theta_algorithm<T,K>>(brezinski_theta_algorithm<T,K>()); }
	case transformation_id_t::chang_wynn_transformation_id			 	  		: { return std::make_unique<chang_wynn_algorithm<T,K>>(chang_wynn_algorithm<T,K>()); }
	case transformation_id_t::drummond_d_u_transformation_id			 		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::u_type, false )); }
	case transformation_id_t::drummond_d_t_transformation_id			 		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::t_type, false )); }
	case transformation_id_t::drummond_d_t_wave_transformation_id	 	  		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::t_wave_type, false )); }
	case transformation_id_t::drummond_d_v_transformation_id			 		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::v_type, false )); }
	case transformation_id_t::drummond_d_v_wave_transformation_id	 	  		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::v_wave_type, false )); }
	case transformation_id_t::recurrent_drummond_d_u_transformation_id	  		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::u_type, true )); }
	case transformation_id_t::recurrent_drummond_d_t_transformation_id	  		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::t_type, true )); }
	case transformation_id_t::recurrent_drummond_d_v_transformation_id	  		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::v_type, true )); }
	case transformation_id_t::recurrent_drummond_d_t_wave_transformation_id		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::t_wave_type, true )); }
	case transformation_id_t::recurrent_drummond_d_v_wave_transformation_id		: { return std::make_unique<drummond_d_algorithm<T,K>>(drummond_d_algorithm<T,K>( remainder_type::v_wave_type, true )); }
	case transformation_id_t::ford_sidi_2_transformation_id				  		: { return std::make_unique<ford_sidi_2_algorithm<T,K>>(ford_sidi_2_algorithm<T,K>()); }
	case transformation_id_t::ford_sidi_3_transformation_id				  		: { return std::make_unique<ford_sidi_3_algorithm<T,K>>(ford_sidi_3_algorithm<T,K>()); }
	case transformation_id_t::levin_sidi_l_u_transformation_id			  		: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::u_type, false )); }
	case transformation_id_t::levin_sidi_l_t_transformation_id			  		: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::t_type, false )); }
	case transformation_id_t::levin_sidi_l_t_wave_transformation_id		  		: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::t_wave_type, false )); }
	case transformation_id_t::levin_sidi_l_v_transformation_id			  		: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::v_type, false )); }
	case transformation_id_t::levin_sidi_l_v_wave_transformation_id		  		: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::v_wave_type, false )); }
	case transformation_id_t::recurrent_levin_sidi_l_u_transformation_id	  	: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::u_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_l_t_transformation_id	  	: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::t_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_l_v_transformation_id	  	: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::v_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_l_t_wave_transformation_id	: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::t_wave_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_l_v_wave_transformation_id	: { return std::make_unique<levin_algorithm<T,K>>(levin_algorithm<T,K>( remainder_type::v_wave_type, true )); }
	case transformation_id_t::levin_sidi_m_u_transformation_id					: { return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>( remainder_type::u_type, false )); }
	case transformation_id_t::levin_sidi_m_t_transformation_id					: { return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>( remainder_type::t_type, false )); }
	case transformation_id_t::levin_sidi_m_t_wave_transformation_id				: { return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>( remainder_type::t_wave_type, false )); }
	case transformation_id_t::levin_sidi_m_v_transformation_id					: { return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>( remainder_type::v_type, false )); }
	case transformation_id_t::levin_sidi_m_v_wave_transformation_id				: { return std::make_unique<levin_sidi_m_algorithm<T,K>>(levin_sidi_m_algorithm<T,K>( remainder_type::v_wave_type, false )); }
	case transformation_id_t::levin_sidi_s_u_transformation_id					: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::u_type, false )); }
	case transformation_id_t::levin_sidi_s_t_transformation_id					: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::t_type, false )); }
	case transformation_id_t::levin_sidi_s_t_wave_transformation_id				: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::t_wave_type, false )); }
	case transformation_id_t::levin_sidi_s_v_transformation_id					: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::v_type, false )); }
	case transformation_id_t::levin_sidi_s_v_wave_transformation_id				: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::v_wave_type, false )); }
	case transformation_id_t::recurrent_levin_sidi_s_u_transformation_id		: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::u_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_s_t_transformation_id		: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::t_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_s_v_transformation_id		: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::v_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_s_t_wave_transformation_id	: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::t_wave_type, true )); }
	case transformation_id_t::recurrent_levin_sidi_s_v_wave_transformation_id	: { return std::make_unique<levin_sidi_s_algorithm<T,K>>(levin_sidi_s_algorithm<T,K>( remainder_type::v_wave_type, true )); }
	case transformation_id_t::lubkin_w_transformation_id						: { return std::make_unique<lubkin_w_algorithm<T,K>>(lubkin_w_algorithm<T,K>()); }
	case transformation_id_t::richardson_transformation_id						: { return std::make_unique<richardson_algorithm<T,K>>(richardson_algorithm<T,K>()); }
	case transformation_id_t::shanks_transformation_id							: { return std::make_unique<richardson_algorithm<T,K>>(richardson_algorithm<T,K>()); }
	case transformation_id_t::shanks_alternating_transformation_id				: { return std::make_unique<richardson_algorithm<T,K>>(richardson_algorithm<T,K>()); }
	case transformation_id_t::weniger_transformation_id							: { return std::make_unique<weniger_algorithm<T,K>>(weniger_algorithm<T,K>()); }
	case transformation_id_t::wynn_epsilon_1_transformation_id					: { return std::make_unique<wynn_epsilon_1_algorithm<T,K>>(wynn_epsilon_1_algorithm<T,K>()); }
	case transformation_id_t::wynn_epsilon_2_transformation_id					: { return std::make_unique<wynn_epsilon_2_algorithm<T,K>>(wynn_epsilon_2_algorithm<T,K>()); }
	case transformation_id_t::wynn_epsilon_3_transformation_id					: { return std::make_unique<wynn_epsilon_3_algorithm<T,K>>(wynn_epsilon_3_algorithm<T,K>()); }
	case transformation_id_t::wynn_rho_rho_transformation_id					: { return std::make_unique<wynn_rho_algorithm<T,K>>(wynn_rho_algorithm<T,K>( numerator_type::rho_type )); }
	case transformation_id_t::wynn_rho_generalized_transformation_id			: { return std::make_unique<wynn_rho_algorithm<T,K>>(wynn_rho_algorithm<T,K>( numerator_type::generalized_type )); }
	case transformation_id_t::wynn_rho_gamma_rho_transformation_id				: { return std::make_unique<wynn_rho_algorithm<T,K>>(wynn_rho_algorithm<T,K>( numerator_type::gamma_rho_type )); }
	default: throw std::domain_error("Invalid transformation ID");
	}
}


/**
 * @brief The main testing function
 * This function provides a convenient and interactive way to test out the convergence acceleration of various series
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */

template <AcceptedLike T, std::unsigned_integral K>
inline void main_testing_function()
{

	//choosing series
	print_series_info();
	std::unique_ptr<series_base<T, K>> series;
	series_id_t series_id;

	//-------------------------------------------------------------------------------------//
	bool valid = false;
	bool error_triggered = false;

	while (!valid){

		series_id = console_IO<series_id_t>::input("series_id");

		try {

			series = create_series_by_id<T, K>(series_id);
			valid = true;

		} catch (std::domain_error& e){

			console_effects::clear_lines_up(1 + static_cast<int>(error_triggered));
			std::cout << e.what() << " : " << static_cast<int>(series_id) << "\n";
			error_triggered = true;

		}

	}

	console_effects::clear_lines_up(2 + static_cast<int>(error_triggered));
	
	//-------------------------------------------------------------------------------------//
	std::cout << "\nChosen series id " <<  static_cast<unsigned long int>(series_id) << "\n";

	std::cout << series->get_name() << "\n";
	//choosing x
	std::cout << "Enter x - the argument for the functional series" << '\n';
	T x = console_IO<T>::input();

	T tParam = static_cast<T>(1);
	K kParam = static_cast<K>(1);

	switch(series_id){
		case series_id_t::bin_series_id:{
			std::cout << "for bin series must initialise variable alpha\n";
			tParam = console_IO<T>::input("alpha");
		}
		case series_id_t::incomplete_Gamma_func_series_id:{
			std::cout << "for incomplete Gamma function must initialise variable alpha\n";
			tParam = console_IO<T>::input("alpha");
		}
		case series_id_t::m_fact_1mx_mp1_inverse_series_id:{
			std::cout << "for m factorial 1mx mp1 ... series must initialise variable alpha\n";
			kParam = console_IO<K>::input("m");
		}
	}

	//choosing transformation
	print_transformation_info();
	transformation_id_t transformation_id;
	std::unique_ptr<series_acceleration<T, K>> transform;

	//-------------------------------------------------------------------------------------//
	valid = false;
	error_triggered = false;
	while (!valid){

		transformation_id = console_IO<transformation_id_t>::input("transformation_id");

		try {

			transform = create_transformation_by_id<T, K>(transformation_id);
			valid = true;

		} catch (std::domain_error& e){

			console_effects::clear_lines_up(1 + static_cast<int>(error_triggered));
			std::cout << e.what() << " : " << static_cast<int>(series_id) << "\n";
			error_triggered = true;

		}

	}

	console_effects::clear_lines_up(2 + static_cast<int>(error_triggered));
	
	//-------------------------------------------------------------------------------------//

	std::cout << "transformation : " << transform->get_name() << "\n";
	print_test_function_info();
	test_function_id_t function_id = console_IO<test_function_id_t>::input("function id");



	std::cout << "Enter n and order:" << '\n';
	K n = console_IO<K>::input("n");
	K order = console_IO<K>::input("order");
	series_result<T> result = series->generate_series(x, n + 3 * order + 1, tParam, kParam);

	std::string answer = "ok";
	while(answer != "Y" && answer != "Yes" && answer != "N" && answer != "No"){
		std::cout << "Appply noise [Y]es, [N]o: "; std::cin >> answer;
	}

	if (answer == "Y" || answer == "Yes"){
		
		std::cout << "Noise type: uniform[0], normal[1], poisson[2]\n";
		NoiseType noise_type_to_use = console_IO<NoiseType>::input("noise_type");

		result = jitter<T,T>(result, noise_type_to_use);
	}

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
		transformation_id_t cmp_transformation_id = console_IO<transformation_id_t>::input("transformation id");
		std::unique_ptr<series_acceleration<T, K>> transform2;

		transform2 = create_transformation_by_id<T, K>( cmp_transformation_id);
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

}