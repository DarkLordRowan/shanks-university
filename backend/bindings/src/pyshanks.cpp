/**
 * @file pyshanks.cpp
 * @brief pybind11 bindings entry point
 * @authors Naumov A.U., Sobolev Y. A.
 */

#include "../include/bindings.hpp"

// --- Binding Function Declarations ---

extern void bind_types(py::module_& m);
extern void bind_series_real(py::module_& m);
extern void bind_series_complex(py::module_& m);
extern void bind_algos_real(py::module_& m);
extern void bind_algos_complex(py::module_& m);
extern void bind_noise_all(py::module_& m);
extern void bind_filters_all(py::module_& m);

PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series (double + float_precision), registry-based";

    // 1. Bind basic types
    bind_types(m);

    // 3. Bind all templated components from separate compilation units
    bind_series_real(m);
    bind_series_complex(m);
    bind_algos_real(m);
    bind_algos_complex(m);
    bind_noise_all(m);
    bind_filters_all(m);
}
