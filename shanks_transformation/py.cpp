/**
 * @file py.cpp
 * @brief pybind11 bindings with support for double and arbitrary-precision float_precision
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "libs/arbitrary_arithmetics/complexprecision.h"
#include "libs/arbitrary_arithmetics/fprecision.h"
#include "libs/arbitrary_arithmetics/precisioncore.cpp"

#include "series.hpp"
#include "methods.hpp"

namespace py = pybind11;

using K = std::size_t;

template <template<class,class> class Impl>
struct W { template<class X, class Y> using type = Impl<X,Y>; };

template<AcceptedLike T>
struct binder{
    static void bind_all(py::module_& m, const std::string& suffix);
};


template <AcceptedLike T>
void binder<T>::bind_all(py::module_& m, const std::string& suffix){
    using MSeriesBase = series_base<T, K>;
    using MSeriesAcceleration = series_acceleration<T, K>;

    auto name = [&](const char* base) { return std::string(base) + suffix; };

    py::class_<series_result<T>>(m, name("SeriesResult").c_str())
        .def(py::init<>())
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readwrite("Sn", &series_result<T>::Sn)
        .def_readwrite("an", &series_result<T>::an);

    // SeriesBase
    py::class_<MSeriesBase>(m, name("SeriesBase").c_str())
        .def("get_x", &MSeriesBase::get_x)
        .def("get_sum", &MSeriesBase::get_sum)
        .def("get_name", &MSeriesBase::get_name)
        .def("generateSeries",
             &MSeriesBase::generate_series,
             py::arg("x"),
             py::arg("vecSize"),
             py::arg("addTParameter") = static_cast<T>(1),
             py::arg("addKParameter") = static_cast<K>(1));

    // Helper: bind a concrete no-arg series Impl<T,KIndex> derived from Base
    // BS = Bind Series
    auto bs = [&](int _i, auto tmpl, const char* basename) {
        using ImplTK = typename decltype(tmpl)::template type<T, K>;
        py::class_<ImplTK, MSeriesBase>(m, name(basename).c_str()).def(py::init<>());
    };

    bs(1,  W<abs_sin_x_minus_2_div_pi_series>{}, "AbsSinXMinus2DivPiSeries");
    bs(2,  W<arcsin_x2_series>{}, "ArcsinX2Series");
    bs(3,  W<arcsin_x_minus_x_series>{}, "ArcsinXMinusXSeries");
    bs(4,  W<arcsin_x_series>{}, "ArcsinXSeries");
    bs(5,  W<arcsinh_x_series>{}, "ArcsinhXSeries");
    bs(6,  W<arctanh_x2_series>{}, "ArctanhX2Series");
    bs(7,  W<arctanh_x_series>{}, "ArctanhXSeries");
    bs(8,  W<arctg_x2_series>{}, "ArctgX2Series");
    bs(9,  W<arctg_x3_series>{}, "ArctgX3Series");
    bs(10, W<arctg_x_series>{}, "ArctgXSeries");
    bs(11, W<bin_series>{}, "BinSeries");
    bs(12, W<ci_x_series>{}, "CiXSeries");
    bs(13, W<cos3xmin1_div_xsquare_series>{}, "Cos3xMinus1DivXsquareSeries");
    bs(14, W<cos_series>{}, "CosSeries");
    bs(15, W<cos_sqrt_x_series>{}, "CosSqrtXSeries");
    bs(16, W<cos_x2_series>{}, "CosX2Series");
    bs(17, W<cosh_series>{}, "CoshSeries");
    bs(18, W<e_x_series>{}, "EXSeries");
    bs(19, W<eighth_pi_m_one_third_series>{}, "EighthPiMOneThirdSeries");
    bs(20, W<erf_series>{}, "ErfSeries");
    bs(21, W<exp_m_cos_x_sinsin_x_series>{}, "ExpMCosXSinsinXSeries");
    bs(22, W<exp_series>{}, "ExpSeries");
    bs(23, W<exp_squared_erf_series>{}, "ExpSquaredErfSeries");
    bs(24, W<five_pi_twelve_series>{}, "FivePiTwelveSeries");
    bs(25, W<four_arctan_series>{}, "FourArctanSeries");
    bs(26, W<four_ln2_m_3_series>{}, "FourLn2M3Series");
    bs(27, W<half_asin_two_x_series>{}, "HalfAsinTwoXSeries");
    bs(28, W<half_minus_sinx_multi_pi_4_series>{}, "HalfMinusSinxMultiPi4Series");
    bs(29, W<half_multi_ln_1div2multi1minuscosx_series>{}, "HalfMultiLn1Div2Multi1MinusCosxSeries");
    bs(30, W<incomplete_Gamma_func_series>{}, "IncompleteGammaFuncSeries");
    bs(31, W<inverse_1mx_series>{}, "Inverse1mxSeries");
    bs(32, W<inverse_sqrt_1m4x_series>{}, "InvSqrt1m4xSeries");
    // bs(33, W<Ja_x_series>{}, "JaXSeries");
    bs(34, W<k_x_series>{}, "KXSeries");
    bs(35, W<lambert_W_func_series>{}, "LambertWFuncSeries");
    bs(36, W<ln1_m_x2_series>{}, "Ln1MinusX2Series");
    bs(37, W<ln1mx_series>{}, "Ln1mxSeries");
    bs(38, W<ln1px4_series>{}, "Ln1px4Series");
    bs(39, W<ln2_series>{}, "Ln2Series");
    bs(40, W<ln13_min_ln7_div_7_series>{}, "Ln13MinusLn7Div7Series");
    bs(41, W<ln_1_plus_x3_series>{}, "Ln1PlusX3Series");
    bs(42, W<ln_1plussqrt1plusxsquare_minus_ln_2_series>{}, "Ln1PlusSqrt1PlusXsquareMinusLn2Series");
    bs(43, W<ln_1plusx_div_1plusx2_series>{}, "Ln1plusXDiv1plusX2Series");
    bs(44, W<ln_cosx_series>{}, "LnCosxSeries");
    bs(45, W<ln_sinx_minus_ln_x_series>{}, "LnSinxMinusLnXSeries");
    bs(46, W<ln_x_plus_one_x_minus_one_halfed_series>{}, "LnXPlusOneXMinusOneHalfedSeries");
    bs(47, W<m_fact_1mx_mp1_inverse_series>{}, "MFact1mxMp1InverseSeries");
    bs(48, W<mean_sinh_sin_series>{}, "MeanSinhSinSeries");
    bs(49, W<minus_3_div_4_or_x_minus_3_div_4_series>{}, "Minus3Div4OrXMinus3Div4Series");
    bs(50, W<minus_one_n_fact_n_in_n_series>{}, "MinusOneNFactNInNSeries");
    bs(51, W<minus_one_ned_in_n_series>{}, "MinusOneNedInNSeries");
    bs(52, W<minus_one_quarter_series>{}, "MinusOneQuarterSeries");
    bs(53, W<minus_three_plus_ln3_three_devided_two_plus_two_ln2_series>{}, "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2Series");
    bs(54, W<minus_x_minus_pi_4_or_minus_pi_4_series>{}, "MinusXMinusPi4OrMinusPi4Series");
    bs(55, W<one_div_sqrt2_sin_xdivsqrt2_series>{}, "OneDivSqrt2SinXDivSqrt2Series");
    bs(56, W<one_div_two_minus_x_multi_three_plus_x_series>{}, "OneDivTwoMinusXMultiThreePlusXSeries");
    bs(57, W<one_minus_sqrt_1minus4x_div_2x_series>{}, "OneMinusSqrt1minus4xDiv2xSeries");
    bs(58, W<one_series>{}, "OneSeries");
    bs(59, W<one_third_pi_squared_m_nine_series>{}, "OneThirdPiSquaredMNineSeries");
    bs(60, W<one_twelfth_3x2_pi2_series>{}, "OneTwelfth3x2Pi2Series");
    bs(61, W<one_twelfth_series>{}, "OneTwelfthSeries");
    bs(62, W<one_twelfth_x2_pi2_series>{}, "OneTwelfthX2Pi2Series");
    bs(63, W<pi_3_series>{}, "Pi3Series");
    bs(64, W<pi_4_series>{}, "Pi4Series");
    bs(65, W<pi_8_cosx_square_minus_1_div_3_cosx_series>{}, "Pi8CosxSquareMinus1Div3CosxSeries");
    bs(66, W<pi_cubed_32_series>{}, "PiCubed32Series");
    bs(67, W<pi_four_minus_ln2_halfed_series>{}, "PiFourMinusLn2HalfedSeries");
    bs(68, W<pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series>{}, "PiMinus3pi4AndPiMinusXMinus3pi4Series");
    bs(69, W<pi_minus_x_2_series>{}, "PiMinusX2Series");
    bs(70, W<pi_series>{}, "PiSeries");
    bs(71, W<pi_six_min_half_series>{}, "PiSixMinHalfSeries");
    bs(72, W<pi_squared_6_minus_one_series>{}, "PiSquared6MinusOneSeries");
    bs(73, W<pi_squared_twelve_series>{}, "PiSquaredTwelveSeries");
    bs(74, W<pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series>{}, "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeries");
    bs(75, W<pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series>{}, "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeries");
    bs(76, W<riemann_zeta_func_series>{}, "RiemannZetaFuncSeries");
    bs(77, W<riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series>{}, "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeries");
    bs(78, W<series_with_ln_number1_series>{}, "SeriesWithLnNumber1");
    bs(79, W<series_with_ln_number2_series>{}, "SeriesWithLnNumber2");
    bs(80, W<si_x_series>{}, "SiXSeries");
    bs(81, W<sin_series>{}, "SinSeries");
    bs(82, W<sin_x2_series>{}, "SinX2Series");
    bs(83, W<sinh_series>{}, "SinhSeries");
    bs(84, W<sinh_x2_series>{}, "SinhX2Series");
    bs(85, W<sqrt_1plusx_min_1_min_x_div_2_series>{}, "Sqrt1plusXMinus1MinusXDiv2Series");
    bs(86, W<sqrt_1plusx_series>{}, "Sqrt1plusXSeries");
    bs(87, W<sqrt_oneminussqrtoneminusx_div_x_series>{}, "SqrtOneminusSqrtoneminusxDivXSeries");
    bs(88, W<ten_minus_x_series>{}, "TenMinusXSeries");
    bs(89, W<three_minus_pi_series>{}, "ThreeMinusPiSeries");
    bs(90, W<two_arcsin_square_x_halfed_series>{}, "TwoArcsinSquareXHalfedSeries");
    bs(91, W<two_degree_x_series>{}, "TwoDegreeXSeries");
    bs(92, W<two_ln2_series>{}, "TwoLn2Series");
    bs(93, W<x_1mx_squared_series>{}, "X1mxSquaredSeries");
    bs(94, W<x_div_1minx2_series>{}, "XDiv1minX2Series");
    bs(95, W<x_div_1minx_series>{}, "XDiv1minXSeries");
    bs(96, W<x_min_sqrt_x_series>{}, "XMinSqrtXSeries");
    bs(97, W<x_series>{}, "XSeries");
    bs(98, W<x_two_series>{}, "XTwoSeries");
    bs(99, W<x_two_throught_squares_series>{}, "XTwoThroughtSquaresSeries");
    // bs(100, W<xmb_Jb_two_series>{}, "XmbJbTwoSeries");
    bs(101, W<xsquareplus3_div_xsquareplus2multix_minus_1_series>{}, "Xsquareplus3DivXsquareplus2multixMinus1Series");

    // SeriesAcceleration
    py::class_<MSeriesAcceleration>(m, name("SeriesAcceleration").c_str())
        .def("printInfo", &MSeriesAcceleration::get_name)
        .def("__call__", &MSeriesAcceleration::operator(),
             py::arg("n"), py::arg("order"), py::arg("data"));

    py::class_<brezinski_theta_algorithm<T,K>, MSeriesAcceleration>
        (m, name("BrezinskiThetaAlgorithm").c_str())
        .def(py::init<>());

    py::class_<chang_wynn_algorithm<T,K>, MSeriesAcceleration>
        (m, name("ChangWynnAlgorithm").c_str())
        .def(py::init<>());

    py::class_<drummond_d_algorithm<T,K>, MSeriesAcceleration>
        (m, name("DrummondDAlgorithm").c_str())
        .def(py::init<remainder_type, bool>(),
             py::arg("remainder") = remainder_type::t_type,
             py::arg("useRecurrentFormula") = false);

    py::class_<ford_sidi_2_algorithm<T,K>, MSeriesAcceleration>
        (m, name("FordSidi2Algorithm").c_str())
        .def(py::init<>());

    py::class_<ford_sidi_3_algorithm<T,K>, MSeriesAcceleration>
        (m, name("FordSidi3Algorithm").c_str())
        .def(py::init<>());

    py::class_<levin_algorithm<T,K>, MSeriesAcceleration>
        (m, name("LevinAlgorithm").c_str())
        .def(py::init<remainder_type, bool, T>(),
            py::arg("remainder") = remainder_type::t_type,
            py::arg("useRecurrentFormula") = false,
            py::arg("beta") = static_cast<T>(1));

    py::class_<levin_sidi_m_algorithm<T,K>, MSeriesAcceleration>

        (m, name("LevinSidiMAlgorithm").c_str())
        .def(py::init<remainder_type, T>(),
            py::arg("remainder") = remainder_type::t_type,
            py::arg("gamma") = static_cast<T>(10));

    py::class_<levin_sidi_s_algorithm<T,K>, MSeriesAcceleration>
        (m, name("LevinSidiSAlgorithm").c_str())
        .def(py::init<remainder_type, bool, T>(),
            py::arg("remainder") = remainder_type::t_type,
            py::arg("useRecurrentFormula") = false,
            py::arg("beta") = static_cast<T>(1));

    py::class_<lubkin_w_algorithm<T,K>, MSeriesAcceleration>
        (m, name("LubkinWAlgorithm").c_str())
        .def(py::init<>());

    py::class_<richardson_algorithm<T,K>, MSeriesAcceleration>
        (m, name("RichardsonAlgorithm").c_str())
        .def(py::init<>());

    py::class_<shanks_transform_alternating<T,K>, MSeriesAcceleration>
        (m, name("ShanksTransformAlternating").c_str())
        .def(py::init<>());

    py::class_<shanks_algorithm<T,K>, MSeriesAcceleration>
        (m, name("ShanksAlgorithm").c_str())
        .def(py::init<>());

    py::class_<weniger_algorithm<T,K>, MSeriesAcceleration>
        (m, name("WenigerAlgorithm").c_str())
        .def(py::init<>());

    py::class_<wynn_epsilon_1_algorithm<T,K>, MSeriesAcceleration>
        (m, name("WynnEpsilon1Algorithm").c_str())
        .def(py::init<>());

    py::class_<wynn_epsilon_2_algorithm<T,K>, MSeriesAcceleration>
        (m, name("WynnEpsilon2Algorithm").c_str())
        .def(py::init<>());

    py::class_<wynn_epsilon_3_algorithm<T,K>, MSeriesAcceleration>
        (m, name("WynnEpsilon3Algorithm").c_str())
        .def(py::init<T>(), py::arg("epsilon_threshold") = static_cast<T>(1e-3));

    py::class_<wynn_rho_algorithm<T,K>, MSeriesAcceleration>
        (m, name("WynnRhoAlgorithm").c_str())
        .def(py::init<numerator_type, T, T>(),
            py::arg("numerator") = numerator_type::rho_type,
            py::arg("gamma") = static_cast<T>(-1),
            py::arg("rho") = static_cast<T>(1));
}

template<FloatLike T>
struct binder<complex_precision<T>>{
    static void bind_all(py::module_& m, const std::string& suffix);
};


template <FloatLike T>
void binder<complex_precision<T>>::bind_all(py::module_& m, const std::string& suffix){

    using Complex = complex_precision<T>;

    using MSeriesBase = series_base<Complex, K>;
    using MSeriesAcceleration = series_acceleration<Complex, K>;

    auto name = [&](const char* base) { return std::string(base) + suffix; };

    py::class_<series_result<Complex>>(m, name("SeriesResult").c_str())
        .def(py::init<>())
        .def(py::init<std::vector<Complex>, std::vector<Complex>>())
        .def_readwrite("Sn", &series_result<Complex>::Sn)
        .def_readwrite("an", &series_result<Complex>::an);

    // SeriesBase
    py::class_<MSeriesBase>(m, name("SeriesBase").c_str())
        .def("get_x", &MSeriesBase::get_x)
        .def("get_sum", &MSeriesBase::get_sum)
        .def("get_name", &MSeriesBase::get_name)
        .def("generateSeries",
             &MSeriesBase::generate_series,
             py::arg("x"),
             py::arg("vecSize"),
             py::arg("addTParameter") = static_cast<T>(1),
             py::arg("addKParameter") = static_cast<K>(1));

    // Helper: bind a concrete no-arg series Impl<T,KIndex> derived from Base
    // BS = Bind Series
    auto bs = [&](int _i, auto tmpl, const char* basename) {
        using ImplTK = typename decltype(tmpl)::template type<Complex, K>;
        py::class_<ImplTK, MSeriesBase>(m, name(basename).c_str()).def(py::init<>());
    };

    bs(1,  W<abs_sin_x_minus_2_div_pi_series>{}, "AbsSinXMinus2DivPiSeries");
    bs(2,  W<arcsin_x2_series>{}, "ArcsinX2Series");
    bs(3,  W<arcsin_x_minus_x_series>{}, "ArcsinXMinusXSeries");
    bs(4,  W<arcsin_x_series>{}, "ArcsinXSeries");
    bs(5,  W<arcsinh_x_series>{}, "ArcsinhXSeries");
    bs(6,  W<arctanh_x2_series>{}, "ArctanhX2Series");
    bs(7,  W<arctanh_x_series>{}, "ArctanhXSeries");
    bs(8,  W<arctg_x2_series>{}, "ArctgX2Series");
    bs(9,  W<arctg_x3_series>{}, "ArctgX3Series");
    bs(10, W<arctg_x_series>{}, "ArctgXSeries");
    bs(11, W<bin_series>{}, "BinSeries");
    bs(12, W<ci_x_series>{}, "CiXSeries");
    bs(13, W<cos3xmin1_div_xsquare_series>{}, "Cos3xMinus1DivXsquareSeries");
    bs(14, W<cos_series>{}, "CosSeries");
    bs(15, W<cos_sqrt_x_series>{}, "CosSqrtXSeries");
    bs(16, W<cos_x2_series>{}, "CosX2Series");
    bs(17, W<cosh_series>{}, "CoshSeries");
    bs(18, W<e_x_series>{}, "EXSeries");
    bs(19, W<eighth_pi_m_one_third_series>{}, "EighthPiMOneThirdSeries");
    bs(20, W<erf_series>{}, "ErfSeries");
    bs(21, W<exp_m_cos_x_sinsin_x_series>{}, "ExpMCosXSinsinXSeries");
    bs(22, W<exp_series>{}, "ExpSeries");
    bs(23, W<exp_squared_erf_series>{}, "ExpSquaredErfSeries");
    bs(24, W<five_pi_twelve_series>{}, "FivePiTwelveSeries");
    bs(25, W<four_arctan_series>{}, "FourArctanSeries");
    bs(26, W<four_ln2_m_3_series>{}, "FourLn2M3Series");
    bs(27, W<half_asin_two_x_series>{}, "HalfAsinTwoXSeries");
    bs(28, W<half_minus_sinx_multi_pi_4_series>{}, "HalfMinusSinxMultiPi4Series");
    bs(29, W<half_multi_ln_1div2multi1minuscosx_series>{}, "HalfMultiLn1Div2Multi1MinusCosxSeries");
    bs(30, W<incomplete_Gamma_func_series>{}, "IncompleteGammaFuncSeries");
    bs(31, W<inverse_1mx_series>{}, "Inverse1mxSeries");
    bs(32, W<inverse_sqrt_1m4x_series>{}, "InvSqrt1m4xSeries");
    // bs(33, W<Ja_x_series>{}, "JaXSeries");
    bs(34, W<k_x_series>{}, "KXSeries");
    bs(35, W<lambert_W_func_series>{}, "LambertWFuncSeries");
    bs(36, W<ln1_m_x2_series>{}, "Ln1MinusX2Series");
    bs(37, W<ln1mx_series>{}, "Ln1mxSeries");
    bs(38, W<ln1px4_series>{}, "Ln1px4Series");
    bs(39, W<ln2_series>{}, "Ln2Series");
    bs(40, W<ln13_min_ln7_div_7_series>{}, "Ln13MinusLn7Div7Series");
    bs(41, W<ln_1_plus_x3_series>{}, "Ln1PlusX3Series");
    bs(42, W<ln_1plussqrt1plusxsquare_minus_ln_2_series>{}, "Ln1PlusSqrt1PlusXsquareMinusLn2Series");
    bs(43, W<ln_1plusx_div_1plusx2_series>{}, "Ln1plusXDiv1plusX2Series");
    bs(44, W<ln_cosx_series>{}, "LnCosxSeries");
    bs(45, W<ln_sinx_minus_ln_x_series>{}, "LnSinxMinusLnXSeries");
    bs(46, W<ln_x_plus_one_x_minus_one_halfed_series>{}, "LnXPlusOneXMinusOneHalfedSeries");
    bs(47, W<m_fact_1mx_mp1_inverse_series>{}, "MFact1mxMp1InverseSeries");
    bs(48, W<mean_sinh_sin_series>{}, "MeanSinhSinSeries");
    bs(49, W<minus_3_div_4_or_x_minus_3_div_4_series>{}, "Minus3Div4OrXMinus3Div4Series");
    bs(50, W<minus_one_n_fact_n_in_n_series>{}, "MinusOneNFactNInNSeries");
    bs(51, W<minus_one_ned_in_n_series>{}, "MinusOneNedInNSeries");
    bs(52, W<minus_one_quarter_series>{}, "MinusOneQuarterSeries");
    bs(53, W<minus_three_plus_ln3_three_devided_two_plus_two_ln2_series>{}, "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2Series");
    bs(54, W<minus_x_minus_pi_4_or_minus_pi_4_series>{}, "MinusXMinusPi4OrMinusPi4Series");
    bs(55, W<one_div_sqrt2_sin_xdivsqrt2_series>{}, "OneDivSqrt2SinXDivSqrt2Series");
    bs(56, W<one_div_two_minus_x_multi_three_plus_x_series>{}, "OneDivTwoMinusXMultiThreePlusXSeries");
    bs(57, W<one_minus_sqrt_1minus4x_div_2x_series>{}, "OneMinusSqrt1minus4xDiv2xSeries");
    bs(58, W<one_series>{}, "OneSeries");
    bs(59, W<one_third_pi_squared_m_nine_series>{}, "OneThirdPiSquaredMNineSeries");
    bs(60, W<one_twelfth_3x2_pi2_series>{}, "OneTwelfth3x2Pi2Series");
    bs(61, W<one_twelfth_series>{}, "OneTwelfthSeries");
    bs(62, W<one_twelfth_x2_pi2_series>{}, "OneTwelfthX2Pi2Series");
    bs(63, W<pi_3_series>{}, "Pi3Series");
    bs(64, W<pi_4_series>{}, "Pi4Series");
    bs(65, W<pi_8_cosx_square_minus_1_div_3_cosx_series>{}, "Pi8CosxSquareMinus1Div3CosxSeries");
    bs(66, W<pi_cubed_32_series>{}, "PiCubed32Series");
    bs(67, W<pi_four_minus_ln2_halfed_series>{}, "PiFourMinusLn2HalfedSeries");
    bs(68, W<pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series>{}, "PiMinus3pi4AndPiMinusXMinus3pi4Series");
    bs(69, W<pi_minus_x_2_series>{}, "PiMinusX2Series");
    bs(70, W<pi_series>{}, "PiSeries");
    bs(71, W<pi_six_min_half_series>{}, "PiSixMinHalfSeries");
    bs(72, W<pi_squared_6_minus_one_series>{}, "PiSquared6MinusOneSeries");
    bs(73, W<pi_squared_twelve_series>{}, "PiSquaredTwelveSeries");
    bs(74, W<pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series>{}, "PiXMinusXSquareSquareMinusThreePiXPlusTwoPiSquareSeries");
    bs(75, W<pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series>{}, "PiXMultiEXpiPlusEMinusXpiDividedEXpiMinusEMinusXpiSeries");
    bs(76, W<riemann_zeta_func_series>{}, "RiemannZetaFuncSeries");
    bs(77, W<riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series>{}, "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeries");
    bs(78, W<series_with_ln_number1_series>{}, "SeriesWithLnNumber1");
    bs(79, W<series_with_ln_number2_series>{}, "SeriesWithLnNumber2");
    bs(80, W<si_x_series>{}, "SiXSeries");
    bs(81, W<sin_series>{}, "SinSeries");
    bs(82, W<sin_x2_series>{}, "SinX2Series");
    bs(83, W<sinh_series>{}, "SinhSeries");
    bs(84, W<sinh_x2_series>{}, "SinhX2Series");
    bs(85, W<sqrt_1plusx_min_1_min_x_div_2_series>{}, "Sqrt1plusXMinus1MinusXDiv2Series");
    bs(86, W<sqrt_1plusx_series>{}, "Sqrt1plusXSeries");
    bs(87, W<sqrt_oneminussqrtoneminusx_div_x_series>{}, "SqrtOneminusSqrtoneminusxDivXSeries");
    bs(88, W<ten_minus_x_series>{}, "TenMinusXSeries");
    bs(89, W<three_minus_pi_series>{}, "ThreeMinusPiSeries");
    bs(90, W<two_arcsin_square_x_halfed_series>{}, "TwoArcsinSquareXHalfedSeries");
    bs(91, W<two_degree_x_series>{}, "TwoDegreeXSeries");
    bs(92, W<two_ln2_series>{}, "TwoLn2Series");
    bs(93, W<x_1mx_squared_series>{}, "X1mxSquaredSeries");
    bs(94, W<x_div_1minx2_series>{}, "XDiv1minX2Series");
    bs(95, W<x_div_1minx_series>{}, "XDiv1minXSeries");
    bs(96, W<x_min_sqrt_x_series>{}, "XMinSqrtXSeries");
    bs(97, W<x_series>{}, "XSeries");
    bs(98, W<x_two_series>{}, "XTwoSeries");
    bs(99, W<x_two_throught_squares_series>{}, "XTwoThroughtSquaresSeries");
    // bs(100, W<xmb_Jb_two_series>{}, "XmbJbTwoSeries");
    bs(101, W<xsquareplus3_div_xsquareplus2multix_minus_1_series>{}, "Xsquareplus3DivXsquareplus2multixMinus1Series");

    // SeriesAcceleration
    py::class_<MSeriesAcceleration>(m, name("SeriesAcceleration").c_str())
        .def("printInfo", &MSeriesAcceleration::get_name)
        .def("__call__", &MSeriesAcceleration::operator(),
             py::arg("n"), py::arg("order"), py::arg("data"));

    py::class_<brezinski_theta_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("BrezinskiThetaAlgorithm").c_str())
        .def(py::init<>());

    py::class_<chang_wynn_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("ChangWynnAlgorithm").c_str())
        .def(py::init<>());

    py::class_<drummond_d_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("DrummondDAlgorithm").c_str())
        .def(py::init<remainder_type, bool>(),
             py::arg("remainder") = remainder_type::t_type,
             py::arg("useRecurrentFormula") = false);

    py::class_<ford_sidi_2_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("FordSidi2Algorithm").c_str())
        .def(py::init<>());

    py::class_<ford_sidi_3_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("FordSidi3Algorithm").c_str())
        .def(py::init<>());

    py::class_<levin_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("LevinAlgorithm").c_str())
        .def(py::init<remainder_type, bool, T>(),
            py::arg("remainder") = remainder_type::t_type,
            py::arg("useRecurrentFormula") = false,
            py::arg("beta") = static_cast<T>(1));

    py::class_<levin_sidi_m_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("LevinSidiMAlgorithm").c_str())
        .def(py::init<remainder_type, T>(),
            py::arg("remainder") = remainder_type::t_type,
            py::arg("gamma") = static_cast<T>(10));

    py::class_<levin_sidi_s_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("LevinSidiSAlgorithm").c_str())
        .def(py::init<remainder_type, bool, T>(),
            py::arg("remainder") = remainder_type::t_type,
            py::arg("useRecurrentFormula") = false,
            py::arg("beta") = static_cast<T>(1));

    py::class_<lubkin_w_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("LubkinWAlgorithm").c_str())
        .def(py::init<>());

    py::class_<richardson_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("RichardsonAlgorithm").c_str())
        .def(py::init<>());

    py::class_<shanks_transform_alternating<Complex,K>, MSeriesAcceleration>
        (m, name("ShanksTransformAlternating").c_str())
        .def(py::init<>());

    py::class_<shanks_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("ShanksAlgorithm").c_str())
        .def(py::init<>());

    py::class_<weniger_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("WenigerAlgorithm").c_str())
        .def(py::init<>());

    py::class_<wynn_epsilon_1_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("WynnEpsilon1Algorithm").c_str())
        .def(py::init<>());

    py::class_<wynn_epsilon_2_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("WynnEpsilon2Algorithm").c_str())
        .def(py::init<>());

    py::class_<wynn_epsilon_3_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("WynnEpsilon3Algorithm").c_str())
        .def(py::init<T>(), py::arg("epsilon_threshold") = static_cast<T>(1e-3));

    py::class_<wynn_rho_algorithm<Complex,K>, MSeriesAcceleration>
        (m, name("WynnRhoAlgorithm").c_str())
        .def(py::init<numerator_type, T, T>(),
            py::arg("numerator") = numerator_type::rho_type,
            py::arg("gamma") = static_cast<T>(-1),
            py::arg("rho") = static_cast<T>(1));
}

PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series (double + float_precision), helper-organized, no-arg constructors, backward-compatible";

    py::enum_<remainder_type>(m, "RemainderType")
        .value("u_type", remainder_type::u_type)
        .value("t_type", remainder_type::t_type)
        .value("v_type", remainder_type::v_type)
        .value("t_wave_type", remainder_type::t_wave_type)
        .value("v_wave_type", remainder_type::v_wave_type)
        .export_values();

    py::enum_<numerator_type>(m, "NumeratorType")
        .value("rho_type", numerator_type::rho_type)
        .value("generalized_type", numerator_type::generalized_type)
        .value("gamma_rho_type", numerator_type::gamma_rho_type)
        .export_values();

    #define ADD_NUM_BINOP(pyname, rpyname, cppop) \
      .def(pyname, [](const float_precision &a, const float_precision &b) { return a cppop b; }, py::is_operator()) \
      .def(rpyname, [](const float_precision &self, const float_precision &other) { return other cppop self; }, py::is_operator()) \
      .def(pyname, [](const float_precision &self, double other) { return self cppop float_precision(other); }, py::is_operator()) \
      .def(rpyname, [](const float_precision &self, double other) { return float_precision(other) cppop self; }, py::is_operator()) \
      .def(pyname, [](const float_precision &self, long long other) { return self cppop float_precision((double)other); }, py::is_operator()) \
      .def(rpyname, [](const float_precision &self, long long other) { return float_precision((double)other) cppop self; }, py::is_operator())
    #define ADD_NUM_INPLACE(pyname, cppop) \
      .def(pyname, [](float_precision &self, const float_precision &other) -> float_precision& { self = self cppop other; return self; }, py::is_operator()) \
      .def(pyname, [](float_precision &self, double other) -> float_precision& { self = self cppop float_precision(other); return self; }, py::is_operator()) \
      .def(pyname, [](float_precision &self, long long other) -> float_precision& { self = self cppop float_precision((double)other); return self; }, py::is_operator())
    #define ADD_NUM_CMP(pyname, cppop) \
      .def(pyname, [](const float_precision &a, const float_precision &b) { return a cppop b; }, py::is_operator()) \
      .def(pyname, [](const float_precision &a, double b) { return a cppop float_precision(b); }, py::is_operator()) \
      .def(pyname, [](const float_precision &a, long long b) { return a cppop float_precision((double)b); }, py::is_operator())

    py::class_<float_precision>(m, "Arb")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("d"))
        .def(py::init<const std::string&>(), py::arg("s"))
        .def("__str__", [](const float_precision &x){ return x.toString(); })
        .def("__format__", [](const float_precision &x, const std::string &fmt) { return x.toString(); })
        .def("__repr__", [](const float_precision &self) { return "<Arb: " + self.toString() + ">"; })
        ADD_NUM_BINOP("__add__", "__radd__", +)
        ADD_NUM_BINOP("__sub__", "__rsub__", -)
        ADD_NUM_BINOP("__mul__", "__rmul__", *)
        ADD_NUM_BINOP("__truediv__", "__rtruediv__", /)
        ADD_NUM_INPLACE("__iadd__", +)
        ADD_NUM_INPLACE("__isub__", -)
        ADD_NUM_INPLACE("__imul__", *)
        ADD_NUM_INPLACE("__itruediv__", /)
        ADD_NUM_CMP("__eq__", ==)
        ADD_NUM_CMP("__ne__", !=)
        ADD_NUM_CMP("__lt__", <)
        ADD_NUM_CMP("__le__", <=)
        ADD_NUM_CMP("__gt__", >)
        ADD_NUM_CMP("__ge__", >=)
        .def("__neg__", [](const float_precision &a) { return -a; })
        .def("__pos__", [](const float_precision &a) { return a; })
        .def("__abs__", [](const float_precision &a) { return a < float_precision(0.0) ? -a : a; })
        .def("__float__", [](const float_precision &x){ return static_cast<double>(x); })
        .def("__int__", [](const float_precision &x){ return static_cast<long>(static_cast<double>(x)); })
        .def("__index__", [](const float_precision &x){ return static_cast<long>(static_cast<double>(x)); })
        .def("__getstate__", [](const float_precision &self) { return self.toString(); })
        .def("__setstate__", [](float_precision &self, const std::string &state) { new (&self) float_precision(state); });

    #undef ADD_NUM_BINOP
    #undef ADD_NUM_INPLACE
    #undef ADD_NUM_CMP

    using CArb = complex_precision<float_precision>;

    #define ADD_NUM_BINOP(pyname, rpyname, op) \
        .def(pyname, [](const CArb& a, const CArb& b){ return a op b; }, py::is_operator()) \
        .def(rpyname, [](const CArb& self, const CArb& other){ return other op self; }, py::is_operator()) \
        .def(pyname, [](const CArb& self, const float_precision& r){ return self op CArb(r); }, py::is_operator()) \
        .def(rpyname, [](const CArb& self, const float_precision& r){ return CArb(r) op self; }, py::is_operator()) \
        .def(pyname, [](const CArb& self, double r){ return self op CArb(float_precision(r)); }, py::is_operator()) \
        .def(rpyname, [](const CArb& self, double r){ return CArb(float_precision(r)) op self; }, py::is_operator()) \
        .def(pyname, [](const CArb& self, long long r){ return self op CArb(float_precision((double)r)); }, py::is_operator()) \
        .def(rpyname, [](const CArb& self, long long r){ return CArb(float_precision((double)r)) op self; }, py::is_operator())
    #define ADD_NUM_INPLACE(pyname, op) \
        .def(pyname, [](CArb& self, const CArb& other)->CArb&{ self = self op other; return self; }, py::is_operator()) \
        .def(pyname, [](CArb& self, const float_precision& r)->CArb&{ self = self op CArb(r); return self; }, py::is_operator()) \
        .def(pyname, [](CArb& self, double r)->CArb&{ self = self op CArb(float_precision(r)); return self; }, py::is_operator()) \
        .def(pyname, [](CArb& self, long long r)->CArb&{ self = self op CArb(float_precision((double)r)); return self; }, py::is_operator())
    py::class_<CArb>(m, "CArb")
        .def(py::init<>())
        .def(py::init([](double re){ return CArb(re, 0.0); }))
        .def(py::init<const std::string&>(), py::arg("s"))
        .def(py::init<float_precision>(), py::arg("re"))
        .def(py::init<float_precision, float_precision>(), py::arg("re"), py::arg("im"))
        .def(py::init([](double re, double im){ return CArb(float_precision(re), float_precision(im)); }),
             py::arg("re"), py::arg("im"))
        .def(py::init([](std::complex<double> z){
             return CArb(float_precision(z.real()), float_precision(z.imag()));
        }), py::arg("z"))
        .def_property("real",
            [](const CArb& z){ return z.real(); },
            [](CArb& z, const float_precision& r){ z.real(r); })
        .def_property("imag",
            [](const CArb& z){ return z.imag(); },
            [](CArb& z, const float_precision& i){ z.imag(i); })
        .def("__str__", [](const CArb& z){ return to_string(z); })
        .def("__format__", [](const CArb &x, const std::string &fmt) { return to_string(x); })
        .def("__repr__", [](const CArb& z){
            return "CArb(" + z.real().toString() + ", " + z.imag().toString() + ")";
        })
        .def("__complex__", [](const CArb& z){
            return std::complex<double>((double)z.real(), (double)z.imag());
        })
        ADD_NUM_BINOP("__add__", "__radd__", +)
        ADD_NUM_BINOP("__sub__", "__rsub__", -)
        ADD_NUM_BINOP("__mul__", "__rmul__", *)
        ADD_NUM_BINOP("__truediv__", "__rtruediv__", /)
        ADD_NUM_INPLACE("__iadd__", +)
        ADD_NUM_INPLACE("__isub__", -)
        ADD_NUM_INPLACE("__imul__", *)
        ADD_NUM_INPLACE("__itruediv__", /)
        .def("__neg__", [](const CArb& a){ return CArb(-a.real(), -a.imag()); })
        .def("__pos__", [](const CArb& a){ return a; })
        .def("__abs__", [](const CArb& a){ return a.abs(); })
        // THIS IS HORRIBLE
        .def("__float__", [](const CArb &x){ return static_cast<double>(x.real()); })
        .def("__int__", [](const CArb &x){ return static_cast<long>(static_cast<double>(x.real())); })
        .def("__eq__", [](const CArb& a, const CArb& b){ return a == b; }, py::is_operator())
        .def("__ne__", [](const CArb& a, const CArb& b){ return !(a == b); }, py::is_operator())
        .def("__getstate__", [](const CArb& z){
            return std::make_pair(z.real().toString(), z.imag().toString());
        })
        .def("__setstate__", [](CArb& z, const std::pair<std::string,std::string>& s){
            new (&z) CArb(float_precision(s.first), float_precision(s.second));
        });

    binder<float>::bind_all(m, "F32");
    binder<double>::bind_all(m, "F64");
    binder<long double>::bind_all(m, "FL");
    binder<float_precision>::bind_all(m, "Arb");

    binder<complex_precision<float>>::bind_all(m, "CF32");
    binder<complex_precision<double>>::bind_all(m, "CF64");
    binder<complex_precision<long double>>::bind_all(m, "CFL");
    binder<complex_precision<float_precision>>::bind_all(m, "CArb");
}
