#include "../include/bindings_templates.hpp"

void bind_series_real(py::module_& m) {
    bind_each_type<types_real>(m, suffixes_real, []<typename T, typename K>(py::module_& mod, const char* s) {
        bind_series<T, K>(mod, s);
    });
}
