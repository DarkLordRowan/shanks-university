#include "../include/bindings.hpp"
#include "../../core/include/filters/kolzur.hpp"
#include "../../core/include/filters/savgol.hpp"

template<AcceptedLike T>
void bind_filters(pybind11::module_& m, const char* suffix){
    m.def(create_name("kolzurFilter", suffix).c_str(),
        &shanks::filters::kolzur_filter<T>,
        py::arg("result"), py::arg("windowLength"), py::arg("degree")
    );

    m.def(create_name("savgolFilter", suffix).c_str(),
        &shanks::filters::savgol_filter<T>,
        py::arg("result"), py::arg("windowLength"),
        py::arg("polyorder"), py::arg("derive"), py::arg("delta")
    );
}

void bind_filters_all(py::module_& m) {
    bind_each_type<types_to_bind>(m, suffixes, []<typename T, typename K>(py::module_& mod, const char* s) {
        bind_filters<T>(mod, s);
    });
}
