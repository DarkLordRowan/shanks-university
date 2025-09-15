/**
 * @file py.cpp
 * @brief pybind11 bindings with support for double and arbitrary-precision float_precision
 */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <type_traits>
#include <string>

#include "series.h"
#include "methods.hpp"
#include "libs/arbitrary_arithmetics/fprecision.h"

using namespace series;
namespace py = pybind11;

template <typename T>
T py_to_T(py::handle obj) {
    try { return py::cast<T>(obj); } catch(...) {}

    if constexpr (std::is_same_v<T, float_precision>) {
        // Try numeric double -> T(double)
        try {
            double d = py::cast<double>(obj);
            return T(d);
        } catch(...) {}

        // Try string -> T(std::string)
        try {
            std::string s = py::cast<std::string>(obj);
            return T(s);
        } catch(...) {}
    }

    throw std::runtime_error("py_to_T: cannot convert Python object to target type");
}

template <typename T>
static bool try_numpy_zero_copy(py::object obj, T*& out_ptr, std::size_t& out_size, py::object& out_owner) {
    if constexpr (std::is_floating_point_v<T>) {
        // Ensure it's an array-like object
        py::array arr = py::array::ensure(obj);
        if (!arr) return false;
        using arr_t = py::array_t<T, py::array::c_style | py::array::forcecast>;
        py::array conv = arr_t::ensure(arr);
        if (!conv) return false;
        if (conv.ndim() != 1) return false;
        auto info = conv.request();
        out_ptr = static_cast<T*>(info.ptr);
        out_size = static_cast<std::size_t>(info.shape[0]);
        out_owner = conv; // keep alive
        return true;
    }
    return false;
}

template <typename T>
static void copy_iterable_to_storage(py::object obj, std::vector<T>& storage, py::object& owner) {
    storage.clear();
    // Try iteration; if object is not iterable, reinterpret as sequence and index
    try {
        for (auto item : py::reinterpret_borrow<py::iterable>(obj)) {
            storage.push_back(py_to_T<T>(item));
        }
        owner = obj;
        return;
    } catch(...) {
        // fall through to sequence-index approach
    }

    try {
        py::sequence seq = py::reinterpret_borrow<py::sequence>(obj);
        storage.reserve(static_cast<std::size_t>(seq.size()));
        for (std::size_t i = 0; i < static_cast<std::size_t>(seq.size()); ++i)
            storage.push_back(py_to_T<T>(seq[py::int_(i)]));
        owner = obj;
        return;
    } catch(...) {
        throw std::runtime_error("copy_iterable_to_storage: object is not iterable/sequence");
    }
}

template <typename T, std::unsigned_integral K>
class array_series : public series::series_base<T,K> {
public:
    array_series() = delete;
    explicit array_series(py::buffer buffer);
    explicit array_series(py::object seq);

    [[nodiscard]] virtual T operator()(K n) const override {
        auto idx = static_cast<std::size_t>(n);
        if (idx >= size_) throw std::out_of_range("array_series: index out of range");
        return data_ptr_[idx];
    }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

private:
    void build_from_buffer(py::buffer buffer);
    void copy_from_object(py::object obj);

    py::object owner_;       ///< keeps the Python object alive if data_ptr_ points into it
    T* data_ptr_ = nullptr;  ///< points into either owner_'s buffer or storage_.data()
    std::vector<T> storage_; ///< owned storage when zero-copy impossible
    std::size_t size_ = 0;
};

template <typename T, std::unsigned_integral K>
array_series<T,K>::array_series(py::buffer buffer)
: owner_(buffer)
{
    build_from_buffer(buffer);
}

template <typename T, std::unsigned_integral K>
array_series<T,K>::array_series(py::object seq)
: owner_(seq)
{
    // Fast path for builtin floats from numpy-like objects
    if (try_numpy_zero_copy<T>(owner_, data_ptr_, size_, owner_)) return;

    // Fallback: iterate / copy elements into storage_
    copy_iterable_to_storage<T>(owner_, storage_, owner_);
    data_ptr_ = storage_.data();
    size_ = storage_.size();
}

template <typename T, std::unsigned_integral K>
void array_series<T,K>::build_from_buffer(py::buffer buffer)
{
    py::buffer_info info = buffer.request();
    if (info.ndim != 1) {
        // not 1-D buffer -> fall back to generic sequence copy
        copy_from_object(owner_);
        return;
    }

    // If builtin floating type: let pybind handle conversion/forcecast where possible
    if constexpr (std::is_floating_point_v<T>) {
        try {
            // Wrap the buffer as an array and try to ensure a contiguous T array
            py::array arr(info);
            using arr_t = py::array_t<T, py::array::c_style | py::array::forcecast>;
            if (py::array conv = arr_t::ensure(arr)) {
                if (conv.ndim() == 1) {
                    auto info2 = conv.request();
                    data_ptr_ = static_cast<T*>(info2.ptr);
                    size_ = static_cast<std::size_t>(info2.shape[0]);
                    owner_ = conv; // keep converted/copy array alive if needed
                    return;
                }
            }
        } catch(...) {
            // fallthrough to element-wise copy
        }

        // Last resort: element-wise copy using py::array wrapper (handles object dtypes)
        try {
            py::array arr(info);
            size_ = static_cast<std::size_t>(info.shape[0]);
            storage_.resize(size_);
            for (std::size_t i = 0; i < size_; ++i) {
                py::object item = arr.attr("__getitem__")(py::int_(i));
                storage_[i] = py_to_T<T>(item);
            }
            data_ptr_ = storage_.data();
            owner_ = arr;
            return;
        } catch(...) {
            copy_from_object(owner_);
            return;
        }
    } else {
        // Non-builtin numeric types: don't interpret raw bytes — do element-wise conversion
        try {
            py::array arr(info);
            size_ = static_cast<std::size_t>(info.shape[0]);
            storage_.resize(size_);
            for (std::size_t i = 0; i < size_; ++i) {
                py::object item = arr.attr("__getitem__")(py::int_(i));
                storage_[i] = py_to_T<T>(item);
            }
            data_ptr_ = storage_.data();
            owner_ = arr;
            return;
        } catch(...) {
            copy_from_object(owner_);
            return;
        }
    }
}

template <typename T, std::unsigned_integral K>
void array_series<T,K>::copy_from_object(py::object obj)
{
    // Generic sequence/iterable copy (works for lists, tuples, generators, etc.)
    copy_iterable_to_storage<T>(obj, storage_, owner_);
    data_ptr_ = storage_.data();
    size_ = storage_.size();
}

using K = size_t;
PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series + all Shanks‑type transformations (double + float_precision)";

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

    // double
    {
        using T = double;
        using SeriesBase = series_base<T, K>;
        py::class_<SeriesBase>(m, "SeriesBaseF64")
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

        S(SER1, exp_series, "ExpSeriesF64", (T))
        S(SER2, cos_series, "CosSeriesF64", (T))
        S(SER3, sin_series, "SinSeriesF64", (T))
        S(SER4, cosh_series, "CoshSeriesF64", (T))
        S(SER5, sinh_series, "SinhSeriesF64", (T))
        S(SER6, bin_series, "BinSeriesF64", (T,T))
        S(SER7, four_arctan_series, "FourArctanSeriesF64", (T))
        S(SER8, ln1mx_series, "Ln1mxSeriesF64", (T))
        S(SER9, mean_sinh_sin_series, "MeanSinhSinSeriesF64", (T))
        S(SER10, exp_squared_erf_series, "ExpSquaredErfSeriesF64", (T))
        S(SER11, xmb_Jb_two_series, "XmbJbTwoSeriesF64", (T,K))
        S(SER12, half_asin_two_x_series, "HalfAsinTwoXSeriesF64", (T))
        S(SER13, inverse_1mx_series, "Inverse1mxSeriesF64", (T))
        S(SER14, x_1mx_squared_series, "X_1mx_SquaredSeriesF64", (T))
        S(SER15, erf_series, "ErfSeriesF64", (T))
        S(SER16, m_fact_1mx_mp1_inverse_series, "MFact1mxMp1InverseSeriesF64", (T,K))
        S(SER17, inverse_sqrt_1m4x_series, "InvSqrt1m4xSeriesF64", (T))
        S(SER18, one_twelfth_3x2_pi2_series, "OneTwelfth3x2Pi2SeriesF64", (T))
        S(SER19, x_twelfth_x2_pi2_series, "XTwelfthX2Pi2SeriesF64", (T))
        S(SER20, ln2_series, "Ln2SeriesF64", (T))
        S(SER21, one_series, "OneSeriesF64", (T))
        S(SER22, minus_one_quarter_series, "MinusOneQuarterSeriesF64", (T))
        S(SER23, pi_3_series, "Pi3SeriesF64", (T))
        S(SER24, pi_4_series, "Pi4SeriesF64", (T))
        S(SER25, pi_squared_6_minus_one_series, "PiSquared6MinusOneSeriesF64", (T))
        S(SER26, three_minus_pi_series, "ThreeMinusPiSeriesF64", (T))
        S(SER27, one_twelfth_series, "OneTwelfthSeriesF64", (T))
        S(SER28, eighth_pi_m_one_third_series, "EighthPiMOneThirdSeriesF64", (T))
        S(SER29, one_third_pi_squared_m_nine_series, "OneThirdPiSquaredMNineSeriesF64", (T))
        S(SER30, four_ln2_m_3_series, "FourLn2M3SeriesF64", (T))
        S(SER31, exp_m_cos_x_sinsin_x_series, "ExpMCosXSinsinXSeriesF64", (T))
        S(SER32, pi_four_minus_ln2_halfed_series, "PiFourMinusLn2HalfedSeriesF64", (T))
        S(SER33, five_pi_twelve_series, "FivePiTwelveSeriesF64", (T))
        S(SER34, x_two_series, "XTwoSeriesF64", (T))
        S(SER35, pi_six_min_half_series, "PiSixMinHalfSeriesF64", (T))
        S(SER36, x_two_throught_squares_series, "XTwoThroughtSquaresSeriesF64", (T))
        S(SER37, minus_one_ned_in_n_series, "MinusOneNedInNSeriesF64", (T))
        S(SER38, minus_one_n_fact_n_in_n_series, "MinusOneNFactNInNSeriesF64", (T))
        S(SER39, ln_x_plus_one_x_minus_one_halfed_series, "LnXPlusOneXMinusOneHalfedSeriesF64", (T))
        S(SER40, two_arcsin_square_x_halfed_series, "TwoArcsinSquareXHalfedSeriesF64", (T))
        S(SER41, pi_squared_twelve_series, "PiSquaredTwelveSeriesF64", (T))
        S(SER42, pi_cubed_32_series, "PiCubed32SeriesF64", (T))
        S(SER43, minus_three_plus_ln3_three_devided_two_plus_two_ln2_series, "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesF64", (T))
        S(SER44, two_ln2_series, "TwoLn2SeriesF64", (T))
        S(SER45, pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series, "PiXMultiE_XpiPlusEMinusXpiDividedE_XpiMinusEMinusXpiMinusOneSeriesF64", (T))
        S(SER46, pi_minus_x_2_series, "PiMinusX2SeriesF64", (T))
        S(SER47, half_multi_ln_1div2multi1minuscosx_series, "HalfMultiLn1Div2Multi1MinusCosxSeriesF64", (T))
        S(SER48, half_minus_sinx_multi_pi_4_series, "HalfMinusSinxMultiPi4SeriesF64", (T))
        S(SER49, ln_1plussqrt1plusxsquare_minus_ln_2_series, "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesF64", (T))
        S(SER50, ln_cosx_series, "LnCosxSeriesF64", (T))
        S(SER51, ln_sinx_minus_ln_x_series, "LnSinxMinusLnXSeriesF64", (T))
        S(SER52, pi_8_cosx_square_minus_1_div_3_cosx_series, "Pi8CosxSquareMinus1Div3CosxSeriesF64", (T))
        S(SER53, sqrt_oneminussqrtoneminusx_div_x_series, "SqrtOneminusSqrtoneminusxDivXSeriesF64", (T))
        S(SER54, one_minus_sqrt_1minus4x_div_2x_series, "OneMinusSqrt1minus4xDiv2xSeriesF64", (T))
        S(SER55, arcsin_x_minus_x_series, "ArcsinXMinusXSeriesF64", (T))
        S(SER56, pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series, "PiXMinusXSquareAndXSquareMinusThreePiXPlusTwoPiSquareSeriesF64", (T))
        S(SER57, abs_sin_x_minus_2_div_pi_series, "AbsSinXMinus2DivPiSeriesF64", (T))
        S(SER58, pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series, "PiMinus3pi4AndPiMinusXMinus3pi4SeriesF64", (T))
        S(SER59, minus_3_div_4_or_x_minus_3_div_4_series, "Minus3Div4OrXMinus3Div4SeriesF64", (T))
        S(SER60, ten_minus_x_series, "TenMinusXSeriesF64", (T))
        S(SER61, x_series, "XSeriesF64", (T))
        S(SER62, minus_x_minus_pi_4_or_minus_pi_4_series, "MinusXMinusPi4OrMinusPi4SeriesF64", (T))
        S(SER63, one_div_two_minus_x_multi_three_plus_x_series, "OneDivTwoMinusXMultiThreePlusXSeriesF64", (T))
        S(SER64, Si_x_series, "SiXSeriesF64", (T))
        S(SER65, Ci_x_series, "CiXSeriesF64", (T))
        S(SER66, Riemann_zeta_func_series, "RiemannZetaFuncSeriesF64", (T))
        S(SER67, Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series, "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesF64", (T))
        S(SER68, xsquareplus3_div_xsquareplus2multix_minus_1_series, "Xsquareplus3DivXsquareplus2multixMinus1SeriesF64", (T))
        S(SER69, arcsin_x_series, "ArcsinXSeriesF64", (T))
        S(SER70, arctg_x_series, "ArctgXSeriesF64", (T))
        S(SER71, K_x_series, "KXSeriesF64", (T))
        S(SER72, E_x_series, "EXSeriesF64", (T))
        S(SER73, sqrt_1plusx_series, "Sqrt1plusXSeriesF64", (T))
        S(SER74, Lambert_W_func_series, "LambertWFuncSeriesF64", (T))
        S(SER75, Incomplete_Gamma_func_series, "IncompleteGammaFuncSeriesF64", (T,T))
        S(SER76, Series_with_ln_number1_series, "SeriesWithLnNumber1F64", (T))
        S(SER77, Series_with_ln_number2_series, "SeriesWithLnNumber2F64", (T))
        S(SER78, pi_series, "PiSeriesF64", (T))
        S(SER79, x_min_sqrt_x_series, "XMinSqrtXSeriesF64", (T))
        S(SER80, arctan_x2_series, "ArctanX2SeriesF64", (T))
        S(SER81, ln1px4_series, "Ln1px4SeriesF64", (T))
        S(SER82, sin_x2_series, "SinX2SeriesF64", (T))
        S(SER83, arctan_x3_series, "ArctanX3SeriesF64", (T))
        S(SER84, arcsin_x2_series, "ArcsinX2SeriesF64", (T))
        S(SER85, ln1_m_x2_series, "Ln1MinusX2SeriesF64", (T))
        S(SER86, artanh_x_series, "ArtanhXSeriesF64", (T))
        S(SER87, arcsinh_x_series, "ArcsinhXSeriesF64", (T))
        S(SER88, cos_x2_series, "CosX2SeriesF64", (T))
        S(SER89, sinh_x2_series, "SinhX2SeriesF64", (T))
        S(SER90, arctanh_x2_series, "ArctanhX2SeriesF64", (T))
        S(SER91, cos3xmin1_div_xsqare_series, "Cos3xMinus1DivXsquareSeriesF64", (T))
        S(SER92, two_degree_x_series, "TwoDegreeXSeriesF64", (T))
        S(SER93, sqrt_1plusx_min_1_min_x_div_2_series, "Sqrt1plusXMinus1MinusXDiv2SeriesF64", (T))
        S(SER94, ln13_min_ln7_div_7_series, "Ln13MinusLn7Div7SeriesF64", (T))
        S(SER95, Ja_x_series, "JaXSeriesF64", (T,T))
        S(SER96, one_div_sqrt2_sin_xdivsqrt2_series, "OneDivSqrt2SinXDivSqrt2SeriesF64", (T))
        S(SER97, ln_1plusx_div_1plusx2_series, "Ln1plusXDiv1plusX2SeriesF64", (T))
        S(SER98, cos_sqrt_x_series, "CosSqrtXSeriesF64", (T))
        S(SER99, ln_1_plus_x3_series, "Ln1PlusX3SeriesF64", (T))
        S(SER100, x_div_1minx_series, "XDiv1minXSeriesF64", (T))
        S(SER101, x_div_1minx2_series, "XDiv1minX2SeriesF64", (T))
        S(SER102, gamma_series, "GammaSeriesF64", (T,T))

        #undef BIND_SERIES
        #undef UNPAREN
        #undef S

        // series_acceleration
        using SeriesAcceleration = series_acceleration<T, K, SeriesBase*>;
        py::class_<SeriesAcceleration>(m, "SeriesAccelerationF64")
            .def("__call__", &SeriesAcceleration::operator());

        using Shanks = shanks_algorithm<T, K, SeriesBase*>;
        py::class_<Shanks, SeriesAcceleration>(m, "ShanksAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using ShanksAlt = shanks_transform_alternating<T, K, SeriesBase*>;
        py::class_<ShanksAlt, SeriesAcceleration>(m, "ShanksTransformAlternatingAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());


        using BrezinskiTheta = brezinski_theta_algorithm<T, K, SeriesBase*>;
        py::class_<BrezinskiTheta, SeriesAcceleration>(m, "BrezinskiThetaAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using ChangWynn = chang_wynn_algorithm<T, K, SeriesBase*>;
        py::class_<ChangWynn, SeriesAcceleration>(m, "ChangWynnAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using DrummondD = drummond_d_algorithm<T, K, SeriesBase*>;
        py::class_<DrummondD, SeriesAcceleration>(m, "DrummondDAlgorithmF64")
            .def(py::init<SeriesBase*, remainder_type, bool>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("useRecFormulas") = false);

        using FordSidi2 = ford_sidi_2_algorithm<T, K, SeriesBase*>;
        py::class_<FordSidi2, SeriesAcceleration>(m, "FordSidi2AlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using FordSidiThree = ford_sidi_3_algorithm<T, K, SeriesBase*>;
        py::class_<FordSidiThree, SeriesAcceleration>(m, "FordSidi3AlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using Levin = levin_algorithm<T, K, SeriesBase*>;
        py::class_<Levin, SeriesAcceleration>(m, "LevinAlgorithmF64")
            .def(py::init<SeriesBase*, remainder_type, bool, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("useRecFormulas") = false,
                py::arg("beta") = static_cast<T>(1));

        using LevinSidiM = levin_sidi_m_algorithm<T, K, SeriesBase*>;
        py::class_<LevinSidiM, SeriesAcceleration>(m, "LevinSidiMAlgorithmF64")
            .def(py::init<SeriesBase*, remainder_type, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("gamma") = static_cast<T>(10));

        using LevinSidiS = levin_sidi_s_algorithm<T, K, SeriesBase*>;
        py::class_<LevinSidiS, SeriesAcceleration>(m, "LevinSidiSAlgorithmF64")
            .def(py::init<SeriesBase*, remainder_type, bool, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("useRecFormulas") = false,
                py::arg("parameter") = static_cast<T>(1));

        using LubkinW = lubkin_w_algorithm<T, K, SeriesBase*>;
        py::class_<LubkinW, SeriesAcceleration>(m, "LubkinWAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WhynnRho = wynn_rho_algorithm<T, K, SeriesBase*>;
        py::class_<WhynnRho, SeriesAcceleration>(m, "WhynnRhoAlgorithmF64")
            .def(py::init<SeriesBase*, numerator_type, T, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("numerator") = numerator_type::rho_variant,
                py::arg("gamma") = static_cast<T>(1),
                py::arg("RHO") = static_cast<T>(0));

        using Richardson = richardson_algorithm<T, K, SeriesBase*>;
        py::class_<Richardson, SeriesAcceleration>(m, "RichardsonAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using Weniger = weniger_algorithm<T, K, SeriesBase*>;
        py::class_<Weniger, SeriesAcceleration>(m, "WenigerAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WynnEpsilon = wynn_epsilon_1_algorithm<T, K, SeriesBase*>;
        py::class_<WynnEpsilon, SeriesAcceleration>(m, "WynnEpsilonAlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WynnEpsilon2 = wynn_epsilon_2_algorithm<T, K, SeriesBase*>;
        py::class_<WynnEpsilon2, SeriesAcceleration>(m, "WynnEpsilon2AlgorithmF64")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WynnEpsilon3 = wynn_epsilon_3_algorithm<T, K, SeriesBase*>;
        py::class_<WynnEpsilon3, SeriesAcceleration>(m, "WynnEpsilon3AlgorithmF64")
            .def(py::init<SeriesBase*, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("epsilon_threshold") = static_cast<T>(1e-3));

        using ArraySeries = array_series<T, K>;
        py::class_<ArraySeries, SeriesBase>(m, "ArraySeriesF64")
            .def(py::init<py::buffer>(), py::arg("buffer"), "Construct from a Python buffer (numpy array/memoryview). Zero-copy when possible.")
            .def(py::init<py::object>(), py::arg("sequence"), "Construct from a Python sequence (will copy).")
            .def("size", &ArraySeries::size);
    }

    // Arbitrary
    {
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
            .def("__setstate__", [](float_precision &self, const std::string &state) { new (&self) float_precision(state); })
            ;
        #undef ADD_NUM_BINOP
        #undef ADD_NUM_INPLACE
        #undef ADD_NUM_CMP

        using T = float_precision;
        using SeriesBase = series_base<T, K>;
        py::class_<SeriesBase>(m, "SeriesBaseArb")
            .def("S_n", &SeriesBase::S_n)
            .def("__call__", &SeriesBase::operator())
            .def("get_sum", &SeriesBase::get_sum)
            .def("get_x", &SeriesBase::get_x);

        #define BIND_SERIES_(DERIVED, PYNAME, ...)       \
            py::class_<DERIVED, SeriesBase>(m, PYNAME).def(py::init<__VA_ARGS__>())
        #define UNPAREN_(...) __VA_ARGS__
        #define S_(Alias, Impl, NameStr, BIND_ARGS) \
            using Alias = Impl<T, K>; \
            BIND_SERIES_(Alias, NameStr, UNPAREN_ BIND_ARGS);

        S_(SER1, exp_series, "ExpSeriesArb", (T))
        S_(SER2, cos_series, "CosSeriesArb", (T))
        S_(SER3, sin_series, "SinSeriesArb", (T))
        S_(SER4, cosh_series, "CoshSeriesArb", (T))
        S_(SER5, sinh_series, "SinhSeriesArb", (T))
        S_(SER6, bin_series, "BinSeriesArb", (T,T))
        S_(SER7, four_arctan_series, "FourArctanSeriesArb", (T))
        S_(SER8, ln1mx_series, "Ln1mxSeriesArb", (T))
        S_(SER9, mean_sinh_sin_series, "MeanSinhSinSeriesArb", (T))
        S_(SER10, exp_squared_erf_series, "ExpSquaredErfSeriesArb", (T))
        S_(SER11, xmb_Jb_two_series, "XmbJbTwoSeriesArb", (T,K))
        S_(SER12, half_asin_two_x_series, "HalfAsinTwoXSeriesArb", (T))
        S_(SER13, inverse_1mx_series, "Inverse1mxSeriesArb", (T))
        S_(SER14, x_1mx_squared_series, "X_1mx_SquaredSeriesArb", (T))
        S_(SER15, erf_series, "ErfSeriesArb", (T))
        S_(SER16, m_fact_1mx_mp1_inverse_series, "MFact1mxMp1InverseSeriesArb", (T,K))
        S_(SER17, inverse_sqrt_1m4x_series, "InvSqrt1m4xSeriesArb", (T))
        S_(SER18, one_twelfth_3x2_pi2_series, "OneTwelfth3x2Pi2SeriesArb", (T))
        S_(SER19, x_twelfth_x2_pi2_series, "XTwelfthX2Pi2SeriesArb", (T))
        S_(SER20, ln2_series, "Ln2SeriesArb", (T))
        S_(SER21, one_series, "OneSeriesArb", (T))
        S_(SER22, minus_one_quarter_series, "MinusOneQuarterSeriesArb", (T))
        S_(SER23, pi_3_series, "Pi3SeriesArb", (T))
        S_(SER24, pi_4_series, "Pi4SeriesArb", (T))
        S_(SER25, pi_squared_6_minus_one_series, "PiSquared6MinusOneSeriesArb", (T))
        S_(SER26, three_minus_pi_series, "ThreeMinusPiSeriesArb", (T))
        S_(SER27, one_twelfth_series, "OneTwelfthSeriesArb", (T))
        S_(SER28, eighth_pi_m_one_third_series, "EighthPiMOneThirdSeriesArb", (T))
        S_(SER29, one_third_pi_squared_m_nine_series, "OneThirdPiSquaredMNineSeriesArb", (T))
        S_(SER30, four_ln2_m_3_series, "FourLn2M3SeriesArb", (T))
        S_(SER31, exp_m_cos_x_sinsin_x_series, "ExpMCosXSinsinXSeriesArb", (T))
        S_(SER32, pi_four_minus_ln2_halfed_series, "PiFourMinusLn2HalfedSeriesArb", (T))
        S_(SER33, five_pi_twelve_series, "FivePiTwelveSeriesArb", (T))
        S_(SER34, x_two_series, "XTwoSeriesArb", (T))
        S_(SER35, pi_six_min_half_series, "PiSixMinHalfSeriesArb", (T))
        S_(SER36, x_two_throught_squares_series, "XTwoThroughtSquaresSeriesArb", (T))
        S_(SER37, minus_one_ned_in_n_series, "MinusOneNedInNSeriesArb", (T))
        S_(SER38, minus_one_n_fact_n_in_n_series, "MinusOneNFactNInNSeriesArb", (T))
        S_(SER39, ln_x_plus_one_x_minus_one_halfed_series, "LnXPlusOneXMinusOneHalfedSeriesArb", (T))
        S_(SER40, two_arcsin_square_x_halfed_series, "TwoArcsinSquareXHalfedSeriesArb", (T))
        S_(SER41, pi_squared_twelve_series, "PiSquaredTwelveSeriesArb", (T))
        S_(SER42, pi_cubed_32_series, "PiCubed32SeriesArb", (T))
        S_(SER43, minus_three_plus_ln3_three_devided_two_plus_two_ln2_series, "MinusThreePlusLn3ThreeDividedTwoPlusTwoLn2SeriesArb", (T))
        S_(SER44, two_ln2_series, "TwoLn2SeriesArb", (T))
        S_(SER45, pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series, "PiXMultiE_XpiPlusEMinusXpiDividedE_XpiMinusEMinusXpiMinusOneSeriesArb", (T))
        S_(SER46, pi_minus_x_2_series, "PiMinusX2SeriesArb", (T))
        S_(SER47, half_multi_ln_1div2multi1minuscosx_series, "HalfMultiLn1Div2Multi1MinusCosxSeriesArb", (T))
        S_(SER48, half_minus_sinx_multi_pi_4_series, "HalfMinusSinxMultiPi4SeriesArb", (T))
        S_(SER49, ln_1plussqrt1plusxsquare_minus_ln_2_series, "Ln1PlusSqrt1PlusXsquareMinusLn2SeriesArb", (T))
        S_(SER50, ln_cosx_series, "LnCosxSeriesArb", (T))
        S_(SER51, ln_sinx_minus_ln_x_series, "LnSinxMinusLnXSeriesArb", (T))
        S_(SER52, pi_8_cosx_square_minus_1_div_3_cosx_series, "Pi8CosxSquareMinus1Div3CosxSeriesArb", (T))
        S_(SER53, sqrt_oneminussqrtoneminusx_div_x_series, "SqrtOneminusSqrtoneminusxDivXSeriesArb", (T))
        S_(SER54, one_minus_sqrt_1minus4x_div_2x_series, "OneMinusSqrt1minus4xDiv2xSeriesArb", (T))
        S_(SER55, arcsin_x_minus_x_series, "ArcsinXMinusXSeriesArb", (T))
        S_(SER56, pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series, "PiXMinusXSquareAndXSquareMinusThreePiXPlusTwoPiSquareSeriesArb", (T))
        S_(SER57, abs_sin_x_minus_2_div_pi_series, "AbsSinXMinus2DivPiSeriesArb", (T))
        S_(SER58, pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series, "PiMinus3pi4AndPiMinusXMinus3pi4SeriesArb", (T))
        S_(SER59, minus_3_div_4_or_x_minus_3_div_4_series, "Minus3Div4OrXMinus3Div4SeriesArb", (T))
        S_(SER60, ten_minus_x_series, "TenMinusXSeriesArb", (T))
        S_(SER61, x_series, "XSeriesArb", (T))
        S_(SER62, minus_x_minus_pi_4_or_minus_pi_4_series, "MinusXMinusPi4OrMinusPi4SeriesArb", (T))
        S_(SER63, one_div_two_minus_x_multi_three_plus_x_series, "OneDivTwoMinusXMultiThreePlusXSeriesArb", (T))
        S_(SER64, Si_x_series, "SiXSeriesArb", (T))
        S_(SER65, Ci_x_series, "CiXSeriesArb", (T))
        S_(SER66, Riemann_zeta_func_series, "RiemannZetaFuncSeriesArb", (T))
        S_(SER67, Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series, "RiemannZetaFuncXmin1DivRiemannZetaFuncXSeriesArb", (T))
        S_(SER68, xsquareplus3_div_xsquareplus2multix_minus_1_series, "Xsquareplus3DivXsquareplus2multixMinus1SeriesArb", (T))
        S_(SER69, arcsin_x_series, "ArcsinXSeriesArb", (T))
        S_(SER70, arctg_x_series, "ArctgXSeriesArb", (T))
        S_(SER71, K_x_series, "KXSeriesArb", (T))
        S_(SER72, E_x_series, "EXSeriesArb", (T))
        S_(SER73, sqrt_1plusx_series, "Sqrt1plusXSeriesArb", (T))
        S_(SER74, Lambert_W_func_series, "LambertWFuncSeriesArb", (T))
        S_(SER75, Incomplete_Gamma_func_series, "IncompleteGammaFuncSeriesArb", (T,T))
        S_(SER76, Series_with_ln_number1_series, "SeriesWithLnNumber1Arb", (T))
        S_(SER77, Series_with_ln_number2_series, "SeriesWithLnNumber2Arb", (T))
        S_(SER78, pi_series, "PiSeriesArb", (T))
        S_(SER79, x_min_sqrt_x_series, "XMinSqrtXSeriesArb", (T))
        S_(SER80, arctan_x2_series, "ArctanX2SeriesArb", (T))
        S_(SER81, ln1px4_series, "Ln1px4SeriesArb", (T))
        S_(SER82, sin_x2_series, "SinX2SeriesArb", (T))
        S_(SER83, arctan_x3_series, "ArctanX3SeriesArb", (T))
        S_(SER84, arcsin_x2_series, "ArcsinX2SeriesArb", (T))
        S_(SER85, ln1_m_x2_series, "Ln1MinusX2SeriesArb", (T))
        S_(SER86, artanh_x_series, "ArtanhXSeriesArb", (T))
        S_(SER87, arcsinh_x_series, "ArcsinhXSeriesArb", (T))
        S_(SER88, cos_x2_series, "CosX2SeriesArb", (T))
        S_(SER89, sinh_x2_series, "SinhX2SeriesArb", (T))
        S_(SER90, arctanh_x2_series, "ArctanhX2SeriesArb", (T))
        S_(SER91, cos3xmin1_div_xsqare_series, "Cos3xMinus1DivXsquareSeriesArb", (T))
        S_(SER92, two_degree_x_series, "TwoDegreeXSeriesArb", (T))
        S_(SER93, sqrt_1plusx_min_1_min_x_div_2_series, "Sqrt1plusXMinus1MinusXDiv2SeriesArb", (T))
        S_(SER94, ln13_min_ln7_div_7_series, "Ln13MinusLn7Div7SeriesArb", (T))
        S_(SER95, Ja_x_series, "JaXSeriesArb", (T,T))
        S_(SER96, one_div_sqrt2_sin_xdivsqrt2_series, "OneDivSqrt2SinXDivSqrt2SeriesArb", (T))
        S_(SER97, ln_1plusx_div_1plusx2_series, "Ln1plusXDiv1plusX2SeriesArb", (T))
        S_(SER98, cos_sqrt_x_series, "CosSqrtXSeriesArb", (T))
        S_(SER99, ln_1_plus_x3_series, "Ln1PlusX3SeriesArb", (T))
        S_(SER100, x_div_1minx_series, "XDiv1minXSeriesArb", (T))
        S_(SER101, x_div_1minx2_series, "XDiv1minX2SeriesArb", (T))
        S_(SER102, gamma_series, "GammaSeriesArb", (T,T))

        #undef BIND_SERIES_
        #undef UNPAREN_
        #undef S_

        // series_acceleration (arbitrary precision)
        using SeriesAcceleration = series_acceleration<T, K, SeriesBase*>;
        py::class_<SeriesAcceleration>(m, "SeriesAccelerationArb")
            .def("__call__", &SeriesAcceleration::operator());

        using Shanks = shanks_algorithm<T, K, SeriesBase*>;
        py::class_<Shanks, SeriesAcceleration>(m, "ShanksAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using ShanksAlt = shanks_transform_alternating<T, K, SeriesBase*>;
        py::class_<ShanksAlt, SeriesAcceleration>(m, "ShanksTransformAlternatingAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());


        using BrezinskiTheta = brezinski_theta_algorithm<T, K, SeriesBase*>;
        py::class_<BrezinskiTheta, SeriesAcceleration>(m, "BrezinskiThetaAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using ChangWynn = chang_wynn_algorithm<T, K, SeriesBase*>;
        py::class_<ChangWynn, SeriesAcceleration>(m, "ChangWynnAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using DrummondD = drummond_d_algorithm<T, K, SeriesBase*>;
        py::class_<DrummondD, SeriesAcceleration>(m, "DrummondDAlgorithmArb")
            .def(py::init<SeriesBase*, remainder_type, bool>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("useRecFormulas") = false);

        using FordSidi2 = ford_sidi_2_algorithm<T, K, SeriesBase*>;
        py::class_<FordSidi2, SeriesAcceleration>(m, "FordSidi2AlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using FordSidiThree = ford_sidi_3_algorithm<T, K, SeriesBase*>;
        py::class_<FordSidiThree, SeriesAcceleration>(m, "FordSidi3AlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using Levin = levin_algorithm<T, K, SeriesBase*>;
        py::class_<Levin, SeriesAcceleration>(m, "LevinAlgorithmArb")
            .def(py::init<SeriesBase*, remainder_type, bool, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("useRecFormulas") = false,
                py::arg("beta") = static_cast<T>(1));

        using LevinSidiM = levin_sidi_m_algorithm<T, K, SeriesBase*>;
        py::class_<LevinSidiM, SeriesAcceleration>(m, "LevinSidiMAlgorithmArb")
            .def(py::init<SeriesBase*, remainder_type, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("gamma") = static_cast<T>(10));

        using LevinSidiS = levin_sidi_s_algorithm<T, K, SeriesBase*>;
        py::class_<LevinSidiS, SeriesAcceleration>(m, "LevinSidiSAlgorithmArb")
            .def(py::init<SeriesBase*, remainder_type, bool, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("remainder") = remainder_type::u_variant,
                py::arg("useRecFormulas") = false,
                py::arg("parameter") = static_cast<T>(1));

        using LubkinW = lubkin_w_algorithm<T, K, SeriesBase*>;
        py::class_<LubkinW, SeriesAcceleration>(m, "LubkinWAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WhynnRho = wynn_rho_algorithm<T, K, SeriesBase*>;
        py::class_<WhynnRho, SeriesAcceleration>(m, "WhynnRhoAlgorithmArb")
            .def(py::init<SeriesBase*, numerator_type, T, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("numerator") = numerator_type::rho_variant,
                py::arg("gamma") = static_cast<T>(1),
                py::arg("RHO") = static_cast<T>(0));

        using Richardson = richardson_algorithm<T, K, SeriesBase*>;
        py::class_<Richardson, SeriesAcceleration>(m, "RichardsonAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using Weniger = weniger_algorithm<T, K, SeriesBase*>;
        py::class_<Weniger, SeriesAcceleration>(m, "WenigerAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WynnEpsilon = wynn_epsilon_1_algorithm<T, K, SeriesBase*>;
        py::class_<WynnEpsilon, SeriesAcceleration>(m, "WynnEpsilonAlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WynnEpsilon2 = wynn_epsilon_2_algorithm<T, K, SeriesBase*>;
        py::class_<WynnEpsilon2, SeriesAcceleration>(m, "WynnEpsilon2AlgorithmArb")
            .def(py::init<SeriesBase*>(), py::arg("series"), py::keep_alive<1, 2>());

        using WynnEpsilon3 = wynn_epsilon_3_algorithm<T, K, SeriesBase*>;
        py::class_<WynnEpsilon3, SeriesAcceleration>(m, "WynnEpsilon3AlgorithmArb")
            .def(py::init<SeriesBase*, T>(),
                py::arg("series"), py::keep_alive<1, 2>(),
                py::arg("epsilon_threshold") = float_precision("1e-3"));

        using ArraySeries = array_series<T, K>;
        py::class_<ArraySeries, SeriesBase>(m, "ArraySeriesArb")
            .def(py::init<py::buffer>(), py::arg("buffer"), "Construct from a Python buffer (numpy array/memoryview). Will copy/convert elements to arbitrary precision.")
            .def(py::init<py::object>(), py::arg("sequence"), "Construct from a Python sequence (will copy).")
            .def("size", &ArraySeries::size);
    }

    // Done
}
