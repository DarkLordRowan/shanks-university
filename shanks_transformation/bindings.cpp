#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "series/series_base.h"
#include "series/serieses/exp_series.h"
#include "series_acceleration/series_acceleration.hpp"
#include "series_acceleration/algorithm/ford_sidi_two_algorithm.hpp"

namespace py = pybind11;

PYBIND11_MODULE(series_module, m) {
    m.doc() = "Python bindings for C++ series computations";

    // Base class binding for long double, long long
    py::class_<series_base<long double, long long>>(m, "SeriesBase")
        .def("S_n", &series_base<long double, long long>::S_n, py::arg("n"))
        .def("term", &series_base<long double, long long>::operator(), py::arg("n"));

    // Exponential series
    py::class_<exp_series<long double, long long>, series_base<long double, long long>>(m, "ExpSeries")
        .def(py::init<long double>(), py::arg("x") = 0.0);

    // Acceleration base class
    py::class_<series_acceleration<long double, long long, series_base<long double, long long> *>>(m, "SeriesAcceleration")
        .def("print_info", &series_acceleration<long double, long long, series_base<long double, long long> *>::print_info)
        .def("accelerate", &series_acceleration<long double, long long, series_base<long double, long long> *>::operator(), py::arg("n"), py::arg("order"));

    // Ford-Sidi second-order acceleration
    py::class_<ford_sidi_two_algorithm<long double, long long, series_base<long double, long long> *>,
               series_acceleration<long double, long long, series_base<long double, long long> *>>(m, "FordSidiTwo")
        .def(py::init<series_base<long double, long long> *>(), py::arg("series"));
}
