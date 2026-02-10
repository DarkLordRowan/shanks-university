#include "../include/bindings_templates.hpp"

void bind_noise_all(py::module_& m) {
    py::enum_<NoiseType>(m, "NoiseType")
        .value("Uniform", NoiseType::uniform)
        .value("Normal", NoiseType::normal)
        .value("Poisson", NoiseType::poisson)
        .export_values();

    py::enum_<NoiseMethod>(m, "NoiseMethod")
        .value("Jitter", NoiseMethod::jitter)
        .value("Scaling", NoiseMethod::scaling)
        .export_values();

    bind_each_type<types_to_bind>(
        m, suffixes, []<typename T, typename K>(py::module_& mod, const char* s) { bind_noise<T>(mod, s); });
}
