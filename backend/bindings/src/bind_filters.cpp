#include "../include/bindings_templates.hpp"

void bind_filters_all(py::module_& m) {
    bind_each_type<types_to_bind>(
        m, suffixes, []<typename T, typename K>(py::module_& mod, const char* s) { bind_filters<T>(mod, s); });
}
