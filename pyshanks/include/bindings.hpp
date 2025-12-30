#ifndef BINDINGS_HPP
#define BINDINGS_HPP
#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../shanks/include/custom_concepts.hpp"
#include "../../shanks/include/utils.hpp"

#include "../../shanks/include/methods.hpp"
#include "../../shanks/include/series.hpp"

namespace py = pybind11;

template <AcceptedLike T, UnsignedIntLike K> 
constexpr void bind_series(py::module_& m, const std::string& suffix);

template <AcceptedLike T, UnsignedIntLike K> 
constexpr void bind_algos(py::module_& m, const std::string& suffix);

template <FloatLike T>
constexpr void bind_complex_num(py::module_& m, const char* pyname);

template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_all(py::module_& m, const std::string& suffix);

#define OVERALL_SERIES_DEFINED 100
extern constinit const char* series_names[OVERALL_SERIES_DEFINED];

#define OVERALL_ARB_TYPES 8
extern constinit const char* available_types[OVERALL_ARB_TYPES];

template<typename TupleOfTypes, std::size_t TypesIndex = 0, std::size_t NameIndex = 0>
constexpr void bind_comlex_types(py::module_& m){
    if constexpr (TypesIndex < std::tuple_size_v<TupleOfTypes> && NameIndex < OVERALL_ARB_TYPES){
        using type_from_tuple = std::tuple_element_t<TypesIndex, TupleOfTypes>;
        bind_complex_num<type_from_tuple>(m, available_types[NameIndex]);
        bind_comlex_types<TupleOfTypes, TypesIndex + 1, NameIndex + 1>(m);
    } else return;
}

template<typename TupleOfSeries, typename BaseClass, std::size_t I = 0>
constexpr void bind_series_by_types(py::module_& m, const std::string& suffix){
    if constexpr (I < std::tuple_size_v<TupleOfSeries>){
        using CurrentSeries = std::tuple_element_t<I, TupleOfSeries>;
        py::class_<CurrentSeries, BaseClass> binding(m, (std::string{series_names[I]} + suffix).c_str());
        bind_series_by_types<TupleOfSeries, BaseClass, I + 1>(m, suffix);
    } else return;
}

template<typename TupleOfTypes, std::size_t TypesIndex = 0, std::size_t NameIndex = 0>
constexpr void bind_all_types(py::module_& m){
    if constexpr (TypesIndex < std::tuple_size_v<TupleOfTypes> && NameIndex < OVERALL_ARB_TYPES){
        using tuple_from_tuple = std::tuple_element_t<TypesIndex, TupleOfTypes>;
        using TypeOfT = std::tuple_element_t<0, tuple_from_tuple>;
        using TypeOfK = std::tuple_element_t<1, tuple_from_tuple>;
        bind_all<TypeOfT, TypeOfK>(m, available_types[NameIndex]);
        bind_all_types<TupleOfTypes, TypesIndex + 1, NameIndex + 1>(m);
    } else return;
}

template <AcceptedLike T, UnsignedIntLike K> 
constexpr void bind_series(py::module_& m, const std::string& suffix){

    using MSeriesBase = shanks::series::series_base<T, K>;

    using SeriesTuple = std::tuple<
        shanks::series::abs_sin_x_minus_2_div_pi_series<T,K>,
        shanks::series::arcsin_x2_series<T,K>,
        shanks::series::arcsin_x_minus_x_series<T,K>,
        shanks::series::arcsin_x_series<T,K>,
        shanks::series::arcsinh_x_series<T,K>,
        shanks::series::arctanh_x2_series<T,K>,
        shanks::series::arctanh_x_series<T,K>,
        shanks::series::arctg_x2_series<T,K>,
        shanks::series::arctg_x3_series<T,K>,
        shanks::series::arctg_x_series<T,K>,
        shanks::series::bin_series<T,K>,
        shanks::series::ci_x_series<T,K>,
        shanks::series::cos3xmin1_div_xsquare_series<T,K>,
        shanks::series::cos_series<T,K>,
        shanks::series::cos_sqrt_x_series<T,K>,
        shanks::series::cos_x2_series<T,K>,
        shanks::series::cosh_series<T,K>,
        shanks::series::e_x_series<T,K>,
        shanks::series::eighth_pi_m_one_third_series<T,K>,
        shanks::series::erf_series<T,K>,
        shanks::series::exp_m_cos_x_sinsin_x_series<T,K>,
        shanks::series::exp_series<T,K>,
        shanks::series::exp_squared_erf_series<T,K>,
        shanks::series::five_pi_twelve_series<T,K>,
        shanks::series::four_arctan_series<T,K>,
        shanks::series::four_ln2_m_3_series<T,K>,
        shanks::series::half_asin_two_x_series<T,K>,
        shanks::series::half_minus_sinx_multi_pi_4_series<T,K>,
        shanks::series::half_multi_ln_1div2multi1minuscosx_series<T,K>,
        shanks::series::incomplete_Gamma_func_series<T,K>,
        shanks::series::inverse_1mx_series<T,K>,
        shanks::series::inverse_sqrt_1m4x_series<T,K>,
        shanks::series::k_x_series<T,K>,
        shanks::series::lambert_W_func_series<T,K>,
        shanks::series::ln1_m_x2_series<T,K>,
        shanks::series::ln1mx_series<T,K>,
        shanks::series::ln1px4_series<T,K>,
        shanks::series::ln2_series<T,K>,
        shanks::series::ln13_min_ln7_div_7_series<T,K>,
        shanks::series::ln_1_plus_x3_series<T,K>,
        shanks::series::ln_1plussqrt1plusxsquare_minus_ln_2_series<T,K>,
        shanks::series::ln_1plusx_div_1plusx2_series<T,K>,
        shanks::series::ln_cosx_series<T,K>,
        shanks::series::ln_sinx_minus_ln_x_series<T,K>,
        shanks::series::ln_x_plus_one_x_minus_one_halfed_series<T,K>,
        shanks::series::m_fact_1mx_mp1_inverse_series<T,K>,
        shanks::series::mean_sinh_sin_series<T,K>,
        shanks::series::minus_3_div_4_or_x_minus_3_div_4_series<T,K>,
        shanks::series::minus_one_n_fact_n_in_n_series<T,K>,
        shanks::series::minus_one_ned_in_n_series<T,K>,
        shanks::series::minus_one_quarter_series<T,K>,
        shanks::series::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T,K>,
        shanks::series::minus_x_minus_pi_4_or_minus_pi_4_series<T,K>,
        shanks::series::one_div_sqrt2_sin_xdivsqrt2_series<T,K>,
        shanks::series::one_div_two_minus_x_multi_three_plus_x_series<T,K>,
        shanks::series::one_minus_sqrt_1minus4x_div_2x_series<T,K>,
        shanks::series::one_series<T,K>,
        shanks::series::one_third_pi_squared_m_nine_series<T,K>,
        shanks::series::one_twelfth_3x2_pi2_series<T,K>,
        shanks::series::one_twelfth_series<T,K>,
        shanks::series::one_twelfth_x2_pi2_series<T,K>,
        shanks::series::one_twelfth_x2_pi2_series<T,K>,
        shanks::series::pi_3_series<T,K>,
        shanks::series::pi_4_series<T,K>,
        shanks::series::pi_8_cosx_square_minus_1_div_3_cosx_series<T,K>,
        shanks::series::pi_cubed_32_series<T,K>,
        shanks::series::pi_four_minus_ln2_halfed_series<T,K>,
        shanks::series::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T,K>,
        shanks::series::pi_minus_x_2_series<T,K>,
        shanks::series::pi_series<T,K>,
        shanks::series::pi_six_min_half_series<T,K>,
        shanks::series::pi_squared_6_minus_one_series<T,K>,
        shanks::series::pi_squared_twelve_series<T,K>,
        shanks::series::pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<T,K>,
        shanks::series::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_series<T,K>,
        shanks::series::riemann_zeta_func_series<T,K>,
        shanks::series::riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T,K>,
        shanks::series::series_with_ln_number1_series<T,K>,
        shanks::series::series_with_ln_number2_series<T,K>,
        shanks::series::si_x_series<T,K>,
        shanks::series::sin_series<T,K>,
        shanks::series::sin_x2_series<T,K>,
        shanks::series::sinh_series<T,K>,
        shanks::series::sinh_x2_series<T,K>,
        shanks::series::sqrt_1plusx_min_1_min_x_div_2_series<T,K>,
        shanks::series::sqrt_1plusx_series<T,K>,
        shanks::series::sqrt_oneminussqrtoneminusx_div_x_series<T,K>,
        shanks::series::ten_minus_x_series<T,K>,
        shanks::series::three_minus_pi_series<T,K>,
        shanks::series::two_arcsin_square_x_halfed_series<T,K>,
        shanks::series::two_degree_x_series<T,K>,
        shanks::series::two_ln2_series<T,K>,
        shanks::series::x_1mx_squared_series<T,K>,
        shanks::series::x_div_1minx2_series<T,K>,
        shanks::series::x_div_1minx_series<T,K>,
        shanks::series::x_min_sqrt_x_series<T,K>,
        shanks::series::x_series<T,K>,
        shanks::series::x_two_series<T,K>,
        shanks::series::x_two_throught_squares_series<T,K>,
        shanks::series::xsquareplus3_div_xsquareplus2multix_minus_1_series<T,K>
    >;

    py::class_<series_result<T>>(m, (std::string("SeriesResult") + suffix).c_str())
        .def(py::init<>())
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readwrite("Sn", &series_result<T>::Sn)
        .def_readwrite("an", &series_result<T>::an);

    // SeriesBase
    py::class_<MSeriesBase>(m, (std::string("SeriesBase") + suffix).c_str())
        .def("get_x", &MSeriesBase::get_x)
        .def("get_sum", &MSeriesBase::get_sum)
        .def("get_name", &MSeriesBase::get_name)
        .def("generateSeries",
             &MSeriesBase::generate_series,
             py::arg("x"),
             py::arg("vecSize"),
             py::arg("addTParameter") = utils::cast<T>(1.0),
             py::arg("addKParameter") = utils::cast<K>(1.0));
    
    bind_series_by_types<SeriesTuple, MSeriesBase>(m, suffix);
    
};

template <AcceptedLike T, UnsignedIntLike K> 
constexpr void bind_algos(py::module_& m, const std::string& suffix){

    using RealT = GetUnderlyingType<T>::value;
    using MSeriesAcceleration = shanks::algos::series_acceleration<T, K>;

    // SeriesAcceleration
    py::class_<MSeriesAcceleration>(m, (std::string{"SeriesAcceleration"} + suffix).c_str())
        .def("printInfo", &MSeriesAcceleration::get_name)
        .def("__call__", &MSeriesAcceleration::operator(),
             py::arg("n"), py::arg("order"), py::arg("data"));

    py::class_<shanks::algos::brezinski_theta_algorithm<T,K>, MSeriesAcceleration>
        (m,(std::string{"BrezinskiThetaAlgorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::chang_wynn_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"ChangWynnAlgorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::drummond_d_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"DrummondDAlgorithm"} + suffix).c_str())
        .def(py::init<shanks::remainders::remainder_type, bool>(),
             py::arg("remainder") = shanks::remainders::remainder_type::t_type,
             py::arg("useRecurrentFormula") = false);

    py::class_<shanks::algos::ford_sidi_2_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"FordSidi2Algorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::ford_sidi_3_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"FordSidi3Algorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::levin_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"LevinAlgorithm"} + suffix).c_str())
        .def(py::init<shanks::remainders::remainder_type, bool, RealT>(),
            py::arg("remainder") = shanks::remainders::remainder_type::t_type,
            py::arg("useRecurrentFormula") = false,
            py::arg("beta") = static_cast<RealT>(1));

    py::class_<shanks::algos::levin_sidi_m_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"LevinSidiMAlgorithm"} + suffix).c_str())
        .def(py::init<shanks::remainders::remainder_type, RealT>(),
            py::arg("remainder") = shanks::remainders::remainder_type::t_type,
            py::arg("gamma") = static_cast<RealT>(10));

    py::class_<shanks::algos::levin_sidi_s_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"LevinSidiSAlgorithm"} + suffix).c_str())
        .def(py::init<shanks::remainders::remainder_type, bool, RealT>(),
            py::arg("remainder") = shanks::remainders::remainder_type::t_type,
            py::arg("useRecurrentFormula") = false,
            py::arg("beta") = static_cast<RealT>(1));

    py::class_<shanks::algos::lubkin_w_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"LubkinWAlgorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::richardson_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"RichardsonAlgorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::shanks_transform_alternating<T,K>, MSeriesAcceleration>
        (m, (std::string{"ShanksTransformAlternating"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::shanks_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"ShanksAlgorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::weniger_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"WenigerAlgorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::wynn_epsilon_1_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"WynnEpsilon1Algorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::wynn_epsilon_2_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"WynnEpsilon2Algorithm"} + suffix).c_str())
        .def(py::init<>());

    py::class_<shanks::algos::wynn_epsilon_3_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"WynnEpsilon3Algorithm"} + suffix).c_str())
        .def(py::init<RealT>(), py::arg("epsilon_threshold") = static_cast<RealT>(1e-3));

    py::class_<shanks::algos::wynn_rho_algorithm<T,K>, MSeriesAcceleration>
        (m, (std::string{"WynnRhoAlgorithm"} + suffix).c_str())
        .def(py::init<shanks::numerators::numerator_type, RealT, RealT>(),
            py::arg("numerator") = shanks::numerators::numerator_type::rho_type,
            py::arg("gamma") = static_cast<RealT>(-1),
            py::arg("rho") = static_cast<RealT>(1));

};

template<typename T>
requires is_precisable<T>::value
void bind_arb_real_num(py::module_& m, const char* name){
    py::class_<T>(m, name)
        .def(
        py::init([](const double& d, const size_t precision){
            T value = utils::cast<T>(d); utils::set_precision(precision, value);
            return value;
        }), py::arg("d"), py::arg("precision"))
        .def(
        py::init([](const std::string& s, const size_t precision){
            T value = utils::cast<T>(s); utils::set_precision(precision, value);
            return value;
        }), py::arg("s"), py::arg("precision"))
        .def("__str__", [](const T &x){ return utils::to_string(x) ; })
        .def("__format__", [](const T &x, const std::string&){ return utils::to_string(x); })
        .def("__repr__", [&name](const T &x){ return std::string("<") + name + ": " + utils::to_string(x) + ">"; })
        .def(py::self + py::self) .def(py::self - py::self)
        .def(py::self * py::self) .def(py::self / py::self)
        .def(-py::self) .def(+py::self)
        .def("__abs__", [](const T& a){ return utils::abs(a); })
        .def(py::self == py::self) .def(py::self != py::self)
        .def(py::self <  py::self) .def(py::self <= py::self)
        .def(py::self >  py::self) .def(py::self >= py::self)
        .def("__float__", [](const T &x){ return utils::cast<double>(x); })
        .def("__int__",   [](const T &x){ return static_cast<long>(utils::cast<double>(x)); })
        .def("__index__", [](const T &x){ return static_cast<long>(utils::cast<double>(x)); })
        .def("__getstate__", [](const T& x){ return utils::to_string(x); })
        .def("__setstate__", [](T& self, const std::string& s){ new (&self) T(s); });

}

template <FloatLike T>
constexpr void bind_complex_num(py::module_& m, const char* pyname) {

    using C = std::complex<T>;

    using State = std::conditional_t<std::is_same_v<T, mpfr::mpreal>, std::pair<std::string, std::string>, std::pair<T, T>>;

    auto cls = py::class_<C>(m, pyname)
        .def(py::init<>())
        .def(py::init<T>(), py::arg("re"))
        .def(py::init<T, T>(), py::arg("re"), py::arg("im"))
        .def(py::init([](std::complex<double> z){
            return C(utils::cast<T>(z.real()), utils::cast<T>(z.imag()));
        }), py::arg("z"))
        .def_property("real",
            [](const C& z){ return z.real(); },
            [](C& z, const T& r){ z.real(r); })
        .def_property("imag",
            [](const C& z){ return z.imag(); },
            [](C& z, const T& i){ z.imag(i); })
        .def("__str__", [](const C& z){ return utils::to_string(z); })
        .def("__format__", [](const C& z, const std::string&){ return utils::to_string(z); })
        .def("__repr__", [](const C& z){return utils::to_string(z);})
        .def("__complex__", [](const C& z){ return std::complex<double>(utils::cast<double>(z.real()), utils::cast<double>(z.imag())); })
        .def(-py::self)
        .def(+py::self)
        .def("__abs__", [](const C& a){ return utils::abs(a); })
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__getstate__", [](const C& z)->State{
            if constexpr (is_precisable<T>::value) return {utils::to_string(z.real()), utils::to_string(z.imag())};
            else return { z.real(), z.imag() };
        })
        .def("__setstate__", [](C& z, const State& s){
            if constexpr (is_precisable<T>::value) new (&z) C(utils::cast<T>(s.first), utils::cast<T>(s.second));
            else new (&z) C(s.first, s.second);
        });
}

template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_all(py::module_& m, const std::string& suffix){
    bind_series<T,K>(m, suffix);
    bind_algos<T,K>(m, suffix);
}

#endif