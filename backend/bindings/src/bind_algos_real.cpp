#include "../include/bindings_templates.hpp"

void bind_algos_real(py::module_& m) {
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

    bind_each_type<types_real>(
        m, suffixes_real, []<typename T, typename K>(py::module_& mod, const char* s) { bind_algos<T, K>(mod, s); });
}
