#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "series.h"
#include "methods/shanks_transformation.hpp"

namespace py = pybind11;
using T = double;
using K = size_t;

using SeriesBase = series_base<T, K>;
using SeriesBasePtr = std::shared_ptr<SeriesBase>;

// instantiate shanks with pointer-to-base so original code (this->series->...) works
using Shanks = shanks_transform<T, K, SeriesBasePtr>;
using ShanksAlt = shanks_transform_alternating<T, K, SeriesBasePtr>;

// --- create local aliases to avoid commas inside macro args ---
using ExpSeriesT        = exp_series<T, K>;
using CosSeriesT        = cos_series<T, K>;
using SinSeriesT        = sin_series<T, K>;
using CoshSeriesT       = cosh_series<T, K>;
using SinhSeriesT       = sinh_series<T, K>;
using BinSeriesT        = bin_series<T, K>;
using FourArctanSeriesT = four_arctan_series<T, K>;
using Ln1mxSeriesT      = ln1mx_series<T, K>;
using MeanSinhSinSeriesT= mean_sinh_sin_series<T, K>;
using ExpSqErfSeriesT   = exp_squared_erf_series<T, K>;
using XmbJbTwoSeriesT   = xmb_Jb_two_series<T, K>;
using HalfAsinTwoXSeriesT = half_asin_two_x_series<T, K>;
using Inverse1mxSeriesT = inverse_1mx_series<T, K>;
using X1mxSqSeriesT     = x_1mx_squared_series<T, K>;
using ErfSeriesT        = erf_series<T, K>;
// add more aliases as you bind more types...

// variadic macro so ctor signature may contain commas
#define BIND_SERIES(DERIVED, PYNAME, ...)                                 \
    py::class_<DERIVED, SeriesBase, std::shared_ptr<DERIVED>>(m, PYNAME)  \
        .def(py::init<__VA_ARGS__>())                                     \
        .def("__call__", &DERIVED::operator())                            \
        .def("S_n", &DERIVED::S_n)                                        \
        .def("get_sum", &DERIVED::get_sum);

// Module
PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series + ShanksTransform (explicit BIND_SERIES)";

    // bind base (use shared_ptr<SeriesBase>)
    py::class_<SeriesBase, SeriesBasePtr>(m, "SeriesBase")
        .def("S_n", &SeriesBase::S_n)
        .def("__call__", &SeriesBase::operator())
        .def("get_sum", &SeriesBase::get_sum)
        .def("get_x", &SeriesBase::get_x);

    // --- Bind concrete series ---
    BIND_SERIES(ExpSeriesT,           "ExpSeries",                T)
    BIND_SERIES(CosSeriesT,           "CosSeries",                T)
    BIND_SERIES(SinSeriesT,           "SinSeries",                T)
    BIND_SERIES(CoshSeriesT,          "CoshSeries",               T)
    BIND_SERIES(SinhSeriesT,          "SinhSeries",               T)
    BIND_SERIES(BinSeriesT,           "BinSeries",                T, T)    // (T x, T alpha)
    BIND_SERIES(FourArctanSeriesT,    "FourArctanSeries",        T)
    BIND_SERIES(Ln1mxSeriesT,         "Ln1mxSeries",             T)
    BIND_SERIES(MeanSinhSinSeriesT,   "MeanSinhSinSeries",       T)
    BIND_SERIES(ExpSqErfSeriesT,      "ExpSquaredErfSeries",     T)
    BIND_SERIES(XmbJbTwoSeriesT,      "XmbJbTwoSeries",          T, K)   // (T x, K b)
    BIND_SERIES(HalfAsinTwoXSeriesT,  "HalfAsinTwoXSeries",      T)
    BIND_SERIES(Inverse1mxSeriesT,    "Inverse1mxSeries",        T)
    BIND_SERIES(X1mxSqSeriesT,        "X_1mx_SquaredSeries",     T)
    BIND_SERIES(ErfSeriesT,           "ErfSeries",               T)
    // add more BIND_SERIES(...) here for other types with correct ctor arg lists

    // Bind Shanks (accepts any SeriesBasePtr)
    py::class_<Shanks>(m, "ShanksTransform")
        .def(py::init<const SeriesBasePtr&>(), py::arg("series"))
        .def("__call__", &Shanks::operator());

    py::class_<ShanksAlt>(m, "ShanksTransformAlternating")
        .def(py::init<const SeriesBasePtr&>(), py::arg("series"))
        .def("__call__", &ShanksAlt::operator());
}

#undef BIND_SERIES
