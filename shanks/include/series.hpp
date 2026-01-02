#ifndef SERIES_HPP
#define SERIES_HPP
#pragma once

#include "custom_concepts.hpp"
#include "series_base.hpp"
#include "series_iterators/series_iterators.hpp"

/**
 * @file series.hpp
 * @brief This file contains enums for all series.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks{ namespace series {

enum class series_id_t {
														   null_series_id,
									   abs_sin_x_minus_2_div_pi_series_id,
													  arcsinh_x_series_id,
													  arcsin_x2_series_id,
											   arcsin_x_minus_x_series_id,
													   arcsin_x_series_id,
													 arctanh_x2_series_id,
													  arctanh_x_series_id,
													   arctg_x2_series_id,
													   arctg_x3_series_id,
														arctg_x_series_id,
															bin_series_id,
														   ci_x_series_id,
										  cos3xmin1_div_xsquare_series_id,
														   cosh_series_id,
															cos_series_id,
													 cos_sqrt_x_series_id,
														 cos_x2_series_id,
										  eighth_pi_m_one_third_series_id,
															erf_series_id,
										   exp_m_cos_x_sinsin_x_series_id,
															exp_series_id,
												exp_squared_erf_series_id,
															e_x_series_id,
												 five_pi_twelve_series_id,
												    four_arctan_series_id,
												   four_ln2_m_3_series_id,
											    half_asin_two_x_series_id,
									 half_minus_sinx_multi_pi_4_series_id,
							 half_multi_ln_1div2multi1minuscosx_series_id,
										  incomplete_Gamma_func_series_id,
												    inverse_1mx_series_id,
											  inverse_sqrt_1m4x_series_id,
															k_x_series_id,
												 lambert_W_func_series_id,
											 ln13_min_ln7_div_7_series_id,
													   ln1_m_x2_series_id,
														  ln1mx_series_id,
							ln_1plussqrt1plusxsquare_minus_ln_2_series_id,
												   ln_1_plus_x3_series_id,
										  ln_1plusx_div_1plusx2_series_id,
														 ln1px4_series_id,
															ln2_series_id,
														ln_cosx_series_id,
											 ln_sinx_minus_ln_x_series_id,
							   ln_x_plus_one_x_minus_one_halfed_series_id,
												  mean_sinh_sin_series_id,
										 m_fact_1mx_mp1_inverse_series_id,
							   minus_3_div_4_or_x_minus_3_div_4_series_id,
											 minus_one_ned_in_n_series_id,
										minus_one_n_fact_n_in_n_series_id,
											  minus_one_quarter_series_id,
			minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id,
							   minus_x_minus_pi_4_or_minus_pi_4_series_id,
									one_div_sqrt2_sin_xdivsqrt2_series_id,
						 one_div_two_minus_x_multi_three_plus_x_series_id,
								 one_minus_sqrt_1minus4x_div_2x_series_id,
															one_series_id,
									one_third_pi_squared_m_nine_series_id,
										    one_twelfth_3x2_pi2_series_id,
													one_twelfth_series_id,
											 one_twelfth_x2_pi2_series_id,
														   pi_3_series_id,
														   pi_4_series_id,
							pi_8_cosx_square_minus_1_div_3_cosx_series_id,
												 	pi_cubed_32_series_id,
									   pi_four_minus_ln2_halfed_series_id,
					  pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id,
												   pi_minus_x_2_series_id,
						    								 pi_series_id,
						    				    pi_six_min_half_series_id,
						    			 pi_squared_6_minus_one_series_id,
						    				  pi_squared_twelve_series_id,
 pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series_id,
pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series_id,
											  riemann_zeta_func_series_id,
				riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id,
				      					 series_with_ln_number1_series_id,
				      					 series_with_ln_number2_series_id,
				      									   sinh_series_id,
				      									sinh_x2_series_id,
				      									    sin_series_id,
				      									 sin_x2_series_id,
				      									   si_x_series_id,
				    			  sqrt_1plusx_min_1_min_x_div_2_series_id,
				    								sqrt_1plusx_series_id,
				    		   sqrt_oneminussqrtoneminusx_div_x_series_id,
				    							    ten_minus_x_series_id,
				    							 three_minus_pi_series_id,
				    				 two_arcsin_square_x_halfed_series_id,
				    							   two_degree_x_series_id,
				    									two_ln2_series_id,
				    							  x_1mx_squared_series_id,
				    							   x_div_1minx2_series_id,
				    								x_div_1minx_series_id,
				    							   x_min_sqrt_x_series_id,
				    								 		  x_series_id,
				    xsquareplus3_div_xsquareplus2multix_minus_1_series_id,
				    					 				  x_two_series_id,
				    					 x_two_throught_squares_series_id,
										 				series_id_t_count
};

template<AcceptedLike T, UnsignedIntLike K>
class abs_sin_x_minus_2_div_pi_series final : public series_base<T,K>{
public:
	explicit abs_sin_x_minus_2_div_pi_series() : series_base<T, K>(
		"abs_sin_x_minus_2_div_pi_series",
		"x must be in (0, 2pi)",
		shanks::iters::series_iterator_id_t::abs_sin_x_minus_2_div_pi_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arcsinh_x_series final : public series_base<T,K>{
public:
	explicit arcsinh_x_series() : series_base<T, K>(
		"arcsinh_x_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::arcsinh_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arcsin_x2_series final : public series_base<T,K>{
public:
	explicit arcsin_x2_series() : series_base<T, K>(
		"arcsin_x2_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::arcsin_x2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arcsin_x_minus_x_series final : public series_base<T,K>{
public:
	explicit arcsin_x_minus_x_series() : series_base<T, K>(
		"arcsin_x_minus_x_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::arcsin_x_minus_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arcsin_x_series final : public series_base<T,K>{
public:
	explicit arcsin_x_series() : series_base<T, K>(
		"arcsin_x_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::arcsin_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arctanh_x2_series final : public series_base<T,K>{
public:
	explicit arctanh_x2_series() : series_base<T, K>(
		"arctanh_x2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::arctanh_x2_iterator_id
	) {};
};
template<AcceptedLike T, UnsignedIntLike K>
class arctanh_x_series final : public series_base<T,K>{
public:
	explicit arctanh_x_series() : series_base<T, K>(
		"arctanh_x_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::arctanh_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arctg_x2_series final : public series_base<T,K>{
public:
	explicit arctg_x2_series() : series_base<T, K>(
		"arctg_x2_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::arctg_x2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arctg_x3_series final : public series_base<T,K>{
public:
	explicit arctg_x3_series() : series_base<T, K>(
		"arctg_x3_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::arctg_x3_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class arctg_x_series final : public series_base<T,K>{
public:
	explicit arctg_x_series() : series_base<T, K>(
		"arctg_x_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::arctg_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class bin_series final : public series_base<T,K>{
public:
	explicit bin_series() : series_base<T, K>(
		"bin_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::bin_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ci_x_series final : public series_base<T,K>{
public:
	explicit ci_x_series() : series_base<T, K>(
		"ci_x_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::ci_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class cos3xmin1_div_xsquare_series final : public series_base<T,K>{
public:
	explicit cos3xmin1_div_xsquare_series() : series_base<T, K>(
		"cos3xmin1_div_xsquare_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::cos3xmin1_div_xsquare_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class cosh_series final : public series_base<T,K>{
public:
	explicit cosh_series() : series_base<T, K>(
		"cosh_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::cosh_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class cos_series final : public series_base<T,K>{
public:
	explicit cos_series() : series_base<T, K>(
		"cos_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::cos_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class cos_sqrt_x_series final : public series_base<T,K>{
public:
	explicit cos_sqrt_x_series() : series_base<T, K>(
		"cos_sqrt_x_series",
		"x must be in (0, +infty)",
		shanks::iters::series_iterator_id_t::cos_sqrt_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class cos_x2_series final : public series_base<T,K>{
public:
	explicit cos_x2_series() : series_base<T, K>(
		"cos_x2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::cos_x2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class eighth_pi_m_one_third_series final : public series_base<T,K>{
public:
	explicit eighth_pi_m_one_third_series() : series_base<T, K>(
		"eighth_pi_m_one_third_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::eighth_pi_m_one_third_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class erf_series final : public series_base<T,K>{
public:
	explicit erf_series() : series_base<T, K>(
		"erf_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::erf_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class exp_m_cos_x_sinsin_x_series final : public series_base<T,K>{
public:
	explicit exp_m_cos_x_sinsin_x_series() : series_base<T, K>(
		"exp_m_cos_x_sinsin_x_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::exp_m_cos_x_sinsin_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class exp_series final : public series_base<T,K>{
public:
	explicit exp_series() : series_base<T, K>(
		"exp_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::exp_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class exp_squared_erf_series final : public series_base<T,K>{
public:
	explicit exp_squared_erf_series() : series_base<T, K>(
		"exp_squared_erf_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::exp_squared_erf_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class e_x_series final : public series_base<T,K>{
public:
	explicit e_x_series() : series_base<T, K>(
		"e_x_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::e_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class five_pi_twelve_series final : public series_base<T,K>{
public:
	explicit five_pi_twelve_series() : series_base<T, K>(
		"five_pi_twelve_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::five_pi_twelve_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class four_arctan_series final : public series_base<T,K>{
public:
	explicit four_arctan_series() : series_base<T, K>(
		"four_arctan_series",
		"x must be in [-1, 1]",
		shanks::iters::series_iterator_id_t::four_arctan_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class four_ln2_m_3_series final : public series_base<T,K>{
public:
	explicit four_ln2_m_3_series() : series_base<T, K>(
		"four_ln2_m_3_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::four_ln2_m_3_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class half_asin_two_x_series final : public series_base<T,K>{
public:
	explicit half_asin_two_x_series() : series_base<T, K>(
		"half_asin_two_x_series",
		"x must be in [-0.5, 0.5]",
		shanks::iters::series_iterator_id_t::half_asin_two_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class half_minus_sinx_multi_pi_4_series final : public series_base<T,K>{
public:
	explicit half_minus_sinx_multi_pi_4_series() : series_base<T, K>(
		"half_minus_sinx_multi_pi_4_series",
		"x must be in [0, pi/2]",
		shanks::iters::series_iterator_id_t::half_minus_sinx_multi_pi_4_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class half_multi_ln_1div2multi1minuscosx_series final : public series_base<T,K>{
public:
	explicit half_multi_ln_1div2multi1minuscosx_series() : series_base<T, K>(
		"half_multi_ln_1div2multi1minuscosx_series",
		"x must be in (-pi, pi)",
		shanks::iters::series_iterator_id_t::half_multi_ln_1div2multi1minuscosx_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class incomplete_Gamma_func_series final : public series_base<T,K>{
public:
	explicit incomplete_Gamma_func_series() : series_base<T, K>(
		"incomplete_Gamma_func_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::incomplete_Gamma_func_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class inverse_1mx_series final : public series_base<T,K>{
public:
	explicit inverse_1mx_series() : series_base<T, K>(
		"inverse_1mx_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::inverse_1mx_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class inverse_sqrt_1m4x_series final : public series_base<T,K>{
public:
	explicit inverse_sqrt_1m4x_series() : series_base<T, K>(
		"inverse_sqrt_1m4x_series",
		"x must be in (-0.25, 0.25)",
		shanks::iters::series_iterator_id_t::inverse_sqrt_1m4x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class k_x_series final : public series_base<T,K>{
public:
	explicit k_x_series() : series_base<T, K>(
		"k_x_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::k_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class lambert_W_func_series final : public series_base<T,K>{
public:
	explicit lambert_W_func_series() : series_base<T, K>(
		"lambert_W_func_series",
		"x must be in (-1/e, 1/e)",
		shanks::iters::series_iterator_id_t::lambert_W_func_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln13_min_ln7_div_7_series final : public series_base<T,K>{
public:
	explicit ln13_min_ln7_div_7_series() : series_base<T, K>(
		"ln13_min_ln7_div_7_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::ln13_min_ln7_div_7_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln1_m_x2_series final : public series_base<T,K>{
public:
	explicit ln1_m_x2_series() : series_base<T, K>(
		"ln1_m_x2_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::ln1_m_x2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln1mx_series final : public series_base<T,K>{
public:
	explicit ln1mx_series() : series_base<T, K>(
		"ln1mx_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::ln1mx_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln_1plussqrt1plusxsquare_minus_ln_2_series final : public series_base<T,K>{
public:
	explicit ln_1plussqrt1plusxsquare_minus_ln_2_series() : series_base<T, K>(
		"ln_1plussqrt1plusxsquare_minus_ln_2_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln_1_plus_x3_series final : public series_base<T,K>{
public:
	explicit ln_1_plus_x3_series() : series_base<T, K>(
		"ln_1_plus_x3_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::ln_1_plus_x3_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln_1plusx_div_1plusx2_series final : public series_base<T,K>{
public:
	explicit ln_1plusx_div_1plusx2_series() : series_base<T, K>(
		"ln_1plusx_div_1plusx2_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::ln_1plusx_div_1plusx2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln1px4_series final : public series_base<T,K>{
public:
	explicit ln1px4_series() : series_base<T, K>(
		"ln1px4_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::ln1px4_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln2_series final : public series_base<T,K>{
public:
	explicit ln2_series() : series_base<T, K>(
		"ln2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::ln2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln_cosx_series final : public series_base<T,K>{
public:
	explicit ln_cosx_series() : series_base<T, K>(
		"ln_cosx_series",
		"x must be in (-pi/2, pi/2)",
		shanks::iters::series_iterator_id_t::ln_cosx_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln_sinx_minus_ln_x_series final : public series_base<T,K>{
public:
	explicit ln_sinx_minus_ln_x_series() : series_base<T, K>(
		"ln_sinx_minus_ln_x_series",
		"x must be in [0, pi)",
		shanks::iters::series_iterator_id_t::ln_sinx_minus_ln_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ln_x_plus_one_x_minus_one_halfed_series final : public series_base<T,K>{
public:
	explicit ln_x_plus_one_x_minus_one_halfed_series() : series_base<T, K>(
		"ln_x_plus_one_x_minus_one_halfed_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::ln_x_plus_one_x_minus_one_halfed_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class mean_sinh_sin_series final : public series_base<T,K>{
public:
	explicit mean_sinh_sin_series() : series_base<T, K>(
		"mean_sinh_sin_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::mean_sinh_sin_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class m_fact_1mx_mp1_inverse_series final : public series_base<T,K>{
public:
	explicit m_fact_1mx_mp1_inverse_series() : series_base<T, K>(
		"m_fact_1mx_mp1_inverse_series",
		"x must be in (-1, 1)",
		shanks::iters::series_iterator_id_t::m_fact_1mx_mp1_inverse_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class minus_3_div_4_or_x_minus_3_div_4_series final : public series_base<T,K>{
public:
	explicit minus_3_div_4_or_x_minus_3_div_4_series() : series_base<T, K>(
		"minus_3_div_4_or_x_minus_3_div_4_series",
		"x must be in (-3, 3)",
		shanks::iters::series_iterator_id_t::minus_3_div_4_or_x_minus_3_div_4_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class minus_one_ned_in_n_series final : public series_base<T,K>{
public:
	explicit minus_one_ned_in_n_series() : series_base<T, K>(
		"minus_one_ned_in_n_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::minus_one_ned_in_n_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class minus_one_n_fact_n_in_n_series final : public series_base<T,K>{
public:
	explicit minus_one_n_fact_n_in_n_series() : series_base<T, K>(
		"minus_one_n_fact_n_in_n_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::minus_one_n_fact_n_in_n_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class minus_one_quarter_series final : public series_base<T,K>{
public:
	explicit minus_one_quarter_series() : series_base<T, K>(
		"minus_one_quarter_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::minus_one_quarter_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class minus_three_plus_ln3_three_devided_two_plus_two_ln2_series final : public series_base<T,K>{
public:
	explicit minus_three_plus_ln3_three_devided_two_plus_two_ln2_series() : series_base<T, K>(
		"minus_three_plus_ln3_three_devided_two_plus_two_ln2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class minus_x_minus_pi_4_or_minus_pi_4_series final : public series_base<T,K>{
public:
	explicit minus_x_minus_pi_4_or_minus_pi_4_series() : series_base<T, K>(
		"minus_x_minus_pi_4_or_minus_pi_4_series",
		"x must be in (-pi, pi)",
		shanks::iters::series_iterator_id_t::minus_x_minus_pi_4_or_minus_pi_4_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_div_sqrt2_sin_xdivsqrt2_series final : public series_base<T,K>{
public:
	explicit one_div_sqrt2_sin_xdivsqrt2_series() : series_base<T, K>(
		"one_div_sqrt2_sin_xdivsqrt2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::one_div_sqrt2_sin_xdivsqrt2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_div_two_minus_x_multi_three_plus_x_series final : public series_base<T,K>{
public:
	explicit one_div_two_minus_x_multi_three_plus_x_series() : series_base<T, K>(
		"one_div_two_minus_x_multi_three_plus_x_series",
		"x must be in (-2, 2)",
		shanks::iters::series_iterator_id_t::one_div_two_minus_x_multi_three_plus_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_minus_sqrt_1minus4x_div_2x_series final : public series_base<T,K>{
public:
	explicit one_minus_sqrt_1minus4x_div_2x_series() : series_base<T, K>(
		"one_minus_sqrt_1minus4x_div_2x_series",
		"x must be in [-0.25, 0)U(0, 0.25]",
		shanks::iters::series_iterator_id_t::one_minus_sqrt_1minus4x_div_2x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_series final : public series_base<T,K>{
public:
	explicit one_series() : series_base<T, K>(
		"one_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::one_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_third_pi_squared_m_nine_series final : public series_base<T,K>{
public:
	explicit one_third_pi_squared_m_nine_series() : series_base<T, K>(
		"one_third_pi_squared_m_nine_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::one_third_pi_squared_m_nine_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_3x2_pi2_series final : public series_base<T,K>{
public:
	explicit one_twelfth_3x2_pi2_series() : series_base<T, K>(
		"one_twelfth_3x2_pi2_series",
		"x must be in (-pi, pi)",
		shanks::iters::series_iterator_id_t::one_twelfth_3x2_pi2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_series final : public series_base<T,K>{
public:
	explicit one_twelfth_series() : series_base<T, K>(
		"one_twelfth_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::one_twelfth_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_x2_pi2_series final : public series_base<T,K>{
public:
	explicit one_twelfth_x2_pi2_series() : series_base<T, K>(
		"one_twelfth_x2_pi2_series",
		"x must be in (-pi, pi)",
		shanks::iters::series_iterator_id_t::one_twelfth_x2_pi2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>	
class pi_3_series final : public series_base<T,K>{
public:
	explicit pi_3_series() : series_base<T, K>(
		"pi_3_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_3_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_4_series final : public series_base<T,K>{
public:
	explicit pi_4_series() : series_base<T, K>(
		"pi_4_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_4_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_8_cosx_square_minus_1_div_3_cosx_series final : public series_base<T,K>{
public:
	explicit pi_8_cosx_square_minus_1_div_3_cosx_series() : series_base<T, K>(
		"pi_8_cosx_square_minus_1_div_3_cosx_series",
		"x must be in [-pi/2, pi/2]",
		shanks::iters::series_iterator_id_t::pi_8_cosx_square_minus_1_div_3_cosx_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_cubed_32_series final : public series_base<T,K>{
public:
	explicit pi_cubed_32_series() : series_base<T, K>(
		"pi_cubed_32_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_cubed_32_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_four_minus_ln2_halfed_series final : public series_base<T,K>{
public:
	explicit pi_four_minus_ln2_halfed_series() : series_base<T, K>(
		"pi_four_minus_ln2_halfed_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_four_minus_ln2_halfed_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series final : public series_base<T,K>{
public:
	explicit pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() : series_base<T, K>(
		"pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series",
		"x must be in (-pi, pi]",
		shanks::iters::series_iterator_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_minus_x_2_series final : public series_base<T,K>{
public:
	explicit pi_minus_x_2_series() : series_base<T, K>(
		"pi_minus_x_2_series",
		"x must be in (0, pi)",
		shanks::iters::series_iterator_id_t::pi_minus_x_2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_series final : public series_base<T,K>{
public:
	explicit pi_series() : series_base<T, K>(
		"pi_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_six_min_half_series final : public series_base<T,K>{
public:
	explicit pi_six_min_half_series() : series_base<T, K>(
		"pi_six_min_half_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_six_min_half_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_squared_6_minus_one_series final : public series_base<T,K>{
public:
	explicit pi_squared_6_minus_one_series() : series_base<T, K>(
		"pi_squared_6_minus_one_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_squared_6_minus_one_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_squared_twelve_series final : public series_base<T,K>{
public:
	explicit pi_squared_twelve_series() : series_base<T, K>(
		"pi_squared_twelve_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_squared_twelve_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series final : public series_base<T,K>{
public:
	explicit pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series() : series_base<T, K>(
		"pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series",
		"x must be in (0, 2pi)",
		shanks::iters::series_iterator_id_t::pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series final : public series_base<T,K>{
public:
	explicit pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series() : series_base<T, K>(
		"pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_series final : public series_base<T,K>{
public:
	explicit riemann_zeta_func_series() : series_base<T, K>(
		"riemann_zeta_func_series",
		"Re(x) must be > 1",
		shanks::iters::series_iterator_id_t::riemann_zeta_func_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series final : public series_base<T,K>{
public:
	explicit riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series() : series_base<T, K>(
		"riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series",
		"Re(x) must be > 2",
		shanks::iters::series_iterator_id_t::riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number1_series final : public series_base<T,K>{
public:
	explicit series_with_ln_number1_series() : series_base<T, K>(
		"series_with_ln_number1_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::series_with_ln_number1_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number2_series final : public series_base<T,K>{
public:
	explicit series_with_ln_number2_series() : series_base<T, K>(
		"series_with_ln_number2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::series_with_ln_number2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class sinh_series final : public series_base<T,K>{
public:
	explicit sinh_series() : series_base<T, K>(
		"sinh_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::sinh_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class sinh_x2_series final : public series_base<T,K>{
public:
	explicit sinh_x2_series() : series_base<T, K>(
		"sinh_x2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::sinh_x2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class sin_series final : public series_base<T,K>{
public:
	explicit sin_series() : series_base<T, K>(
		"sin_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::sin_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class sin_x2_series final : public series_base<T,K>{
public:
	explicit sin_x2_series() : series_base<T, K>(
		"sin_x2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::sin_x2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class si_x_series final : public series_base<T,K>{
public:
	explicit si_x_series() : series_base<T, K>(
		"si_x_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::si_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_min_1_min_x_div_2_series final : public series_base<T,K>{
public:
	explicit sqrt_1plusx_min_1_min_x_div_2_series() : series_base<T, K>(
		"sqrt_1plusx_min_1_min_x_div_2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::sqrt_1plusx_min_1_min_x_div_2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_series final : public series_base<T,K>{
public:
	explicit sqrt_1plusx_series() : series_base<T, K>(
		"sqrt_1plusx_series",
		"x must be in [-1,1]",
		shanks::iters::series_iterator_id_t::sqrt_1plusx_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class sqrt_oneminussqrtoneminusx_div_x_series final : public series_base<T,K>{
public:
	explicit sqrt_oneminussqrtoneminusx_div_x_series() : series_base<T, K>(
		"sqrt_oneminussqrtoneminusx_div_x_series",
		"x must be in (-1,0)U(0,1)",
		shanks::iters::series_iterator_id_t::sqrt_oneminussqrtoneminusx_div_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class ten_minus_x_series final : public series_base<T,K>{
public:
	explicit ten_minus_x_series() : series_base<T, K>(
		"ten_minus_x_series",
		"x must be in (5,15)",
		shanks::iters::series_iterator_id_t::ten_minus_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class three_minus_pi_series final : public series_base<T,K>{
public:
	explicit three_minus_pi_series() : series_base<T, K>(
		"three_minus_pi_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::three_minus_pi_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class two_arcsin_square_x_halfed_series final : public series_base<T,K>{
public:
	explicit two_arcsin_square_x_halfed_series() : series_base<T, K>(
		"two_arcsin_square_x_halfed_series",
		"x must be in [-2,2]",
		shanks::iters::series_iterator_id_t::two_arcsin_square_x_halfed_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class two_degree_x_series final : public series_base<T,K>{
public:
	explicit two_degree_x_series() : series_base<T, K>(
		"two_degree_x_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::two_degree_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class two_ln2_series final : public series_base<T,K>{
public:
	explicit two_ln2_series() : series_base<T, K>(
		"two_ln2_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::two_ln2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class x_1mx_squared_series final : public series_base<T,K>{
public:
	explicit x_1mx_squared_series() : series_base<T, K>(
		"x_1mx_squared_series",
		"x must be in (-1,1)",
		shanks::iters::series_iterator_id_t::x_1mx_squared_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class x_div_1minx2_series final : public series_base<T,K>{
public:
	explicit x_div_1minx2_series() : series_base<T, K>(
		"x_div_1minx2_series",
		"x must be in (-1,1)",
		shanks::iters::series_iterator_id_t::x_div_1minx2_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class x_div_1minx_series final : public series_base<T,K>{
public:
	explicit x_div_1minx_series() : series_base<T, K>(
		"x_div_1minx_series",
		"x must be in (-1,1)",
		shanks::iters::series_iterator_id_t::x_div_1minx_iterator_id
	) {};
};
template<AcceptedLike T, UnsignedIntLike K>
class x_min_sqrt_x_series final : public series_base<T,K>{
public:
	explicit x_min_sqrt_x_series() : series_base<T, K>(
		"x_min_sqrt_x_series",
		"x must be in (0,2)",
		shanks::iters::series_iterator_id_t::x_min_sqrt_x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class x_series final : public series_base<T,K>{
public:
	explicit x_series() : series_base<T, K>(
		"x_series",
		"x must be in [-1,1]",
		shanks::iters::series_iterator_id_t::x_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class xsquareplus3_div_xsquareplus2multix_minus_1_series final : public series_base<T,K>{
public:
	explicit xsquareplus3_div_xsquareplus2multix_minus_1_series() : series_base<T, K>(
		"xsquareplus3_div_xsquareplus2multix_minus_1_series",
		"x must be in (0,2)",
		shanks::iters::series_iterator_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class x_two_series final : public series_base<T,K>{
public:
	explicit x_two_series() : series_base<T, K>(
		"x_two_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::x_two_iterator_id
	) {};
};

template<AcceptedLike T, UnsignedIntLike K>
class x_two_throught_squares_series final : public series_base<T,K>{
public:
	explicit x_two_throught_squares_series() : series_base<T, K>(
		"x_two_throught_squares_series",
		"x is not finite",
		shanks::iters::series_iterator_id_t::x_two_throught_squares_iterator_id
	) {};
};

} // namespace shanks::series
} // namespace shanks

#endif