/**
 * @file py.cpp
 * @brief This file contains pybind11 bindings.
 */
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "series.h"
#include "methods.hpp"

namespace py = pybind11;

template <std::floating_point T, std::unsigned_integral K>
class array_series : public series_base<T,K> {
public:
    array_series() = delete;

    explicit array_series(py::buffer buffer);
    explicit array_series(py::object seq);

    [[nodiscard]] virtual T operator()(K n) const override;
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

private:
    void build_from_buffer(py::buffer buffer);
    void copy_from_buffer_info(const py::buffer_info& info);
    void copy_from_object(py::object obj);

    py::object owner_;       ///< keeps the Python object alive while this instance exists
    T* data_ptr_ = nullptr;  ///< points into either owner_'s buffer or storage_.data()
    std::vector<T> storage_; ///< owned storage when zero-copy impossible
    std::size_t size_ = 0;
};

template <std::floating_point T, std::unsigned_integral K>
array_series<T,K>::array_series(py::buffer buffer)
: owner_(buffer)
{
    build_from_buffer(buffer);
}

template <std::floating_point T, std::unsigned_integral K>
array_series<T,K>::array_series(py::object seq)
: owner_(seq)
{
    // Try to treat as array-like first (fast path, allows numpy, memoryview, etc.)
    if (py::array arr = py::array::ensure(seq)) {
        // ask pybind11 to produce a contiguous c-style array of T (forcecast: allow conversion)
        using arr_t = py::array_t<T, py::array::c_style | py::array::forcecast>;
        if (py::array conv = arr_t::ensure(arr)) {
            if (conv.ndim() == 1) {
                auto info = conv.request();
                data_ptr_ = static_cast<T*>(info.ptr);
                size_ = static_cast<std::size_t>(info.shape[0]);
                owner_ = conv; // keep the possibly-new array alive
                return;
            }
        }
    }
    // fallback: sequence-copy
    copy_from_object(owner_);
}

template <std::floating_point T, std::unsigned_integral K>
void array_series<T,K>::build_from_buffer(py::buffer buffer)
{
    py::buffer_info info = buffer.request();
    if (info.ndim != 1) {
        // not 1-D buffer -> fall back to generic sequence copy
        copy_from_object(owner_);
        return;
    }

    // If format exactly matches T and contiguous in memory -> zero-copy
    if (info.format == py::format_descriptor<T>::format()
        && info.strides.size() <= 1
        && static_cast<std::size_t>(info.itemsize) == sizeof(T)
        && (info.strides.empty() || static_cast<std::size_t>(info.strides[0]) == sizeof(T)))
    {
        data_ptr_ = static_cast<T*>(info.ptr);
        size_ = static_cast<std::size_t>(info.shape[0]);
        // owner_ already holds the original buffer object (e.g., numpy array)
        return;
    }

    // otherwise try letting pybind11 build a contiguous T array (handles cast & endianness)
    try {
        py::array arr(info); // wrap
        using arr_t = py::array_t<T, py::array::c_style | py::array::forcecast>;
        if (py::array conv = arr_t::ensure(arr)) {
            if (conv.ndim() == 1) {
                auto info2 = conv.request();
                data_ptr_ = static_cast<T*>(info2.ptr);
                size_ = static_cast<std::size_t>(info2.shape[0]);
                owner_ = conv; // keep converted/copy array alive
                return;
            }
        }
    } catch (...) {
        // fall through to manual conversion
    }

    // last resort: perform a reasonably fast manual conversion from some common native formats.
    copy_from_buffer_info(info);
}

template <std::floating_point T, std::unsigned_integral K>
void array_series<T,K>::copy_from_buffer_info(const py::buffer_info& info)
{
    // prepare storage
    size_ = static_cast<std::size_t>(info.shape[0]);
    storage_.resize(size_);

    // Common fast typed-read cases: float32 -> T, float64 -> T, int32/int64 -> T, uint32/uint64 -> T
    // Compare format codes via py::format_descriptor<Src>::format()
    const char* fmt = info.format.c_str();
    const std::size_t itemsize = static_cast<std::size_t>(info.itemsize);

    auto try_cast_from = [&](auto* src_ptr) {
        using Src = std::remove_pointer_t<decltype(src_ptr)>;
        for (std::size_t i = 0; i < size_; ++i)
            storage_[i] = static_cast<T>(src_ptr[i]);
        data_ptr_ = storage_.data();
    };

    if (itemsize == sizeof(float) && info.format == py::format_descriptor<float>::format()) {
        const float* ptr = static_cast<const float*>(info.ptr);
        if (info.strides.empty() || static_cast<std::size_t>(info.strides[0]) == sizeof(float)) {
            try_cast_from(const_cast<float*>(ptr));
            return;
        }
    }
    if (itemsize == sizeof(double) && info.format == py::format_descriptor<double>::format()) {
        const double* ptr = static_cast<const double*>(info.ptr);
        if (info.strides.empty() || static_cast<std::size_t>(info.strides[0]) == sizeof(double)) {
            try_cast_from(const_cast<double*>(ptr));
            return;
        }
    }
    if (itemsize == sizeof(std::int32_t) && info.format == py::format_descriptor<std::int32_t>::format()) {
        const std::int32_t* ptr = static_cast<const std::int32_t*>(info.ptr);
        if (info.strides.empty() || static_cast<std::size_t>(info.strides[0]) == sizeof(std::int32_t)) {
            try_cast_from(const_cast<std::int32_t*>(ptr));
            return;
        }
    }
    if (itemsize == sizeof(std::int64_t) && info.format == py::format_descriptor<std::int64_t>::format()) {
        const std::int64_t* ptr = static_cast<const std::int64_t*>(info.ptr);
        if (info.strides.empty() || static_cast<std::size_t>(info.strides[0]) == sizeof(std::int64_t)) {
            try_cast_from(const_cast<std::int64_t*>(ptr));
            return;
        }
    }
    if (itemsize == sizeof(std::uint32_t) && info.format == py::format_descriptor<std::uint32_t>::format()) {
        const std::uint32_t* ptr = static_cast<const std::uint32_t*>(info.ptr);
        if (info.strides.empty() || static_cast<std::size_t>(info.strides[0]) == sizeof(std::uint32_t)) {
            try_cast_from(const_cast<std::uint32_t*>(ptr));
            return;
        }
    }
    if (itemsize == sizeof(std::uint64_t) && info.format == py::format_descriptor<std::uint64_t>::format()) {
        const std::uint64_t* ptr = static_cast<const std::uint64_t*>(info.ptr);
        if (info.strides.empty() || static_cast<std::size_t>(info.strides[0]) == sizeof(std::uint64_t)) {
            try_cast_from(const_cast<std::uint64_t*>(ptr));
            return;
        }
    }

    // If we got here, we couldn't do a tight bulk read. Fall back to a safe element-wise conversion
    // that avoids PyObject indexing: create a py::array wrapper and read elements via array indexing.
    try {
        py::array arr(info);
        // read element-by-element via array indexing (still avoids Python-level sequence ops)
        for (std::size_t i = 0; i < size_; ++i) {
            py::object item = arr.attr("__getitem__")(py::int_(i));
            storage_[i] = py::cast<T>(item);
        }
        data_ptr_ = storage_.data();
        return;
    } catch (...) {
        // final fallback: treat owner_ as generic sequence (this will be slow)
        copy_from_object(owner_);
    }
}

template <std::floating_point T, std::unsigned_integral K>
void array_series<T,K>::copy_from_object(py::object obj)
{
    py::sequence seq = py::reinterpret_borrow<py::sequence>(obj);
    size_ = static_cast<std::size_t>(seq.size());
    storage_.resize(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        storage_[i] = py::cast<T>(seq[py::int_(i)]);
    }
    data_ptr_ = storage_.data();
}

template <std::floating_point T, std::unsigned_integral K>
T array_series<T,K>::operator()(K n) const
{
    std::size_t idx = static_cast<std::size_t>(n);
    if (idx >= size_) throw std::out_of_range("array_series: index out of range");
    return data_ptr_[idx];
}

using T = double;
using K = size_t;

PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series + all Shanks‑type transformations";

    // series
    using SeriesBase = series_base<T, K>;
    py::class_<SeriesBase>(m, "SeriesBase")
        .def("S_n", &SeriesBase::S_n)
        .def("__call__", &SeriesBase::operator())
        .def("get_sum", &SeriesBase::get_sum)
        .def("get_x", &SeriesBase::get_x);

    #define BIND_SERIES(DERIVED, PYNAME, ...)       \
        py::class_<DERIVED, SeriesBase>(m, PYNAME).def(py::init<__VA_ARGS__>())
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
    S(SER46, pi_minus_x_2_series, "PiMinusX2Series", (T))
    S(SER47, half_multi_ln_1div2multi1minuscosx_series, "HalfMultiLn1Div2Multi1MinusCosxSeries", (T))
    S(SER48, half_minus_sinx_multi_pi_4_series, "HalfMinusSinxMultiPi4Series", (T))
    S(SER49, ln_1plussqrt1plusxsquare_minus_ln_2_series, "Ln1PlusSqrt1PlusXsquareMinusLn2Series", (T))
    S(SER50, ln_cosx_series, "LnCosxSeries", (T))
    S(SER51, ln_sinx_minus_ln_x_series, "LnSinxMinusLnXSeries", (T))
    S(SER52, pi_8_cosx_square_minus_1_div_3_cosx_series, "Pi8CosxSquareMinus1Div3CosxSeries", (T))
    S(SER53, sqrt_oneminussqrtoneminusx_div_x_series, "SqrtOneminusSqrtoneminusxDivXSeries", (T))
    S(SER54, one_minus_sqrt_1minus4x_div_2x_series, "OneMinusSqrt1minus4xDiv2xSeries", (T))
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
    using SeriesAcceleration = series_acceleration<T, K, SeriesBase*>;
    py::class_<SeriesAcceleration>(m, "SeriesAcceleration")
        .def("__call__", &SeriesAcceleration::operator());

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

    using Shanks = shanks_algorithm<T, K, SeriesBase*>;
    py::class_<Shanks, SeriesAcceleration>(m, "ShanksAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using ShanksAlt = shanks_transform_alternating<T, K, SeriesBase*>;
    py::class_<ShanksAlt, SeriesAcceleration>(m, "ShanksTransformAlternatingAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());


    using BrezinskiTheta = brezinski_theta_algorithm<T, K, SeriesBase*>;
    py::class_<BrezinskiTheta, SeriesAcceleration>(m, "BrezinskiThetaAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using ChangWynn = chang_wynn_algorithm<T, K, SeriesBase*>;
    py::class_<ChangWynn, SeriesAcceleration>(m, "ChangWynnAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using DrummondD = drummond_d_algorithm<T, K, SeriesBase*>;
    py::class_<DrummondD, SeriesAcceleration>(m, "DrummondDAlgorithm")
        .def(py::init<SeriesBase*, remainder_type, bool>(),
            py::arg("series"), py::keep_alive<1, 2>(),
            py::arg("remainder") = remainder_type::u_variant,
            py::arg("useRecFormulas") = false);

    using FordSidi2 = ford_sidi_2_algorithm<T, K, SeriesBase*>;
    py::class_<FordSidi2, SeriesAcceleration>(m, "FordSidi2Algorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using FordSidiThree = ford_sidi_3_algorithm<T, K, SeriesBase*>;
    py::class_<FordSidiThree, SeriesAcceleration>(m, "FordSidi3Algorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using Levin = levin_algorithm<T, K, SeriesBase*>;
    py::class_<Levin, SeriesAcceleration>(m, "LevinAlgorithm")
        .def(py::init<SeriesBase*, remainder_type, bool, T>(),
            py::arg("series"), py::keep_alive<1, 2>(),
            py::arg("remainder") = remainder_type::u_variant,
            py::arg("useRecFormulas") = false,
            py::arg("beta") = static_cast<T>(1));

    using LevinSidiM = levin_sidi_m_algorithm<T, K, SeriesBase*>;
    py::class_<LevinSidiM, SeriesAcceleration>(m, "LevinSidiMAlgorithm")
        .def(py::init<SeriesBase*, remainder_type, T>(),
            py::arg("series"), py::keep_alive<1, 2>(),
            py::arg("remainder") = remainder_type::u_variant,
            py::arg("gamma") = static_cast<T>(10));

    using LevinSidiS = levin_sidi_s_algorithm<T, K, SeriesBase*>;
    py::class_<LevinSidiS, SeriesAcceleration>(m, "LevinSidiSAlgorithm")
        .def(py::init<SeriesBase*, remainder_type, bool, T>(),
            py::arg("series"), py::keep_alive<1, 2>(),
            py::arg("remainder") = remainder_type::u_variant,
            py::arg("useRecFormulas") = false,
            py::arg("parameter") = static_cast<T>(1));

    using LubkinW = lubkin_w_algorithm<T, K, SeriesBase*>;
    py::class_<LubkinW, SeriesAcceleration>(m, "LubkinWAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using WhynnRho = wynn_rho_algorithm<T, K, SeriesBase*>;
    py::class_<WhynnRho, SeriesAcceleration>(m, "WhynnRhoAlgorithm")
        .def(py::init<SeriesBase*, numerator_type, T, T>(),
            py::arg("series"), py::keep_alive<1, 2>(),
            py::arg("numerator") = numerator_type::rho_variant,
            py::arg("gamma") = static_cast<T>(1),
            py::arg("RHO") = static_cast<T>(0));

    using Richardson = richardson_algorithm<T, K, SeriesBase*>;
    py::class_<Richardson, SeriesAcceleration>(m, "RichardsonAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using Weniger = weniger_algorithm<T, K, SeriesBase*>;
    py::class_<Weniger, SeriesAcceleration>(m, "WenigerAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using WynnEpsilon = wynn_epsilon_1_algorithm<T, K, SeriesBase*>;
    py::class_<WynnEpsilon, SeriesAcceleration>(m, "WynnEpsilonAlgorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using WynnEpsilon2 = wynn_epsilon_2_algorithm<T, K, SeriesBase*>;
    py::class_<WynnEpsilon2, SeriesAcceleration>(m, "WynnEpsilon2Algorithm")
        .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

    using WynnEpsilon3 = wynn_epsilon_3_algorithm<T, K, SeriesBase*>;
    py::class_<WynnEpsilon3, SeriesAcceleration>(m, "WynnEpsilon3Algorithm")
        .def(py::init<SeriesBase*, T>(),
            py::arg("series"), py::keep_alive<1, 2>(),
            py::arg("epsilon_threshold") = static_cast<T>(1e-3));

    using ArraySeries = array_series<T, K>;
    py::class_<ArraySeries, SeriesBase>(m, "ArraySeries")
        .def(py::init<py::buffer>(), py::arg("buffer"), "Construct from a Python buffer (numpy array/memoryview). Zero-copy when possible.")
        .def(py::init<py::object>(), py::arg("sequence"), "Construct from a Python sequence (will copy).")
        .def("size", &ArraySeries::size);
}
