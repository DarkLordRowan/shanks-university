
#ifndef SERIES_ITERATORS_HPP
#define SERIES_ITERATORS_HPP
#pragma once

/**
 * @file series_iterators.hpp
 * @brief This file aggregates all available series iterators and defines an ID enumeration for them.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Enumeration of all supported series iterators.
 *
 * Each ID corresponds to a specific mathematical series implementation.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
enum class series_iterator_id_t {
    null_iterator_id,
#define SERIES_ENTRY(snake, camel) snake##_iterator_id,
#define SERIES_LAST(snake, camel) snake##_iterator_id,
#define SERIES_ENTRY_ARGS(snake, camel, a1, a2) snake##_iterator_id,
#define SERIES_LAST_ARGS(snake, camel, a1, a2) snake##_iterator_id,
#include "series_registry.def"
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS
    series_iterator_id_t_count
};

}  // namespace series
}  // namespace shanks
// Include all specific iterator definitions
#include "series_iterators/abs_sin_x_minus_2_div_pi_iterator.hpp"
#include "series_iterators/arcsin_x2_iterator.hpp"
#include "series_iterators/arcsin_x_iterator.hpp"
#include "series_iterators/arcsin_x_minus_x_iterator.hpp"
#include "series_iterators/arcsinh_x_iterator.hpp"
#include "series_iterators/arctan_x2_iterator.hpp"
#include "series_iterators/arctan_x3_iterator.hpp"
#include "series_iterators/arctan_x_iterator.hpp"
#include "series_iterators/arctanh_x2_iterator.hpp"
#include "series_iterators/arctanh_x_iterator.hpp"
#include "series_iterators/bessel_first_kind.hpp"
#include "series_iterators/bin_iterator.hpp"
#include "series_iterators/ci_x_iterator.hpp"
#include "series_iterators/cos3xmin1_div_xsquare_iterator.hpp"
#include "series_iterators/cos_iterator.hpp"
#include "series_iterators/cos_sqrt_x_iterator.hpp"
#include "series_iterators/cos_x2_iterator.hpp"
#include "series_iterators/cosh_iterator.hpp"
#include "series_iterators/e_x_iterator.hpp"
#include "series_iterators/eighth_pi_m_one_third_iterator.hpp"
#include "series_iterators/erf_iterator.hpp"
#include "series_iterators/euler_mascheroni_iterator.hpp"
#include "series_iterators/exp_iterator.hpp"
#include "series_iterators/exp_m_cos_x_sinsin_x_iterator.hpp"
#include "series_iterators/exp_squared_erf_iterator.hpp"
#include "series_iterators/five_pi_twelve_iterator.hpp"
#include "series_iterators/four_arctan_iterator.hpp"
#include "series_iterators/four_ln2_m_3_iterator.hpp"
#include "series_iterators/half_asin_two_x_iterator.hpp"
#include "series_iterators/half_minus_sinx_multi_pi_4_iterator.hpp"
#include "series_iterators/half_multi_ln_1div2multi1minuscosx_iterator.hpp"
#include "series_iterators/incomplete_Gamma_func_iterator.hpp"
#include "series_iterators/integral1_iterator.hpp"
#include "series_iterators/inverse_1mx_iterator.hpp"
#include "series_iterators/inverse_sqrt_1m4x_iterator.hpp"
#include "series_iterators/k_x_iterator.hpp"
#include "series_iterators/lambert_W_func_iterator.hpp"
#include "series_iterators/legendre1_iterator.hpp"
#include "series_iterators/ln13_min_ln7_div_7_iterator.hpp"
#include "series_iterators/ln1_m_x2_iterator.hpp"
#include "series_iterators/ln1mx_iterator.hpp"
#include "series_iterators/ln1px4_iterator.hpp"
#include "series_iterators/ln2_iterator.hpp"
#include "series_iterators/ln_1_plus_x3_iterator.hpp"
#include "series_iterators/ln_1plussqrt1plusxsquare_minus_ln_2_iterator.hpp"
#include "series_iterators/ln_1plusx_div_1plusx2_iterator.hpp"
#include "series_iterators/ln_cosx_iterator.hpp"
#include "series_iterators/ln_sinx_minus_ln_x_iterator.hpp"
#include "series_iterators/ln_x_plus_one_x_minus_one_halfed_iterator.hpp"
#include "series_iterators/m_fact_1mx_mp1_inverse_iterator.hpp"
#include "series_iterators/mean_sinh_sin_iterator.hpp"
#include "series_iterators/minus_3_div_4_or_x_minus_3_div_4_iterator.hpp"
#include "series_iterators/minus_one_n_fact_n_in_n_iterator.hpp"
#include "series_iterators/minus_one_ned_in_n_iterator.hpp"
#include "series_iterators/minus_one_quarter_iterator.hpp"
#include "series_iterators/minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator.hpp"
#include "series_iterators/minus_x_minus_pi_4_or_minus_pi_4_iterator.hpp"
#include "series_iterators/natural_series_iterator.hpp"
#include "series_iterators/one_div_sqrt2_sin_xdivsqrt2_iterator.hpp"
#include "series_iterators/one_div_two_minus_x_multi_three_plus_x_iterator.hpp"
#include "series_iterators/one_iterator.hpp"
#include "series_iterators/one_minus_sqrt_1minus4x_div_2x_iterator.hpp"
#include "series_iterators/one_third_pi_squared_m_nine_iterator.hpp"
#include "series_iterators/one_twelfth_3x2_pi2_iterator.hpp"
#include "series_iterators/one_twelfth_iterator.hpp"
#include "series_iterators/one_twelfth_x2_pi2_iterator.hpp"
#include "series_iterators/pi_3_iterator.hpp"
#include "series_iterators/pi_4_iterator.hpp"
#include "series_iterators/pi_8_cosx_square_minus_1_div_3_cosx_iterator.hpp"
#include "series_iterators/pi_cubed_32_iterator.hpp"
#include "series_iterators/pi_four_minus_ln2_halfed_iterator.hpp"
#include "series_iterators/pi_iterator.hpp"
#include "series_iterators/pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator.hpp"
#include "series_iterators/pi_minus_x_2_iterator.hpp"
#include "series_iterators/pi_six_min_half_iterator.hpp"
#include "series_iterators/pi_squared_6_minus_one_iterator.hpp"
#include "series_iterators/pi_squared_twelve_iterator.hpp"
#include "series_iterators/pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator.hpp"
#include "series_iterators/pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator.hpp"
#include "series_iterators/riemann_zeta_func_iterator.hpp"
#include "series_iterators/riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator.hpp"
#include "series_iterators/rump_seq1_iterator.hpp"
#include "series_iterators/rump_seq2_iterator.hpp"
#include "series_iterators/rump_seq3_iterator.hpp"
#include "series_iterators/rump_seq4_iterator.hpp"
#include "series_iterators/rump_seq5_iterator.hpp"
#include "series_iterators/rump_seq6_iterator.hpp"
#include "series_iterators/rump_seq7_iterator.hpp"
#include "series_iterators/si_x_iterator.hpp"
#include "series_iterators/sin_iterator.hpp"
#include "series_iterators/sin_x2_iterator.hpp"
#include "series_iterators/sinh_iterator.hpp"
#include "series_iterators/sinh_x2_iterator.hpp"
#include "series_iterators/sqrt_1plusx_iterator.hpp"
#include "series_iterators/sqrt_1plusx_min_1_min_x_div_2_iterator.hpp"
#include "series_iterators/sqrt_oneminussqrtoneminusx_div_x_iterator.hpp"
#include "series_iterators/ten_minus_x_iterator.hpp"
#include "series_iterators/three_minus_pi_iterator.hpp"
#include "series_iterators/two_arcsin_square_x_halfed_iterator.hpp"
#include "series_iterators/two_degree_x_iterator.hpp"
#include "series_iterators/two_ln2_iterator.hpp"
#include "series_iterators/x_1mx_squared_iterator.hpp"
#include "series_iterators/x_div_1minx2_iterator.hpp"
#include "series_iterators/x_div_1minx_iterator.hpp"
#include "series_iterators/x_iterator.hpp"
#include "series_iterators/x_min_sqrt_x_iterator.hpp"
#include "series_iterators/x_two_iterator.hpp"
#include "series_iterators/x_two_throught_squares_iterator.hpp"
#include "series_iterators/xsquareplus3_div_xsquareplus2multix_minus_1_iterator.hpp"

#endif
