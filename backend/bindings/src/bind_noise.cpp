#include "../include/bindings_templates.hpp"

void bind_noise_all(py::module_& m) {
    bind_each_type<types_to_bind>(
        m, suffixes, []<typename T, typename K>(py::module_& mod, const char* s) { bind_noise<T>(mod, s); });
}
