#include "../../include/interfaces/series_info.hpp"

/**
 * @file series_info.cpp
 * @brief This file contains the implementation of the metadata retrieval for all mathematical series.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Virtual destructor for the series information interface.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
ISeriesInfo::~ISeriesInfo() {}

// Retrieval of unique IDs for each supported series
shanks::series::series_id_t                                                  Abs_sin_x_minus_2_div_piSeriesInfo::getId() const { return shanks::series::series_id_t::abs_sin_x_minus_2_div_pi_series_id; }
shanks::series::series_id_t                                                                 Arcsinh_xSeriesInfo::getId() const { return shanks::series::series_id_t::arcsinh_x_series_id; }
shanks::series::series_id_t                                                                 Arcsin_x2SeriesInfo::getId() const { return shanks::series::series_id_t::arcsin_x2_series_id; }
shanks::series::series_id_t                                                          Arcsin_x_minus_xSeriesInfo::getId() const { return shanks::series::series_id_t::arcsin_x_minus_x_series_id; }
shanks::series::series_id_t                                                                  Arcsin_xSeriesInfo::getId() const { return shanks::series::series_id_t::arcsin_x_series_id; }
shanks::series::series_id_t                                                                Arctanh_x2SeriesInfo::getId() const { return shanks::series::series_id_t::arctanh_x2_series_id; }
shanks::series::series_id_t                                                                 Arctanh_xSeriesInfo::getId() const { return shanks::series::series_id_t::arctanh_x_series_id; }
shanks::series::series_id_t                                                                 Arctan_x2SeriesInfo::getId() const { return shanks::series::series_id_t::arctg_x2_series_id; }
shanks::series::series_id_t                                                                 Arctan_x3SeriesInfo::getId() const { return shanks::series::series_id_t::arctg_x3_series_id; }
shanks::series::series_id_t                                                                   Arctg_xSeriesInfo::getId() const { return shanks::series::series_id_t::arctg_x_series_id; }
shanks::series::series_id_t                                                                       BinSeriesInfo::getId() const { return shanks::series::series_id_t::bin_series_id; }
shanks::series::series_id_t                                                                      Ci_xSeriesInfo::getId() const { return shanks::series::series_id_t::ci_x_series_id; }
shanks::series::series_id_t                                                      Cos3xmin1_div_xsqareSeriesInfo::getId() const { return shanks::series::series_id_t::cos3xmin1_div_xsquare_series_id; }
shanks::series::series_id_t                                                                      CoshSeriesInfo::getId() const { return shanks::series::series_id_t::cosh_series_id; }
shanks::series::series_id_t                                                                       CosSeriesInfo::getId() const { return shanks::series::series_id_t::cos_series_id; }
shanks::series::series_id_t                                                                Cos_sqrt_xSeriesInfo::getId() const { return shanks::series::series_id_t::cos_sqrt_x_series_id; }
shanks::series::series_id_t                                                                    Cos_x2SeriesInfo::getId() const { return shanks::series::series_id_t::cos_x2_series_id; }
shanks::series::series_id_t                                                     Eighth_pi_m_one_thirdSeriesInfo::getId() const { return shanks::series::series_id_t::eighth_pi_m_one_third_series_id; }
shanks::series::series_id_t                                                                       ErfSeriesInfo::getId() const { return shanks::series::series_id_t::erf_series_id; }
shanks::series::series_id_t                                                      Exp_m_cos_x_sinsin_xSeriesInfo::getId() const { return shanks::series::series_id_t::exp_m_cos_x_sinsin_x_series_id; }
shanks::series::series_id_t                                                                       ExpSeriesInfo::getId() const { return shanks::series::series_id_t::exp_series_id; }
shanks::series::series_id_t                                                           Exp_squared_erfSeriesInfo::getId() const { return shanks::series::series_id_t::exp_squared_erf_series_id; }
shanks::series::series_id_t                                                                       E_xSeriesInfo::getId() const { return shanks::series::series_id_t::e_x_series_id; }
shanks::series::series_id_t                                                            Five_pi_twelveSeriesInfo::getId() const { return shanks::series::series_id_t::five_pi_twelve_series_id; }
shanks::series::series_id_t                                                               Four_arctanSeriesInfo::getId() const { return shanks::series::series_id_t::four_arctan_series_id; }
shanks::series::series_id_t                                                              Four_ln2_m_3SeriesInfo::getId() const { return shanks::series::series_id_t::four_ln2_m_3_series_id; }
shanks::series::series_id_t                                                           Half_asin_two_xSeriesInfo::getId() const { return shanks::series::series_id_t::half_asin_two_x_series_id; }
shanks::series::series_id_t                                                Half_minus_sinx_multi_pi_4SeriesInfo::getId() const { return shanks::series::series_id_t::half_minus_sinx_multi_pi_4_series_id; }
shanks::series::series_id_t                                        Half_multi_ln_1div2multi1minuscosxSeriesInfo::getId() const { return shanks::series::series_id_t::half_multi_ln_1div2multi1minuscosx_series_id; }
shanks::series::series_id_t                                                     Incomplete_Gamma_funcSeriesInfo::getId() const { return shanks::series::series_id_t::incomplete_Gamma_func_series_id; }
shanks::series::series_id_t                                                               Inverse_1mxSeriesInfo::getId() const { return shanks::series::series_id_t::inverse_1mx_series_id; }
shanks::series::series_id_t                                                         Inverse_sqrt_1m4xSeriesInfo::getId() const { return shanks::series::series_id_t::inverse_sqrt_1m4x_series_id; }
shanks::series::series_id_t                                                                       K_xSeriesInfo::getId() const { return shanks::series::series_id_t::k_x_series_id; }
shanks::series::series_id_t                                                            Lambert_W_funcSeriesInfo::getId() const { return shanks::series::series_id_t::lambert_W_func_series_id; }
shanks::series::series_id_t                                                        Ln13_min_ln7_div_7SeriesInfo::getId() const { return shanks::series::series_id_t::ln13_min_ln7_div_7_series_id; }
shanks::series::series_id_t                                                                  Ln1_m_x2SeriesInfo::getId() const { return shanks::series::series_id_t::ln1_m_x2_series_id; }
shanks::series::series_id_t                                                                     Ln1mxSeriesInfo::getId() const { return shanks::series::series_id_t::ln1mx_series_id; }
shanks::series::series_id_t                                       Ln_1plussqrt1plusxsquare_minus_ln_2SeriesInfo::getId() const { return shanks::series::series_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_series_id; }
shanks::series::series_id_t                                                              Ln_1_plus_x3SeriesInfo::getId() const { return shanks::series::series_id_t::ln_1_plus_x3_series_id; }
shanks::series::series_id_t                                                     Ln_1plusx_div_1plusx2SeriesInfo::getId() const { return shanks::series::series_id_t::ln_1plusx_div_1plusx2_series_id; }
shanks::series::series_id_t                                                                    Ln1px4SeriesInfo::getId() const { return shanks::series::series_id_t::ln1px4_series_id; }
shanks::series::series_id_t                                                                       Ln2SeriesInfo::getId() const { return shanks::series::series_id_t::ln2_series_id; }
shanks::series::series_id_t                                                                   Ln_cosxSeriesInfo::getId() const { return shanks::series::series_id_t::ln_cosx_series_id; }
shanks::series::series_id_t                                                        Ln_sinx_minus_ln_xSeriesInfo::getId() const { return shanks::series::series_id_t::ln_sinx_minus_ln_x_series_id; }
shanks::series::series_id_t                                          Ln_x_plus_one_x_minus_one_halfedSeriesInfo::getId() const { return shanks::series::series_id_t::ln_x_plus_one_x_minus_one_halfed_series_id; }
shanks::series::series_id_t                                                             Mean_sinh_sinSeriesInfo::getId() const { return shanks::series::series_id_t::mean_sinh_sin_series_id; }
shanks::series::series_id_t                                                    M_fact_1mx_mp1_inverseSeriesInfo::getId() const { return shanks::series::series_id_t::m_fact_1mx_mp1_inverse_series_id; }
shanks::series::series_id_t                                          Minus_3_div_4_or_x_minus_3_div_4SeriesInfo::getId() const { return shanks::series::series_id_t::minus_3_div_4_or_x_minus_3_div_4_series_id; }
shanks::series::series_id_t                                                        Minus_one_ned_in_nSeriesInfo::getId() const { return shanks::series::series_id_t::minus_one_ned_in_n_series_id; }
shanks::series::series_id_t                                                   Minus_one_n_fact_n_in_nSeriesInfo::getId() const { return shanks::series::series_id_t::minus_one_n_fact_n_in_n_series_id; }
shanks::series::series_id_t                                                         Minus_one_quarterSeriesInfo::getId() const { return shanks::series::series_id_t::minus_one_quarter_series_id; }
shanks::series::series_id_t                       Minus_three_plus_ln3_three_devided_two_plus_two_ln2SeriesInfo::getId() const { return shanks::series::series_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series_id; }
shanks::series::series_id_t                                          Minus_x_minus_pi_4_or_minus_pi_4SeriesInfo::getId() const { return shanks::series::series_id_t::minus_x_minus_pi_4_or_minus_pi_4_series_id; }
shanks::series::series_id_t                                               One_div_sqrt2_sin_xdivsqrt2SeriesInfo::getId() const { return shanks::series::series_id_t::one_div_sqrt2_sin_xdivsqrt2_series_id; }
shanks::series::series_id_t                                    One_div_two_minus_x_multi_three_plus_xSeriesInfo::getId() const { return shanks::series::series_id_t::one_div_two_minus_x_multi_three_plus_x_series_id; }
shanks::series::series_id_t                                            One_minus_sqrt_1minus4x_div_2xSeriesInfo::getId() const { return shanks::series::series_id_t::one_minus_sqrt_1minus4x_div_2x_series_id; }
shanks::series::series_id_t                                                                       OneSeriesInfo::getId() const { return shanks::series::series_id_t::one_series_id; }
shanks::series::series_id_t                                               One_third_pi_squared_m_nineSeriesInfo::getId() const { return shanks::series::series_id_t::one_third_pi_squared_m_nine_series_id; }
shanks::series::series_id_t                                                       One_twelfth_3x2_pi2SeriesInfo::getId() const { return shanks::series::series_id_t::one_twelfth_3x2_pi2_series_id; }
shanks::series::series_id_t                                                               One_twelfthSeriesInfo::getId() const { return shanks::series::series_id_t::one_twelfth_series_id; }
shanks::series::series_id_t                                                        One_twelfth_x2_pi2SeriesInfo::getId() const { return shanks::series::series_id_t::one_twelfth_x2_pi2_series_id; }
shanks::series::series_id_t                                                                      Pi_3SeriesInfo::getId() const { return shanks::series::series_id_t::pi_3_series_id; }
shanks::series::series_id_t                                                                      Pi_4SeriesInfo::getId() const { return shanks::series::series_id_t::pi_4_series_id; }
shanks::series::series_id_t                                       Pi_8_cosx_square_minus_1_div_3_cosxSeriesInfo::getId() const { return shanks::series::series_id_t::pi_8_cosx_square_minus_1_div_3_cosx_series_id; }
shanks::series::series_id_t                                                               Pi_cubed_32SeriesInfo::getId() const { return shanks::series::series_id_t::pi_cubed_32_series_id; }
shanks::series::series_id_t                                                  Pi_four_minus_ln2_halfedSeriesInfo::getId() const { return shanks::series::series_id_t::pi_four_minus_ln2_halfed_series_id; }
shanks::series::series_id_t                                 Pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4SeriesInfo::getId() const { return shanks::series::series_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series_id; }
shanks::series::series_id_t                                                              Pi_minus_x_2SeriesInfo::getId() const { return shanks::series::series_id_t::pi_minus_x_2_series_id; }
shanks::series::series_id_t                                                                        PiSeriesInfo::getId() const { return shanks::series::series_id_t::pi_series_id; }
shanks::series::series_id_t                                                           Pi_six_min_halfSeriesInfo::getId() const { return shanks::series::series_id_t::pi_six_min_half_series_id; }
shanks::series::series_id_t                                                    Pi_squared_6_minus_oneSeriesInfo::getId() const { return shanks::series::series_id_t::pi_squared_6_minus_one_series_id; }
shanks::series::series_id_t                                                         Pi_squared_twelveSeriesInfo::getId() const { return shanks::series::series_id_t::pi_squared_twelve_series_id; }
shanks::series::series_id_t      Pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_squareSeriesInfo::getId() const { return shanks::series::series_id_t::pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series_id; }
shanks::series::series_id_t Pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_oneSeriesInfo::getId() const { return shanks::series::series_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series_id; }
shanks::series::series_id_t                                                         Riemann_zeta_funcSeriesInfo::getId() const { return shanks::series::series_id_t::riemann_zeta_func_series_id; }
shanks::series::series_id_t                           Riemann_zeta_func_xmin1_div_Riemann_zeta_func_xSeriesInfo::getId() const { return shanks::series::series_id_t::riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series_id; }
shanks::series::series_id_t                                                    Series_with_ln_number1SeriesInfo::getId() const { return shanks::series::series_id_t::series_with_ln_number1_series_id; }
shanks::series::series_id_t                                                    Series_with_ln_number2SeriesInfo::getId() const { return shanks::series::series_id_t::series_with_ln_number2_series_id; }
shanks::series::series_id_t                                                                      SinhSeriesInfo::getId() const { return shanks::series::series_id_t::sinh_series_id; }
shanks::series::series_id_t                                                                   Sinh_x2SeriesInfo::getId() const { return shanks::series::series_id_t::sinh_x2_series_id; }
shanks::series::series_id_t                                                                       SinSeriesInfo::getId() const { return shanks::series::series_id_t::sin_series_id; }
shanks::series::series_id_t                                                                    Sin_x2SeriesInfo::getId() const { return shanks::series::series_id_t::sin_x2_series_id; }
shanks::series::series_id_t                                                                      Si_xSeriesInfo::getId() const { return shanks::series::series_id_t::si_x_series_id; }
shanks::series::series_id_t                                             Sqrt_1plusx_min_1_min_x_div_2SeriesInfo::getId() const { return shanks::series::series_id_t::sqrt_1plusx_min_1_min_x_div_2_series_id; }
shanks::series::series_id_t                                                               Sqrt_1plusxSeriesInfo::getId() const { return shanks::series::series_id_t::sqrt_1plusx_series_id; }
shanks::series::series_id_t                                          Sqrt_oneminussqrtoneminusx_div_xSeriesInfo::getId() const { return shanks::series::series_id_t::sqrt_oneminussqrtoneminusx_div_x_series_id; }
shanks::series::series_id_t                                                               Ten_minus_xSeriesInfo::getId() const { return shanks::series::series_id_t::ten_minus_x_series_id; }
shanks::series::series_id_t                                                            Three_minus_piSeriesInfo::getId() const { return shanks::series::series_id_t::three_minus_pi_series_id; }
shanks::series::series_id_t                                                Two_arcsin_square_x_halfedSeriesInfo::getId() const { return shanks::series::series_id_t::two_arcsin_square_x_halfed_series_id; }
shanks::series::series_id_t                                                              Two_degree_xSeriesInfo::getId() const { return shanks::series::series_id_t::two_degree_x_series_id; }
shanks::series::series_id_t                                                                   Two_ln2SeriesInfo::getId() const { return shanks::series::series_id_t::two_ln2_series_id; }
shanks::series::series_id_t                                                             X_1mx_squaredSeriesInfo::getId() const { return shanks::series::series_id_t::x_1mx_squared_series_id; }
shanks::series::series_id_t                                                              X_div_1minx2SeriesInfo::getId() const { return shanks::series::series_id_t::x_div_1minx2_series_id; }
shanks::series::series_id_t                                                               X_div_1minxSeriesInfo::getId() const { return shanks::series::series_id_t::x_div_1minx_series_id; }
shanks::series::series_id_t                                                              X_min_sqrt_xSeriesInfo::getId() const { return shanks::series::series_id_t::x_min_sqrt_x_series_id; }
shanks::series::series_id_t                                                                         XSeriesInfo::getId() const { return shanks::series::series_id_t::x_series_id; }
shanks::series::series_id_t                               Xsquareplus3_div_xsquareplus2multix_minus_1SeriesInfo::getId() const { return shanks::series::series_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_series_id; }
shanks::series::series_id_t                                                                     X_twoSeriesInfo::getId() const { return shanks::series::series_id_t::x_two_series_id; }
shanks::series::series_id_t                                                    X_two_throught_squaresSeriesInfo::getId() const { return shanks::series::series_id_t::x_two_throught_squares_series_id; }


// Retrieval of descriptive names for each supported series
std::string                                                  Abs_sin_x_minus_2_div_piSeriesInfo::getName() const { return "abs_sin_x_minus_2_div_pi_series";}
std::string                                                                 Arcsinh_xSeriesInfo::getName() const { return "arcsinh_x_series";}
std::string                                                                 Arcsin_x2SeriesInfo::getName() const { return "arcsin_x2_series";}
std::string                                                          Arcsin_x_minus_xSeriesInfo::getName() const { return "arcsin_x_minus_x_series";}
std::string                                                                  Arcsin_xSeriesInfo::getName() const { return "arcsin_x_series";}
std::string                                                                Arctanh_x2SeriesInfo::getName() const { return "arctanh_x2_series";}
std::string                                                                 Arctanh_xSeriesInfo::getName() const { return "artanh_x_series";}
std::string                                                                 Arctan_x2SeriesInfo::getName() const { return "arctan_x2_series";}
std::string                                                                 Arctan_x3SeriesInfo::getName() const { return "arctan_x3_series";}
std::string                                                                   Arctg_xSeriesInfo::getName() const { return "arctg_x_series";}
std::string                                                                       BinSeriesInfo::getName() const { return "bin_series";}
std::string                                                                      Ci_xSeriesInfo::getName() const { return "Ci_x_series";}
std::string                                                      Cos3xmin1_div_xsqareSeriesInfo::getName() const { return "cos3xmin1_div_xsqare_series";}
std::string                                                                      CoshSeriesInfo::getName() const { return "cosh_series";}
std::string                                                                       CosSeriesInfo::getName() const { return "cos_series";}
std::string                                                                Cos_sqrt_xSeriesInfo::getName() const { return "cos_sqrt_x_series";}
std::string                                                                    Cos_x2SeriesInfo::getName() const { return "cos_x2_series";}
std::string                                                     Eighth_pi_m_one_thirdSeriesInfo::getName() const { return "eighth_pi_m_one_third_series";}
std::string                                                                       ErfSeriesInfo::getName() const { return "exp_squared_erf_series";}
std::string                                                      Exp_m_cos_x_sinsin_xSeriesInfo::getName() const { return "exp_m_cos_x_sinsin_x_series";}
std::string                                                                       ExpSeriesInfo::getName() const { return "exp_series";}
std::string                                                           Exp_squared_erfSeriesInfo::getName() const { return "exp_squared_erf_series";}
std::string                                                                       E_xSeriesInfo::getName() const { return "E_x_series";}
std::string                                                            Five_pi_twelveSeriesInfo::getName() const { return "five_pi_twelve_series";}
std::string                                                               Four_arctanSeriesInfo::getName() const { return "four_arctan_series";}
std::string                                                              Four_ln2_m_3SeriesInfo::getName() const { return "four_ln2_m_3_series";}
std::string                                                           Half_asin_two_xSeriesInfo::getName() const { return "half_asin_two_x_series";}
std::string                                                Half_minus_sinx_multi_pi_4SeriesInfo::getName() const { return "half_minus_sinx_multi_pi_4_series";}
std::string                                        Half_multi_ln_1div2multi1minuscosxSeriesInfo::getName() const { return "half_multi_ln_1div2multi1minuscosx_series";}
std::string                                                     Incomplete_Gamma_funcSeriesInfo::getName() const { return "Incomplete_Gamma_func_series";}
std::string                                                               Inverse_1mxSeriesInfo::getName() const { return "inverse_1mx_series";}
std::string                                                         Inverse_sqrt_1m4xSeriesInfo::getName() const { return "inverse_sqrt_1m4x_series";}
std::string                                                                       K_xSeriesInfo::getName() const { return "K_x_series";}
std::string                                                            Lambert_W_funcSeriesInfo::getName() const { return "Lambert_W_func_series";}
std::string                                                        Ln13_min_ln7_div_7SeriesInfo::getName() const { return "ln13_min_ln7_div_7_series";}
std::string                                                                  Ln1_m_x2SeriesInfo::getName() const { return "ln1_m_x2_series";}
std::string                                                                     Ln1mxSeriesInfo::getName() const { return "ln1mx_series";}
std::string                                       Ln_1plussqrt1plusxsquare_minus_ln_2SeriesInfo::getName() const { return "ln_1plussqrt1plusxsquare_minus_ln_2_series";}
std::string                                                              Ln_1_plus_x3SeriesInfo::getName() const { return "ln_1_plus_x3_series";}
std::string                                                     Ln_1plusx_div_1plusx2SeriesInfo::getName() const { return "ln_1plusx_div_1plusx2_series";}
std::string                                                                    Ln1px4SeriesInfo::getName() const { return "ln1px4_series";}
std::string                                                                       Ln2SeriesInfo::getName() const { return "ln2_series";}
std::string                                                                   Ln_cosxSeriesInfo::getName() const { return "ln_cosx_series";}
std::string                                                        Ln_sinx_minus_ln_xSeriesInfo::getName() const { return "ln_sinx_minus_ln_x_series";}
std::string                                          Ln_x_plus_one_x_minus_one_halfedSeriesInfo::getName() const { return "ln_x_plus_one_x_minus_one_halfed_series";}
std::string                                                             Mean_sinh_sinSeriesInfo::getName() const { return "mean_sinh_sin_series";}
std::string                                                    M_fact_1mx_mp1_inverseSeriesInfo::getName() const { return "m_fact_1mx_mp1_inverse_series";}
std::string                                          Minus_3_div_4_or_x_minus_3_div_4SeriesInfo::getName() const { return "minus_3_div_4_or_x_minus_3_div_4_series";}
std::string                                                        Minus_one_ned_in_nSeriesInfo::getName() const { return "minus_one_ned_in_n_series";}
std::string                                                   Minus_one_n_fact_n_in_nSeriesInfo::getName() const { return "minus_one_n_fact_n_in_n_series";}
std::string                                                         Minus_one_quarterSeriesInfo::getName() const { return "minus_one_quarter_series";}
std::string                       Minus_three_plus_ln3_three_devided_two_plus_two_ln2SeriesInfo::getName() const { return "minus_three_plus_ln3_three_devided_two_plus_two_ln2_series";}
std::string                                          Minus_x_minus_pi_4_or_minus_pi_4SeriesInfo::getName() const { return "minus_x_minus_pi_4_or_minus_pi_4_series";}
std::string                                               One_div_sqrt2_sin_xdivsqrt2SeriesInfo::getName() const { return "one_div_sqrt2_sin_xdivsqrt2_series";}
std::string                                    One_div_two_minus_x_multi_three_plus_xSeriesInfo::getName() const { return "one_div_two_minus_x_multi_three_plus_x_series";}
std::string                                            One_minus_sqrt_1minus4x_div_2xSeriesInfo::getName() const { return "one_minus_sqrt_1minus4x_div_2x_series";}
std::string                                                                       OneSeriesInfo::getName() const { return "one_series";}
std::string                                               One_third_pi_squared_m_nineSeriesInfo::getName() const { return "one_third_pi_squared_m_nine_series";}
std::string                                                       One_twelfth_3x2_pi2SeriesInfo::getName() const { return "one_twelfth_3x2_pi2_series";}
std::string                                                               One_twelfthSeriesInfo::getName() const { return "one_twelfth_series";}
std::string                                                        One_twelfth_x2_pi2SeriesInfo::getName() const { return "one_twelfth_x2_pi2_series";}
std::string                                                                      Pi_3SeriesInfo::getName() const { return "pi_3_series";}
std::string                                                                      Pi_4SeriesInfo::getName() const { return "pi_4_series";}
std::string                                       Pi_8_cosx_square_minus_1_div_3_cosxSeriesInfo::getName() const { return "pi_8_cosx_square_minus_1_div_3_cosx_series";}
std::string                                                               Pi_cubed_32SeriesInfo::getName() const { return "pi_cubed_32_series";}
std::string                                                  Pi_four_minus_ln2_halfedSeriesInfo::getName() const { return "pi_four_minus_ln2_halfed_series";}
std::string                                 Pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4SeriesInfo::getName() const { return "pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series";}
std::string                                                              Pi_minus_x_2SeriesInfo::getName() const { return "pi_minus_x_2_series";}
std::string                                                                        PiSeriesInfo::getName() const { return "pi_series";}
std::string                                                           Pi_six_min_halfSeriesInfo::getName() const { return "pi_six_min_half_series";}
std::string                                                    Pi_squared_6_minus_oneSeriesInfo::getName() const { return "pi_squared_6_minus_one_series";}
std::string                                                         Pi_squared_twelveSeriesInfo::getName() const { return "pi_squared_twelve_series";}
std::string      Pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_squareSeriesInfo::getName() const { return "pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series";}
std::string Pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_oneSeriesInfo::getName() const { return "pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series";}
std::string                                                         Riemann_zeta_funcSeriesInfo::getName() const { return "Riemann_zeta_func_series";}
std::string                           Riemann_zeta_func_xmin1_div_Riemann_zeta_func_xSeriesInfo::getName() const { return "Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series";}
std::string                                                    Series_with_ln_number1SeriesInfo::getName() const { return "Series_with_ln_number1_series";}
std::string                                                    Series_with_ln_number2SeriesInfo::getName() const { return "Series_with_ln_number2_series";}
std::string                                                                      SinhSeriesInfo::getName() const { return "sinh_series";}
std::string                                                                   Sinh_x2SeriesInfo::getName() const { return "sinh_x2_series";}
std::string                                                                       SinSeriesInfo::getName() const { return "sin_series";}
std::string                                                                    Sin_x2SeriesInfo::getName() const { return "sin_x2_series";}
std::string                                                                      Si_xSeriesInfo::getName() const { return "Si_x_series";}
std::string                                             Sqrt_1plusx_min_1_min_x_div_2SeriesInfo::getName() const { return "sqrt_1plusx_min_1_min_x_div_2_series";}
std::string                                                               Sqrt_1plusxSeriesInfo::getName() const { return "sqrt_1plusx_series";}
std::string                                          Sqrt_oneminussqrtoneminusx_div_xSeriesInfo::getName() const { return "sqrt_oneminussqrtoneminusx_div_x_series";}
std::string                                                               Ten_minus_xSeriesInfo::getName() const { return "ten_minus_x_series";}
std::string                                                            Three_minus_piSeriesInfo::getName() const { return "three_minus_pi_series";}
std::string                                                Two_arcsin_square_x_halfedSeriesInfo::getName() const { return "two_arcsin_square_x_halfed_series";}
std::string                                                              Two_degree_xSeriesInfo::getName() const { return "two_degree_x_series";}
std::string                                                                   Two_ln2SeriesInfo::getName() const { return "two_ln2_series";}
std::string                                                             X_1mx_squaredSeriesInfo::getName() const { return "x_1mx_squared_series";}
std::string                                                              X_div_1minx2SeriesInfo::getName() const { return "x_div_1minx2_series";}
std::string                                                               X_div_1minxSeriesInfo::getName() const { return "x_div_1minx_series";}
std::string                                                              X_min_sqrt_xSeriesInfo::getName() const { return "x_min_sqrt_x_series";}
std::string                                                                         XSeriesInfo::getName() const { return "x_series";}
std::string                               Xsquareplus3_div_xsquareplus2multix_minus_1SeriesInfo::getName() const { return "xsquareplus3_div_xsquareplus2multix_minus_1_series";}
std::string                                                                     X_twoSeriesInfo::getName() const { return "x_two_series";}
std::string                                                    X_two_throught_squaresSeriesInfo::getName() const { return "x_two_throught_squares_series";}