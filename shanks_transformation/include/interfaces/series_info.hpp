#ifndef SERIES_INFO_INTERFACES_HPP
#define SERIES_INFO_INTERFACES_HPP

#include <string>
#include "../series.hpp"

/**
 * @brief Interface for series information
*/
class ISeriesInfo {
public:

	virtual ~ISeriesInfo();

	virtual series_id_t getId() const = 0;
	virtual std::string getName() const = 0;

};

class ExpSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class CosSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class SinSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class CoshSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class SinhSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class BinSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Four_arctanSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln1mxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Mean_sinh_sinSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Exp_squared_erfSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Half_asin_two_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Inverse_1mxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class X_1mx_squaredSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class ErfSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class M_fact_1mx_mp1_inverseSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Inverse_sqrt_1m4xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class One_twelfth_3x2_pi2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class X_twelfth_x2_pi2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class OneSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Minus_one_quarterSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_squared_6_minus_oneSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Three_minus_piSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class One_twelfthSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Eighth_pi_m_one_thirdSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class One_third_pi_squared_m_nineSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Four_ln2_m_3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Exp_m_cos_x_sinsin_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_four_minus_ln2_halfedSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Five_pi_twelveSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class X_twoSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_six_min_halfSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class X_two_throught_squaresSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Minus_one_ned_in_nSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Minus_one_n_fact_n_in_nSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln_x_plus_one_x_minus_one_halfedSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Two_arcsin_square_x_halfedSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_squared_twelveSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_cubed_32SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Minus_three_plus_ln3_three_devided_two_plus_two_ln2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Two_ln2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_oneSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_minus_x_2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Half_multi_ln_1div2multi1minuscosxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Half_minus_sinx_multi_pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln_1plussqrt1plusxsquare_minus_ln_2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln_cosxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln_sinx_minus_ln_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_8_cosx_square_minus_1_div_3_cosxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Sqrt_oneminussqrtoneminusx_div_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class One_minus_sqrt_1minus4x_div_2xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arcsin_x_minus_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_squareSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Abs_sin_x_minus_2_div_piSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Minus_3_div_4_or_x_minus_3_div_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ten_minus_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class XSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Minus_x_minus_pi_4_or_minus_pi_4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class One_div_two_minus_x_multi_three_plus_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Si_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ci_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Riemann_zeta_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Riemann_zeta_func_xmin1_div_Riemann_zeta_func_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Xsquareplus3_div_xsquareplus2multix_minus_1SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arcsin_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arctg_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class K_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class E_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Sqrt_1plusxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Lambert_W_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Incomplete_Gamma_funcSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Series_with_ln_number1SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Series_with_ln_number2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class PiSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class X_min_sqrt_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arctan_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln1px4SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Sin_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arctan_x3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arcsin_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln1_m_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arctanh_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arcsinh_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Cos_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Sinh_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Arctanh_x2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Cos3xmin1_div_xsqareSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Two_degree_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Sqrt_1plusx_min_1_min_x_div_2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln13_min_ln7_div_7SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class One_div_sqrt2_sin_xdivsqrt2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln_1plusx_div_1plusx2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Cos_sqrt_xSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class Ln_1_plus_x3SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class X_div_1minxSeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

class X_div_1minx2SeriesInfo : public ISeriesInfo {
public:
	series_id_t getId() const override;
	std::string getName() const override;
};

#endif