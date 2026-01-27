#include "../../core/include/filters/kolmogorov_zurbenko.hpp"
#include "../../core/include/filters/savitzky_golay.hpp"
#include "../include/bindings.hpp"

template <AcceptedLike T>
void bind_filters(pybind11::module_& m, const char* suffix)
{
    m.def(create_name("kolmogorovZurbenkoFilter", suffix).c_str(),
          &shanks::filters::kolmogorov_zurbenko_filter<T>,
          py::arg("result"),
          py::arg("windowLength"),
          py::arg("degree"));

    m.def(create_name("savitzkyGolayFilter", suffix).c_str(),
          &shanks::filters::savitzky_golay_filter<T>,
          py::arg("result"),
          py::arg("windowLength"),
          py::arg("polyorder"),
          py::arg("derive"),
          py::arg("delta"));
}

void bind_filters_all(py::module_& m)
{
    bind_each_type<types_to_bind>(
        m, suffixes, []<typename T, typename K>(py::module_& mod, const char* s) { bind_filters<T>(mod, s); });
}
