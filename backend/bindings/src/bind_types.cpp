#include "../include/bindings.hpp"

void bind_types(py::module_& m) {
    auto bind_complex_props = []<typename T>(py::class_<std::complex<T>>& c) {
        c.def(py::self + py::self)
         .def(py::self - py::self)
         .def(py::self * py::self)
         .def(py::self / py::self)
         .def(py::self == py::self)
         .def(py::self != py::self)
         .def(-py::self)
         .def_property("real", 
             [](const std::complex<T> &c) { return c.real(); }, 
             [](std::complex<T> &c, T v) { c.real(v); })
         .def_property("imag", 
             [](const std::complex<T> &c) { return c.imag(); }, 
             [](std::complex<T> &c, T v) { c.imag(v); })
         .def("__abs__", [](const std::complex<T> &c) { return std::abs(c); })
         .def("__hash__", [](const std::complex<T> &c) {
             return py::hash(py::make_tuple(c.real(), c.imag()));
         });
    };

    auto cf32 = py::class_<std::complex<float>>(m, "CF32")
        .def(py::init<float, float>(), py::arg("real") = 0.0f, py::arg("imag") = 0.0f)
        .def(py::pickle(
            [](const std::complex<float> &c) { return py::make_tuple(c.real(), c.imag()); },
            [](py::tuple t) { return std::complex<float>(t[0].cast<float>(), t[1].cast<float>()); }
        ))
        .def("__repr__", [](const std::complex<float> &c) {
             std::ostringstream oss;
             oss << "(" << c.real() << (c.imag() >= 0 ? "+" : "") << c.imag() << "j)";
             return oss.str();
        });
    bind_complex_props(cf32);

    auto cf64 = py::class_<std::complex<double>>(m, "CF64")
        .def(py::init<double, double>(), py::arg("real") = 0.0, py::arg("imag") = 0.0)
        .def(py::pickle(
            [](const std::complex<double> &c) { return py::make_tuple(c.real(), c.imag()); },
            [](py::tuple t) { return std::complex<double>(t[0].cast<double>(), t[1].cast<double>()); }
        ))
        .def("__repr__", [](const std::complex<double> &c) {
             std::ostringstream oss;
             oss << "(" << c.real() << (c.imag() >= 0 ? "+" : "") << c.imag() << "j)";
             return oss.str();
        });
    bind_complex_props(cf64);

    auto cfLong = py::class_<std::complex<long double>>(m, "CFLong")
        .def(py::init<long double, long double>(), py::arg("real") = 0.0L, py::arg("imag") = 0.0L)
        .def(py::pickle(
            [](const std::complex<long double> &c) { return py::make_tuple(c.real(), c.imag()); },
            [](py::tuple t) { return std::complex<long double>(t[0].cast<long double>(), t[1].cast<long double>()); }
        ))
        .def("__repr__", [](const std::complex<long double> &c) {
             std::ostringstream oss;
             oss << "(" << c.real() << (c.imag() >= 0 ? "+" : "") << c.imag() << "j)";
             return oss.str();
        });
    bind_complex_props(cfLong);

    py::class_<mpfr::mpreal>(m, "Arb")
        .def(py::init<double, mp_prec_t>(), py::arg("d"), py::arg("prec"))
        .def(py::init<std::string, mp_prec_t>(), py::arg("s"), py::arg("prec"))
        .def(py::self + py::self).def(py::self - py::self)
        .def(py::self * py::self).def(py::self / py::self)
        .def(-py::self)
        .def(py::self < py::self).def(py::self > py::self)
        .def(py::self <= py::self).def(py::self >= py::self)
        .def(py::self == py::self).def(py::self != py::self)
        .def("__abs__", [](const mpfr::mpreal& self) { return mpfr::abs(self); })
        .def("__pow__", [](const mpfr::mpreal& self, const mpfr::mpreal& exp) { return mpfr::pow(self, exp); })
        .def("sqrt", [](const mpfr::mpreal& self) { return mpfr::sqrt(self); })
        .def("__hash__", [](const mpfr::mpreal& self) { return py::hash(py::str(self.toString())); })
        .def("__repr__", [](const mpfr::mpreal& self) { return self.toString(); })
        .def(py::pickle([](const mpfr::mpreal &num) { return num.toString(); }, [](std::string s) { return mpfr::mpreal(s); }));

    auto cArb = py::class_<std::complex<mpfr::mpreal>>(m, "CArb")
        .def(py::init<mpfr::mpreal>())
        .def(py::init<mpfr::mpreal, mpfr::mpreal>())
        .def(py::pickle(
            [](const std::complex<mpfr::mpreal> &c) { return py::make_tuple(c.real(), c.imag()); },
            [](py::tuple t) { return std::complex<mpfr::mpreal>(t[0].cast<mpfr::mpreal>(), t[1].cast<mpfr::mpreal>()); }
        ))
        .def("__repr__", [](const std::complex<mpfr::mpreal> &c) {
             std::string r = c.real().toString();
             std::string i = c.imag().toString();
             if (i.empty() || i[0] != '-') return "(" + r + "+" + i + "j)";
             return "(" + r + i + "j)"; 
        });
    bind_complex_props(cArb);

    py::implicitly_convertible<double, mpfr::mpreal>();
    py::implicitly_convertible<long, mpfr::mpreal>();
    py::implicitly_convertible<std::string, mpfr::mpreal>();
    py::implicitly_convertible<float, std::complex<float>>();
    py::implicitly_convertible<double, std::complex<double>>();
    py::implicitly_convertible<long double, std::complex<long double>>();
    py::implicitly_convertible<mpfr::mpreal, std::complex<mpfr::mpreal>>();
}
