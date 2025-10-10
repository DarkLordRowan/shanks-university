#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <math.h>

#include "libs/arbitrary_arithmetics/fprecision.h"
#include "libs/arbitrary_arithmetics/precisioncore.cpp"

#include "methods.hpp"
#include "series.hpp"

PYBIND11_MODULE(pyshanks, m) {

    m.doc() = "pybind11: polymorphic series + all Shanks‑type transformations (double + float_precision";

    py::enum_<remainder_type>(m, "RemainderType")
        .value("u_variant", remainder_type::u_variant)
        .value("t_variant", remainder_type::t_variant)
        .value("v_variant", remainder_type::v_variant)
        .value("t_wave_variant", remainder_type::t_wave_variant)
        .value("v_wave_variant", remainder_type::v_wave_variant)
        .export_values();
    
    py::enum_<numerator_type>(m, "NumeratorType")
        .value("rho_variant", numerator_type::rho_variant)
        .value("generalized_variant", numerator_type::generalized_variant)
        .value("gamma_rho_variant", numerator_type::gamma_rho_variant)
        .export_values();

    using T = double;
    using K = unsigned long long int;

    py::class_<SeriesResult<T>>(m, "SeriesResult")
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readwrite("Sn", &SeriesResult<T>::Sn)
        .def_readwrite("an", &SeriesResult<T>::an);

    py::class_<series_acceleration<T,K>>(m, "SeriesAcceleration")
        .def("printInfo", &series_acceleration<T, K>::print_info)
        .def("__call__", &series_acceleration<T,K>::operator(), py::arg("n"), py::arg("order"), py::arg("data"), py::arg("offset") = static_cast<K>(0));

    //init algos
    py::class_<brezinski_theta_algorithm<T,K>, series_acceleration<T,K>>(m, "BrezinskiThetaAlgorithm")
        .def(py::init<>());

    py::class_<chang_wynn_algorithm<T,K>, series_acceleration<T,K>>(m, "ChangWynnAlgorithm")
        .def(py::init<>());
    
    py::class_<drummond_d_algorithm<T,K>, series_acceleration<T,K>>(m, "DrummondDAlgorithm")
        .def(py::init<remainder_type, bool>(), py::arg("remainder") = remainder_type::t_variant, py::arg("useRecurrentFormula") = false);

    py::class_<ford_sidi_2_algorithm<T,K>, series_acceleration<T,K>>(m, "FordSidi2Algorithm")
        .def(py::init<>());

    py::class_<ford_sidi_3_algorithm<T,K>, series_acceleration<T,K>>(m, "FordSidi3Algorithm")
        .def(py::init<>());

    py::class_<levin_algorithm<T,K>, series_acceleration<T,K>>(m, "LevinLAlgorithm")
        .def(py::init<remainder_type, bool, T>(), py::arg("remainder") = remainder_type::t_variant, py::arg("useRecurrentFormula") = false, py::arg("beta") = static_cast<T>(1)); //ADD
    
    py::class_<levin_sidi_m_algorithm<T,K>, series_acceleration<T,K>>(m, "LevinSidiMAlgorithm")
        .def(py::init<remainder_type, T>(), py::arg("remainder") = remainder_type::t_variant, py::arg("gamma") = static_cast<T>(10)); //ADD

    py::class_<levin_sidi_s_algorithm<T,K>, series_acceleration<T,K>>(m, "LevinSidiSAlgorithm")
        .def(py::init<remainder_type, bool, T>(), py::arg("remainder") = remainder_type::t_variant, py::arg("useRecurrentFormula") = false, py::arg("beta") = static_cast<T>(1)); //ADD

    py::class_<lubkin_w_algorithm<T,K>, series_acceleration<T,K>>(m, "LubkinWAlgorithm")
        .def(py::init<>());

    py::class_<richardson_algorithm<T,K>, series_acceleration<T,K>>(m, "RichardsonAlgorithm")
        .def(py::init<>());

    py::class_<shanks_algorithm<T,K>, series_acceleration<T,K>>(m, "ShanksAlgorithm")
        .def(py::init<>());

    py::class_<shanks_transform_alternating<T,K>, series_acceleration<T,K>>(m, "AlternatingShanksAlgorithm")
        .def(py::init<>());

    py::class_<weniger_algorithm<T,K>, series_acceleration<T,K>>(m, "WenigerAlgorithm")
        .def(py::init<>());

    py::class_<wynn_epsilon_1_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnEpsilon1Algorithm")
        .def(py::init<>());

    py::class_<wynn_epsilon_2_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnEpsilon2Algorithm")
        .def(py::init<>());

    py::class_<wynn_epsilon_3_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnEpsilon3Algorithm")
        .def(py::init<T>(), py::arg("epsilon_threshold") = static_cast<T>(1e-3));

    py::class_<wynn_rho_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnRhoAlgorithm")
        .def(py::init<numerator_type, T, T>(), py::arg("numerator") = remainder_type::t_variant, py::arg("gamma") = static_cast<T>(-1), py::arg("rho") = static_cast<T>(1));


    
    py::class_<series_base<T, K>>(m, "SeriesBase")
        .def("get_x", &series_base<T,K>::get_x)
        .def("get_sum", &series_base<T,K>::get_sum)
        .def("generate_series", &series_base<T,K>::generateSeries, 
            py::arg("x"), 
            py::arg("size"),
            py::arg("doubleParameter") = static_cast<T>(0),
            py::arg("size_tParamteter") = static_cast<K>(0)
        );
    

    py::class_<abs_sin_x_minus_2_div_pi_series<T,K>>(m, "AbsSinXMinus2DivPiSeriesF64").def(py::init<>());
    py::class_<arcsinh_x_series<T,K>>(m, "ArcsinhXSeriesF64").def(py::init<>());
    py::class_<arcsin_x2_series<T,K>>(m, "ArcsinX2SeriesF64").def(py::init<>());
    py::class_<arcsin_x_minus_x_series<T,K>>(m, "ArcsinXMinusXSeriesF64").def(py::init<>());
    py::class_<arcsin_x_series<T,K>>(m, "ArcsinXSeriesF64").def(py::init<>());
    py::class_<arctanh_x2_series<T,K>>(m, "ArctanhX2SeriesF64").def(py::init<>());
    py::class_<arctanh_x_series<T,K>>(m, "ArctanhXSeriesArb").def(py::init<>());
    py::class_<arctg_x2_series<T,K>>(m, "ArctgX2SeriesF64").def(py::init<>());
    py::class_<arctg_x3_series<T,K>>(m, "ArctgX3SeriesF64").def(py::init<>());
    py::class_<arctg_x_series<T,K>>(m, "ArctgXSeriesF64").def(py::init<>());
    py::class_<bin_series<T,K>>(m, "BinSeriesF64").def(py::init<>());
    py::class_<ci_x_series<T,K>>(m, "CiXSeriesF64").def(py::init<>());
    py::class_<cos3xmin1_div_xsquare_series<T,K>>(m, "Cos3xMinus1DivXsquareSeriesF64").def(py::init<>());
    py::class_<cosh_series<T,K>>(m, "CoshSeriesF64").def(py::init<>());
    py::class_<cos_series<T,K>>(m, "CosSeriesF64").def(py::init<>());
    py::class_<cos_sqrt_x_series<T,K>>(m, "CosSqrtXSeriesF64").def(py::init<>());
    py::class_<cos_x2_series<T,K>>(m, "CosX2SeriesF64").def(py::init<>());
    py::class_<eighth_pi_m_one_third_series<T,K>>(m, "EighthPiMOneThirdSeriesF64").def(py::init<>());
    py::class_<erf_series<T,K>>(m, "ErfSeries").def(py::init<>());
    py::class_<exp_m_cos_x_sinsin_x_series<T,K>>(m, "ExpMCosXSinsinXSeriesF64").def(py::init<>());
    py::class_<exp_series<T,K>>(m, "ExpSeriesF64").def(py::init<>());
    py::class_<exp_squared_erf_series<T,K>>(m, "ExpSquaredErfSeriesF64").def(py::init<>());
    py::class_<e_x_series<T,K>>(m, "EXSeriesF64").def(py::init<>());
    py::class_<five_pi_twelve_series<T,K>>(m, "FivePiTwelveSeriesF64").def(py::init<>());
    py::class_<four_arctan_series<T,K>>(m, "FourArctanSeriesF64").def(py::init<>());
    py::class_<four_ln2_m_3_series<T,K>>(m, "FourLn2M3SeriesF64").def(py::init<>());
    py::class_<half_asin_two_x_series<T,K>>(m, "HalfAsinTwoXSeriesF64").def(py::init<>());
    py::class_<half_minus_sinx_multi_pi_4_series<T,K>>(m, "HalfMinusSinxMultiPi4SeriesF64").def(py::init<>());
    py::class_<half_multi_ln_1div2multi1minuscosx_series<T,K>>(m, "HalfMultiLn1Div2Multi1MinusCosxSeriesF64").def(py::init<>());
    py::class_<incomplete_Gamma_func_series<T,K>>(m, "IncompleteGammaFuncSeriesF64").def(py::init<>());
    py::class_<inverse_1mx_series<T,K>>(m, "Inverse1mxSeriesF64").def(py::init<>());
    py::class_<inverse_sqrt_1m4x_series<T,K>>(m, "InvSqrt1m4xSeriesF64").def(py::init<>());
    py::class_<k_x_series<T,K>>(m, "KXSeriesF64").def(py::init<>());
    py::class_<lambert_W_func_series<T,K>>(m, "LambertWFuncSeriesF64").def(py::init<>());
    py::class_<ln13_min_ln7_div_7_series<T,K>>(m, "Ln13MinusLn7Div7SeriesF64").def(py::init<>());
    py::class_<ln1_m_x2_series<T,K>>(m, "Ln1MinusX2SeriesF64").def(py::init<>());
    py::class_<ln1mx_series<T,K>>(m, "Ln1mxSeriesF64").def(py::init<>());
    py::class_<ln_1plussqrt1plusxsquare_minus_ln_2_series<T,K>>(m, "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesF64").def(py::init<>());
    py::class_<ln_1_plus_x3_series<T,K>>(m, "Ln1PlusX3SeriesF64").def(py::init<>());
    py::class_<ln_1plusx_div_1plusx2_series<T,K>>(m, "Ln1plusXDiv1plusX2SeriesF64").def(py::init<>());
    py::class_<ln1px4_series<T,K>>(m, "Ln1px4SeriesF64").def(py::init<>());
    py::class_<ln2_series<T,K>>(m, "Ln2SeriesF64").def(py::init<>());
    py::class_<ln_cosx_series<T,K>>(m, "LnCosxSeriesF64").def(py::init<>());
    py::class_<ln_sinx_minus_ln_x_series<T,K>>(m, "LnSinxMinusLnXSeriesF64").def(py::init<>());
    py::class_<ln_x_plus_one_x_minus_one_halfed_series<T,K>>(m, "LnXPlusOneXMinusOneHalfedSeriesF64").def(py::init<>());
    py::class_<mean_sinh_sin_series<T,K>>(m, "MeanSinhSinSeriesF64").def(py::init<>());
    py::class_<m_fact_1mx_mp1_inverse_series<T,K>>(m, "MFact1mxMp1InverseSeriesF64").def(py::init<>());
    py::class_<minus_3_div_4_or_x_minus_3_div_4_series<T,K>>(m, "Minus3Div4OrXMinus3Div4SeriesF64").def(py::init<>());
    py::class_<minus_one_ned_in_n_series<T,K>>(m, "MinusOneNedInNSeriesF64").def(py::init<>());
    py::class_<minus_one_n_fact_n_in_n_series<T,K>>(m, "MinusOneNFactNInNSeriesF64").def(py::init<>());
    py::class_<minus_one_quarter_series<T,K>>(m, "MinusOneQuarterSeriesF64").def(py::init<>());
    py::class_<minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T,K>>(m, "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesF64").def(py::init<>());
    py::class_<minus_x_minus_pi_4_or_minus_pi_4_series<T,K>>(m, "MinusXMinusPi4OrMinusPi4SeriesF64").def(py::init<>());
    py::class_<one_div_sqrt2_sin_xdivsqrt2_series<T,K>>(m, "OneDivSqrt2SinXDivSqrt2SeriesF64").def(py::init<>());
    py::class_<one_div_two_minus_x_multi_three_plus_x_series<T,K>>(m, "OneDivTwoMinusXMultiThreePlusXSeriesF64").def(py::init<>());
    py::class_<one_minus_sqrt_1minus4x_div_2x_series<T,K>>(m, "OneMinusSqrt1minus4xDiv2xSeriesF64").def(py::init<>());
    py::class_<one_series<T,K>>(m, "OneSeriesF64").def(py::init<>());
    py::class_<one_third_pi_squared_m_nine_series<T,K>>(m, "OneThirdPiSquaredMNineSeriesF64").def(py::init<>());
    py::class_<one_twelfth_3x2_pi2_series<T,K>>(m, "OneTwelfth3x2Pi2SeriesF64").def(py::init<>());
    py::class_<one_twelfth_series<T,K>>(m, "OneTwelfthSeriesF64").def(py::init<>());
    py::class_<one_twelfth_x2_pi2_series<T,K>>(m, "OneTwelfthX2Pi2SeriesF64").def(py::init<>());
    py::class_<pi_3_series<T,K>>(m, "Pi3SeriesF64").def(py::init<>());
    py::class_<pi_4_series<T,K>>(m, "Pi4SeriesF64").def(py::init<>());
    py::class_<pi_8_cosx_square_minus_1_div_3_cosx_series<T,K>>(m, "Pi8CosxSquareMinus1Div3CosxSeriesF64").def(py::init<>());
    py::class_<pi_cubed_32_series<T,K>>(m, "PiCubed32SeriesF64").def(py::init<>());
    py::class_<pi_four_minus_ln2_halfed_series<T,K>>(m, "PiFourMinusLn2HalfedSeriesF64").def(py::init<>());
    py::class_<pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T,K>>(m, "PiMinus3pi4AndPiMinusXMinus3pi4SeriesF64").def(py::init<>());
    py::class_<pi_minus_x_2_series<T,K>>(m, "PiMinusX2SeriesF64").def(py::init<>());
    py::class_<pi_series<T,K>>(m, "PiSeriesF64").def(py::init<>());
    py::class_<pi_six_min_half_series<T,K>>(m, "PiSixMinHalfSeriesF64").def(py::init<>());
    py::class_<pi_squared_6_minus_one_series<T,K>>(m, "PiSquared6MinusOneSeriesF64").def(py::init<>());
    py::class_<pi_squared_twelve_series<T,K>>(m, "PiSquaredTwelveSeriesF64").def(py::init<>());
    py::class_<pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<T,K>>(m,"PiXMinusXSquareAndXSquareMinusThreePiXPlusTwoPiSquareSeriesArb").def(py::init<>());
    py::class_<pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series<T,K>>(m,"PiXMultiE_XpiPlusEMinusXpiDividedE_XpiMinusEMinusXpiMinusOneSeriesF64").def(py::init<>());
    py::class_<riemann_zeta_func_series<T,K>>(m,"RiemannZetaFuncSeriesF64").def(py::init<>());
    py::class_<riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T,K>>(m,"RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesF64").def(py::init<>());
    py::class_<series_with_ln_number1_series<T,K>>(m,"SeriesWithLnNumber1F64").def(py::init<>());
    py::class_<series_with_ln_number2_series<T,K>>(m,"SeriesWithLnNumber2F64").def(py::init<>());
    py::class_<sinh_series<T,K>>(m,"SinhSeriesF64").def(py::init<>());
    py::class_<sinh_x2_series<T,K>>(m,"SinhX2SeriesF64").def(py::init<>());
    py::class_<sin_series<T,K>>(m,"Sin2SeriesF64").def(py::init<>());
    py::class_<sin_x2_series<T,K>>(m,"SinX2SeriesF64").def(py::init<>());
    py::class_<si_x_series<T,K>>(m,"SiXSeriesF64").def(py::init<>());
    py::class_<sqrt_1plusx_min_1_min_x_div_2_series<T,K>>(m,"Sqrt1plusXMinus1MinusXDiv2SeriesF64").def(py::init<>());
    py::class_<sqrt_1plusx_series<T,K>>(m,"Sqrt1plusXSeriesF64").def(py::init<>());
    py::class_<sqrt_oneminussqrtoneminusx_div_x_series<T,K>>(m,"SqrtOneminusSqrtoneminusxDivXSeriesF64").def(py::init<>());
    py::class_<ten_minus_x_series<T,K>>(m,"TenMinusXSeriesF64").def(py::init<>());
    py::class_<three_minus_pi_series<T,K>>(m,"ThreeMinusPiSeriesF64").def(py::init<>());
    py::class_<two_arcsin_square_x_halfed_series<T,K>>(m,"TwoArcsinSquareXHalfedSeriesF64").def(py::init<>());
    py::class_<two_degree_x_series<T,K>>(m,"TwoDegreeXSeriesF64").def(py::init<>());
    py::class_<two_ln2_series<T,K>>(m,"TwoLn2SeriesF64").def(py::init<>());
    py::class_<x_1mx_squared_series<T,K>>(m,"X_1mx_SquaredSeriesF64").def(py::init<>());
    py::class_<x_div_1minx2_series<T,K>>(m,"XDiv1minX2SeriesF64").def(py::init<>());
    py::class_<x_div_1minx_series<T,K>>(m,"XDiv1minXSeriesF64").def(py::init<>());
    py::class_<x_min_sqrt_x_series<T,K>>(m,"XMinSqrtXSeriesF64").def(py::init<>());
    py::class_<x_series<T,K>>(m,"XSeriesF64").def(py::init<>());
    py::class_<xsquareplus3_div_xsquareplus2multix_minus_1_series<T,K>>(m,"Xsquareplus3DivXsquareplus2multixMinus1SeriesF64").def(py::init<>());
    py::class_<x_two_series<T,K>>(m,"XTwoSeriesF64").def(py::init<>());
    py::class_<x_two_throught_squares_series<T,K>>(m,"XTwoThroughtSquaresSeriesF64").def(py::init<>());
}