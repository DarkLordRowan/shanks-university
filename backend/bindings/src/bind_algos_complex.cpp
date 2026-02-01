#include "../include/bindings_templates.hpp"
#include "../../core/include/methods.hpp"

void bind_algos_complex(py::module_& m) {
    bind_each_type<types_complex>(
        m, suffixes_complex, []<typename T, typename K>(py::module_& mod, const char* s) { bind_algos<T, K>(mod, s); });
}
