#ifndef BIND_SERIES_HPP
#define BIND_SERIES_HPP
#pragma once

#include "bindings.hpp"

#define OVERALL_SERIES_DEFINED 100
extern constinit const char* series_names[OVERALL_SERIES_DEFINED];

namespace py = pybind11;

template<typename TupleOfSeries, typename BaseClass, std::size_t I = 0>
constexpr void bind_series_by_types(py::module_& m, const std::string& suffix){
    if constexpr (I < std::tuple_size_v<TupleOfSeries>){
        using CurrentSeries = std::tuple_element_t<I, TupleOfSeries>;
        py::class_<CurrentSeries, BaseClass> binding(m, (std::string{series_names[I]} + suffix).c_str());
        bind_series_by_types<TupleOfSeries, BaseClass, I + 1>(m, suffix);
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

#endif