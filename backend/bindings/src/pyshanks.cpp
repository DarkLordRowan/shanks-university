/**
 * @file pyshanks.cpp
 * @brief pybind11 bindings entry point
 * @authors Naumov A.U., Sobolev Y. A.
 */

#include "../../core/include/noise/noise_generator.hpp"
#include "../../core/include/remainders.hpp"
#include "../../core/include/wynn_numerators.hpp"
#include "../include/bindings.hpp"

PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series (double + float_precision), registry-based";

    // 0. Bind profiling utils if enabled
    py::class_<shanks::profiling::OperationCounts>(m, "OperationCounts")
        .def_readonly("add", &shanks::profiling::OperationCounts::add)
        .def_readonly("mul", &shanks::profiling::OperationCounts::mul)
        .def_readonly("div", &shanks::profiling::OperationCounts::div)
        .def_readonly("special", &shanks::profiling::OperationCounts::special)
        .def("__repr__", [](const shanks::profiling::OperationCounts& oc) {
            std::ostringstream oss;
            oss << "OperationCounts(add=" << oc.add << ", mul=" << oc.mul << ", div=" << oc.div
                << ", special=" << oc.special << ")";
            return oss.str();
        });

    m.def("reset_operation_counts", &shanks::profiling::reset_counts);
    m.def("get_operation_counts", &shanks::profiling::get_counts);

    // 1. Bind basic types
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

    // 3. Bind all templated components from separate compilation units
    bind_series_real(m);
    bind_series_complex(m);
    bind_algos_real(m);
    bind_algos_complex(m);
    bind_noise_all(m);
    bind_filters_all(m);
}
