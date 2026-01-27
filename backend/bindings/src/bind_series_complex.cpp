#include "../include/bindings_templates.hpp"

void bind_series_complex(py::module_& m) {
    bind_each_type<types_complex>(m, suffixes_complex, []<typename T, typename K>(py::module_& mod, const char* s) {
        bind_series<T, K>(mod, s);
    });
}
