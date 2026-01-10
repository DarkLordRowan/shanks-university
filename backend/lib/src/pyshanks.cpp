/**
 * @file pyshanks.cpp
 * @brief pybind11 bindings with support for double and arbitrary-precision float_precision
 * @authors Sobolev Y. A., Naumov A.U.
*/

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <complex>

#include "mpreal.h"
#include "../include/bindings.hpp"

PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series (double + float_precision), registry-based";

    // 1. Bind basic types first!
    bind_types(m);

    // 2. Bind enums
    py::enum_<shanks::remainders::remainder_type>(m, "RemainderType")
        .value("u_type", shanks::remainders::remainder_type::u_type)
        .value("t_type", shanks::remainders::remainder_type::t_type)
        .value("v_type", shanks::remainders::remainder_type::v_type)
        .value("t_wave_type", shanks::remainders::remainder_type::t_wave_type)
        .value("v_wave_type", shanks::remainders::remainder_type::v_wave_type)
        .export_values();

    py::enum_<shanks::numerators::numerator_type>(m, "NumeratorType")
        .value("rho_type", shanks::numerators::numerator_type::rho_type)
        .value("generalized_type", shanks::numerators::numerator_type::generalized_type)
        .value("gamma_rho_type", shanks::numerators::numerator_type::gamma_rho_type)
        .export_values();

    py::enum_<NoiseType>(m, "NoiseType")
        .value("Uniform", NoiseType::uniform)
        .value("Normal", NoiseType::normal)
        .value("Poisson", NoiseType::poisson)
        .export_values();

    py::enum_<NoiseMethod>(m, "NoiseMethod")
        .value("Jitter", NoiseMethod::jitter)
        .value("Scaling", NoiseMethod::scaling)
        .export_values();

    // 3. Bind all templated series and algos
    using types_to_bind = std::tuple<
        std::tuple<       float, size_t>,
        std::tuple<      double, size_t>,
        std::tuple< long double, size_t>,
        std::tuple<std::complex<float       >, size_t>,
        std::tuple<std::complex<double      >, size_t>,
        std::tuple<std::complex<long double >, size_t>
    >;

    bind_all_types<types_to_bind>(m);

    bind_all<mpfr::mpreal, size_t>(m, "Arb");
    bind_all<std::complex<mpfr::mpreal>, size_t>(m, "CArb");
}