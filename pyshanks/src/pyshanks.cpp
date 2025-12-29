
/**
 * @file pyshanks.cpp
 * @brief pybind11 bindings with support for double and arbitrary-precision float_precision
*/

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

#include "../libs/mpfr/mpreal.h"
#include "../include/bindings.hpp"

namespace py = pybind11;

inline py::size_t hash_float_precision(const mpfr::mpreal& x) {
    py::size_t h = 0;
    h ^= std::hash<size_t>{}(utils::get_precision(x)) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<int>{}(mpfr::sgn(x)) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<mp_exp_t>{}(x.get_exp()) + 0x9e3779b9 + (h << 6) + (h >> 2);
    //auto mantissa = x.number();
    //for (size_t i = 0; i < std::min(mantissa.size(), size_t(3)); ++i) {
    //    h ^= std::hash<wctype>{}(mantissa[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
    //}
    return h;
}

PYBIND11_MODULE(pyshanks, m) {
    m.doc() = "pybind11: polymorphic series (double + float_precision), helper-organized, no-arg constructors, backward-compatible";

    py::enum_<shanks::remainders::remainder_type>(m, "RemainderType")
        .value("u_type", shanks::remainders::remainder_type::u_type)
        .value("t_type", shanks::remainders::remainder_type::t_type)
        .value("v_type", shanks::remainders::remainder_type::v_type)
        .value("t_wave_type", shanks::remainders::remainder_type::t_wave_type)
        .value("v_wave_type", shanks::remainders::remainder_type::v_wave_type)
        .export_values();

    py::enum_<shanks::numerators::numerator_type>(m, "NumeratorType")
        .value("rho_type", shanks::numerators::numerator_type::rho_type)
        .value("generalized_type", shanks::numerators::numerator_type::generalized_type)
        .value("gamma_rho_type", shanks::numerators::numerator_type::gamma_rho_type)
        .export_values();

    // Arb
    {
        using R = mpfr::mpreal;

        py::class_<R>(m, "Arb")
            .def(
            py::init([](const double& d, const size_t precision){
                    return mpfr::mpreal(d, mpfr::digits2bits(precision));
            }), py::arg("d"), py::arg("precision"))
            .def(
            py::init([](const std::string& s, const size_t precision){
                    return mpfr::mpreal(s, mpfr::digits2bits(precision));
            }), py::arg("s"), py::arg("precision"))
            .def("__str__", [](const R &x){ return utils::to_string(x) ; })
            .def("__format__", [](const R &x, const std::string&){ return utils::to_string(x); })
            .def("__repr__", [](const R &x){ return std::string("<") + "Arb" + ": " + utils::to_string(x) + ">"; })
            .def(py::self + py::self) .def(py::self - py::self)
            .def(py::self * py::self) .def(py::self / py::self)
            .def(-py::self) .def(+py::self)
            .def("__abs__", [](const R& a){ return a < utils::cast<R>(0.0) ? -a : a; })
            .def(py::self == py::self) .def(py::self != py::self)
            .def(py::self <  py::self) .def(py::self <= py::self)
            .def(py::self >  py::self) .def(py::self >= py::self)
            .def("__float__", [](const R &x){ return utils::cast<double>(x); })
            .def("__int__",   [](const R &x){ return static_cast<long>(utils::cast<double>(x)); })
            .def("__index__", [](const R &x){ return static_cast<long>(utils::cast<double>(x)); })
            .def("__getstate__", [](const R& x){ return utils::to_string(x); })
            .def("__setstate__", [](R& self, const std::string& s){ new (&self) R(s); });
    }

    //bind_complex_num<float_precision>(m, "CArb");
    bind_complex_num<float>(m, "CF32");
    bind_complex_num<double>(m, "CF64");
    bind_complex_num<long double>(m, "CFLong");
    bind_complex_num<mpfr::mpreal>(m, "CArb");

    bind_all<float, size_t>(m, "F32");
    bind_all<double, size_t>(m, "F64");
    bind_all<long double, size_t>(m, "FLong");
    bind_all<mpfr::mpreal, size_t>(m, "Arb");

    bind_all<std::complex<float>, size_t>(m, "CF32");
    bind_all<std::complex<double>, size_t>(m, "CF64");
    bind_all<std::complex<long double>, size_t>(m, "CFLong");
    bind_all<std::complex<mpfr::mpreal>, size_t>(m, "CArb");
}
