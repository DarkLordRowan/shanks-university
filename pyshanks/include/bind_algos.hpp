#ifndef BIND_ALGOS_HPP
#define BIND_ALGOS_HPP
#pragma once

#include "bindings.hpp"

namespace py = pybind11;

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

#endif