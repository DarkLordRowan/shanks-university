#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "series.h"
#include "methods.hpp"


namespace py = pybind11;
using T = double;
using K = size_t;

PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series + all Shanks‑type transformations";

    /* ---- Base series ---- */
    using SeriesBase      = series_base<T, K>;
    py::class_<SeriesBase>(m, "SeriesBase")
        .def("S_n", &SeriesBase::S_n)
        .def("__call__", &SeriesBase::operator())
        .def("get_sum", &SeriesBase::get_sum)
        .def("get_x", &SeriesBase::get_x);

    #define BIND_SERIES(DERIVED, PYNAME, ...)       \
        py::class_<DERIVED, SeriesBase>(m, PYNAME)  \
            .def(py::init<__VA_ARGS__>())           \
            .def("__call__", &DERIVED::operator())  \
            .def("S_n", &DERIVED::S_n)              \
            .def("get_sum", &DERIVED::get_sum);
    #define UNPAREN(...) __VA_ARGS__
    #define S(Alias, Impl, NameStr, BIND_ARGS) \
        using Alias = Impl<T, K>; \
        BIND_SERIES(Alias, NameStr, UNPAREN BIND_ARGS);
    S(SER1, exp_series, "ExpSeries", (T))
    S(SER2, cos_series, "CosSeries", (T))
    S(SER3, sin_series, "SinSeries", (T))
    S(SER4, cosh_series, "CoshSeries", (T))
    S(SER5, sinh_series, "SinhSeries", (T))
    S(SER6, bin_series, "BinSeries", (T,T))
    S(SER7, four_arctan_series, "FourArctanSeries", (T))
    S(SER8, ln1mx_series, "Ln1mxSeries", (T))
    S(SER9, mean_sinh_sin_series, "MeanSinhSinSeries", (T))
    S(SER10, exp_squared_erf_series, "ExpSquaredErfSeries", (T))
    S(SER11, xmb_Jb_two_series, "XmbJbTwoSeries", (T,K))
    S(SER12, half_asin_two_x_series, "HalfAsinTwoXSeries", (T))
    S(SER13, inverse_1mx_series, "Inverse1mxSeries", (T))
    S(SER14, x_1mx_squared_series, "X_1mx_SquaredSeries", (T))
    S(SER15, erf_series, "ErfSeries", (T))
    S(SER16, m_fact_1mx_mp1_inverse_series, "MFact1mxMp1InverseSeries", (T,K))
    S(SER17, inverse_sqrt_1m4x_series, "InvSqrt1m4xSeries", (T))
    S(SER18, one_twelfth_3x2_pi2_series, "OneTwelfth3x2Pi2Series", (T))
    S(SER19, x_twelfth_x2_pi2_series, "XTwelfthX2Pi2Series", (T))
    S(SER20, ln2_series, "Ln2Series", (T))
    S(SER21, one_series, "OneSeries", (T))
    S(SER22, minus_one_quarter_series, "MinusOneQuarterSeries", (T))
    S(SER23, pi_3_series, "Pi3Series", (T))
    S(SER24, pi_4_series, "Pi4Series", (T))
    S(SER25, pi_squared_6_minus_one_series, "PiSquared6MinusOneSeries", (T))
    S(SER26, three_minus_pi_series, "ThreeMinusPiSeries", (T))
    S(SER27, one_twelfth_series, "OneTwelfthSeries", (T))
    S(SER28, eighth_pi_m_one_third_series, "EighthPiMOneThirdSeries", (T))
    S(SER29, one_third_pi_squared_m_nine_series, "OneThirdPiSquaredMNineSeries", (T))
    S(SER30, four_ln2_m_3_series, "FourLn2M3Series", (T))
    S(SER31, exp_m_cos_x_sinsin_x_series, "ExpMCosXSinsinXSeries", (T))
    S(SER32, pi_four_minus_ln2_halfed_series, "PiFourMinusLn2HalfedSeries", (T))
    S(SER33, five_pi_twelve_series, "FivePiTwelveSeries", (T))
    S(SER34, x_two_series, "XTwoSeries", (T))
    S(SER35, pi_six_min_half_series, "PiSixMinHalfSeries", (T))
    S(SER36, x_two_throught_squares_series, "XTwoThroughtSquaresSeries", (T))
    S(SER37, minus_one_ned_in_n_series, "MinusOneNedInNSeries", (T))
    S(SER38, minus_one_n_fact_n_in_n_series, "MinusOneNFactNInNSeries", (T))
    S(SER39, ln_x_plus_one_x_minus_one_halfed_series, "LnXPlusOneXMinusOneHalfedSeries", (T))
    S(SER40, two_arcsin_square_x_halfed_series, "TwoArcsinSquareXHalfedSeries", (T))
    S(SER41, pi_squared_twelve_series, "PiSquaredTwelveSeries", (T))
    S(SER42, pi_cubed_32_series, "PiCubed32Series", (T))
    S(SER43, minus_three_plus_ln3_three_devided_two_plus_two_ln2_series, "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2Series", (T))
    S(SER44, two_ln2_series, "TwoLn2Series", (T))
    S(SER45, pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series, "PiXMultiE_XpiPlusEMinusXpiDividedE_XpiMinusEMinusXpiMinusOneSeries", (T))
    S(SER46, pi_minus_x_2, "PiMinusX2Series", (T))
    S(SER47, half_multi_ln_1div2multi1minuscosx, "HalfMultiLn1Div2Multi1MinusCosxSeries", (T))
    S(SER48, half_minus_sinx_multi_pi_4, "HalfMinusSinxMultiPi4Series", (T))
    S(SER49, ln_1plussqrt1plusxsquare_minus_ln_2, "Ln1PlusSqrt1PlusXsquareMinusLn2Series", (T))
    S(SER50, ln_cosx, "LnCosxSeries", (T))
    S(SER51, ln_sinx_minus_ln_x, "LnSinxMinusLnXSeries", (T))
    S(SER52, pi_8_cosx_square_minus_1_div_3_cosx, "Pi8CosxSquareMinus1Div3CosxSeries", (T))
    S(SER53, sqrt_oneminussqrtoneminusx_div_x, "SqrtOneminusSqrtoneminusxDivXSeries", (T))
    S(SER54, one_minus_sqrt_1minus4x_div_2x, "OneMinusSqrt1minus4xDiv2xSeries", (T))
    S(SER55, arcsin_x_minus_x_series, "ArcsinXMinusXSeries", (T))
    S(SER56, pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series, "PiXMinusXSquareAndXSquareMinusThreePiXPlusTwoPiSquareSeries", (T))
    S(SER57, abs_sin_x_minus_2_div_pi_series, "AbsSinXMinus2DivPiSeries", (T))
    S(SER58, pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series, "PiMinus3pi4AndPiMinusXMinus3pi4Series", (T))
    S(SER59, minus_3_div_4_or_x_minus_3_div_4_series, "Minus3Div4OrXMinus3Div4Series", (T))
    S(SER60, ten_minus_x_series, "TenMinusXSeries", (T))
    S(SER61, x_series, "XSeries", (T))
    S(SER62, minus_x_minus_pi_4_or_minus_pi_4_series, "MinusXMinusPi4OrMinusPi4Series", (T))
    S(SER63, one_div_two_minus_x_multi_three_plus_x_series, "OneDivTwoMinusXMultiThreePlusXSeries", (T))
    S(SER64, Si_x_series, "SiXSeries", (T))
    S(SER65, Ci_x_series, "CiXSeries", (T))
    S(SER66, Riemann_zeta_func_series, "RiemannZetaFuncSeries", (T))
    S(SER67, Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series, "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeries", (T))
    S(SER68, xsquareplus3_div_xsquareplus2multix_minus_1_series, "Xsquareplus3DivXsquareplus2multixMinus1Series", (T))
    S(SER69, arcsin_x_series, "ArcsinXSeries", (T))
    S(SER70, arctg_x_series, "ArctgXSeries", (T))
    S(SER71, K_x_series, "KXSeries", (T))
    S(SER72, E_x_series, "EXSeries", (T))
    S(SER73, sqrt_1plusx_series, "Sqrt1plusXSeries", (T))
    S(SER74, Lambert_W_func_series, "LambertWFuncSeries", (T))
    S(SER75, Incomplete_Gamma_func_series, "IncompleteGammaFuncSeries", (T,T))
    S(SER76, Series_with_ln_number1, "SeriesWithLnNumber1", (T))
    S(SER77, Series_with_ln_number2, "SeriesWithLnNumber2", (T))
    S(SER78, pi_series, "PiSeries", (T))
    S(SER79, x_min_sqrt_x_series, "XMinSqrtXSeries", (T))
    S(SER80, arctan_x2_series, "ArctanX2Series", (T))
    S(SER81, ln1px4_series, "Ln1px4Series", (T))
    S(SER82, sin_x2_series, "SinX2Series", (T))
    S(SER83, arctan_x3_series, "ArctanX3Series", (T))
    S(SER84, arcsin_x2_series, "ArcsinX2Series", (T))
    S(SER85, ln1_m_x2_series, "Ln1MinusX2Series", (T))
    S(SER86, artanh_x_series, "ArtanhXSeries", (T))
    S(SER87, arcsinh_x_series, "ArcsinhXSeries", (T))
    S(SER88, cos_x2_series, "CosX2Series", (T))
    S(SER89, sinh_x2_series, "SinhX2Series", (T))
    S(SER90, arctanh_x2_series, "ArctanhX2Series", (T))
    S(SER91, cos3xmin1_div_xsqare_series, "Cos3xMinus1DivXsquareSeries", (T))
    S(SER92, two_degree_x_series, "TwoDegreeXSeries", (T))
    S(SER93, sqrt_1plusx_min_1_min_x_div_2_series, "Sqrt1plusXMinus1MinusXDiv2Series", (T))
    S(SER94, ln13_min_ln7_div_7_series, "Ln13MinusLn7Div7Series", (T))
    S(SER95, Ja_x_series, "JaXSeries", (T,T))
    S(SER96, one_div_sqrt2_sin_xdivsqrt2_series, "OneDivSqrt2SinXDivSqrt2Series", (T))
    S(SER97, ln_1plusx_div_1plusx2, "Ln1plusXDiv1plusX2Series", (T))
    S(SER98, cos_sqrt_x, "CosSqrtXSeries", (T))
    S(SER99, ln_1_plus_x3, "Ln1PlusX3Series", (T))
    S(SER100, x_div_1minx, "XDiv1minXSeries", (T))
    S(SER101, x_div_1minx2, "XDiv1minX2Series", (T))
    S(SER102, gamma_series, "GammaSeries", (T,T))
    #undef BIND_SERIES
    #undef UNPAREN
    #undef S

    // series_acceleration
    using SeriesAccel = series_acceleration<T, K, SeriesBase*>;
    py::class_<SeriesAccel>(m, "SeriesAcceleration")
        .def("__call__", &SeriesAccel::operator());

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

    using Shanks = shanks_transform<T, K, SeriesBase*>;
    py::class_<Shanks, SeriesAccel>(m, "ShanksTransform")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &Shanks::operator());

    using ShanksAlt = shanks_transform_alternating<T, K, SeriesBase*>;
    py::class_<ShanksAlt, SeriesAccel>(m, "ShanksTransformAlternating")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &ShanksAlt::operator());


    using BrezinskiTheta = brezinski_theta_algorithm<T, K, SeriesBase*>;
    py::class_<BrezinskiTheta, SeriesAccel>(m, "BrezinskiThetaAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &BrezinskiTheta::operator());

    using ChangWynn = chang_wynn_algorithm<T, K, SeriesBase*>;
    py::class_<ChangWynn, SeriesAccel>(m, "ChangWynnAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &ChangWynn::operator());

    using DrummondD = drummond_d_algorithm<T, K, SeriesBase*>;
    py::class_<DrummondD, SeriesAccel>(m, "DrummondDAlgorithm")
        .def(py::init<SeriesBase*, remainder_type, bool>(),
             py::arg("series"), py::keep_alive<1, 2>(),
             py::arg("variant") = remainder_type::u_variant,
             py::arg("useRecFormulas") = false)
        .def("__call__", &DrummondD::operator());

    using FordSidiTwo = ford_sidi_algorithm_two<T, K, SeriesBase*>;
    py::class_<FordSidiTwo, SeriesAccel>(m, "FordSidiAlgorithmTwo")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &FordSidiTwo::operator());

    using FordSidiThree = ford_sidi_algorithm_three<T, K, SeriesBase*>;
    py::class_<FordSidiThree, SeriesAccel>(m, "FordSidiAlgorithmThree")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &FordSidiThree::operator());

    using LevinL = levin_algorithm<T, K, SeriesBase*>;
    py::class_<LevinL, SeriesAccel>(m, "LevinLAlgorithm")
        .def(py::init<SeriesBase*, bool, T>(),
             py::arg("series"), py::keep_alive<1, 2>(),
             py::arg("useRecFormulas") = false,
             py::arg("beta") = static_cast<T>(-1.5))
        .def("__call__", (T (LevinL::*)(K, K) const) &LevinL::operator());

    using LevinSidiM = levin_sidi_m_algorithm<T, K, SeriesBase*>;
    py::class_<LevinSidiM, SeriesAccel>(m, "LevinSidiMAlgorithm")
        .def(py::init<SeriesBase*, remainder_type, T>(),
             py::arg("series"), py::keep_alive<1, 2>(),
             py::arg("variant") = remainder_type::u_variant,
             py::arg("gamma") = static_cast<T>(10))
        .def("__call__", &LevinSidiM::operator());

    using LevinSidiS = levin_sidi_s_algorithm<T, K, SeriesBase*>;
    py::class_<LevinSidiS, SeriesAccel>(m, "LevinSidiSAlgorithm")
        .def(py::init<SeriesBase*, remainder_type, bool, T>(),
             py::arg("series"), py::keep_alive<1, 2>(),
             py::arg("variant") = remainder_type::u_variant,
             py::arg("useRecFormulas") = false,
             py::arg("parameter") = static_cast<T>(1))
        .def("__call__", &LevinSidiS::operator());

    using LubkinW = W_lubkin_algorithm<T, K, SeriesBase*>;
    py::class_<LubkinW, SeriesAccel>(m, "LubkinWAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &LubkinW::operator());

    using RhoWynn = rho_wynn_algorithm<T, K, SeriesBase*>;
    py::class_<RhoWynn, SeriesAccel>(m, "RhoWynnAlgorithm")
        .def(py::init<SeriesBase*, numerator_type, T, T>(),
             py::arg("series"), py::keep_alive<1, 2>(),
             py::arg("variant") = numerator_type::rho_variant,
             py::arg("gamma") = static_cast<T>(1),
             py::arg("RHO") = static_cast<T>(0))
        .def("__call__", (T (RhoWynn::*)(K, K) const) &RhoWynn::operator());

    using Richardson = richardson_algorithm<T, K, SeriesBase*>;
    py::class_<Richardson, SeriesAccel>(m, "RichardsonAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &Richardson::operator());

    using Weniger = weniger_algorithm<T, K, SeriesBase*>;
    py::class_<Weniger, SeriesAccel>(m, "WenigerAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &Weniger::operator());

    using Epsilon = wynn_epsilon_1_algorithm<T, K, SeriesBase*>;
    py::class_<Epsilon, SeriesAccel>(m, "WynnEpsilonAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &Epsilon::operator());

    using Epsilon2 = wynn_epsilon_algorithm_two<T, K, SeriesBase*>;
    py::class_<Epsilon2, SeriesAccel>(m, "WynnEpsilonAlgorithmTwo")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>())
        .def("__call__", &Epsilon2::operator());

    using Epsilon3 = wynn_epsilon_algorithm_three<T, K, SeriesBase*>;
    py::class_<Epsilon3, SeriesAccel>(m, "WynnEpsilonAlgorithmThree")
        .def(py::init<SeriesBase*, T>(),
             py::arg("series"), py::keep_alive<1, 2>(),
             py::arg("epsilon_threshold") = static_cast<T>(1e-3))
        .def("__call__", &Epsilon3::operator());
}
