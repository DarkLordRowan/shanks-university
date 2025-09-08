// py.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "series.h"
#include "methods/shanks_transformation.hpp"   // we instantiate the template with a smart pointer

namespace py = pybind11;

PYBIND11_MODULE(pyshanks, m) {
    using T = double;
    using K = size_t;
    using ExpSeries = exp_series<T, K>;
    using ExpSeriesPtr = std::shared_ptr<ExpSeries>;
    using Shanks = shanks_transform<T, K, ExpSeriesPtr>;
    using ShanksAlt = shanks_transform_alternating<T, K, ExpSeriesPtr>;

    // Bind ExpSeries with std::shared_ptr holder (no default ctor)
    py::class_<ExpSeries, ExpSeriesPtr>(m, "ExpSeries")
        .def(py::init<T>(), py::arg("x"))              // <- update if real ctor differs
        .def("__call__", &ExpSeries::operator())
        .def("S_n", &ExpSeries::S_n)
        .def("get_sum", &ExpSeries::get_sum);

    // Bind shanks_transform instantiated with shared_ptr<ExpSeries>
    py::class_<Shanks>(m, "ShanksTransform")
        .def(py::init<const ExpSeriesPtr&>(), py::arg("series"))
        .def("__call__", &Shanks::operator());

    // If you need the alternating specialization too:
    py::class_<ShanksAlt>(m, "ShanksTransformAlternating")
        .def(py::init<const ExpSeriesPtr&>(), py::arg("series"))
        .def("__call__", &ShanksAlt::operator());
}
