#include "../include/bindings_templates.hpp"

void bind_noise_all(py::module_& m) {
    py::enum_<shanks::NoiseType>(m, "NoiseType")
        .value("Uniform", shanks::NoiseType::uniform)
        .value("Normal",  shanks::NoiseType::normal)
        .value("Poisson", shanks::NoiseType::poisson)
        .export_values();

    py::enum_<shanks::NoiseMethod>(m, "NoiseMethod")
        .value("Jitter",  shanks::NoiseMethod::jitter)
        .value("Scaling", shanks::NoiseMethod::scaling)
        .export_values();

    bind_each_type<types_to_bind>(
        m, suffixes, []<typename T, typename K>(py::module_& mod, const char* s) { bind_noise<T>(mod, s); });
}
