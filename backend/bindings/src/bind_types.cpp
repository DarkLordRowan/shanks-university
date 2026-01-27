#include "../include/bindings.hpp"

void bind_types(py::module_& m) {
#ifdef SHANKS_ENABLE_PROFILING
    py::class_<OP<float>>(m, "F32").def(py::init<float>());
    py::class_<OP<double>>(m, "F64").def(py::init<double>());
    py::class_<OP<long double>>(m, "FLong").def(py::init<long double>());

    py::implicitly_convertible<float, OP<float>>();
    py::implicitly_convertible<double, OP<double>>();
    py::implicitly_convertible<long double, OP<long double>>();
#endif

    auto bind_complex_props = []<typename T>(py::class_<std::complex<T>>& c) {
        c.def(py::self + py::self)
            .def(py::self - py::self)
            .def(py::self * py::self)
            .def(py::self / py::self)
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def(-py::self)
            .def_property(
                "real", [](const std::complex<T>& c) { return c.real(); }, [](std::complex<T>& c, T v) { c.real(v); })
            .def_property(
                "imag", [](const std::complex<T>& c) { return c.imag(); }, [](std::complex<T>& c, T v) { c.imag(v); })
            .def("__abs__", [](const std::complex<T>& c) { return std::abs(c); })
            .def("__hash__", [](const std::complex<T>& c) { return py::hash(py::make_tuple(c.real(), c.imag())); });
    };

    auto cf32 = py::class_<std::complex<OP<float>>>(m, "CF32")
                    .def(py::init<OP<float>, OP<float>>(), py::arg("real") = 0.0f, py::arg("imag") = 0.0f)
                    .def(py::pickle([](const std::complex<OP<float>>& c) { return py::make_tuple(c.real(), c.imag()); },
                                    [](py::tuple t) {
                                        return std::complex<OP<float>>(t[0].cast<OP<float>>(), t[1].cast<OP<float>>());
                                    }))
                    .def("__repr__", [](const std::complex<OP<float>>& c) {
                        std::ostringstream oss;
                        oss << "CF32(" << shanks::profiling::to_string(c.real()) << ", "
                            << shanks::profiling::to_string(c.imag()) << ")";
                        return oss.str();
                    });
    bind_complex_props(cf32);

    auto cf64 =
        py::class_<std::complex<OP<double>>>(m, "CF64")
            .def(py::init<OP<double>, OP<double>>(), py::arg("real") = 0.0, py::arg("imag") = 0.0)
            .def(py::pickle(
                [](const std::complex<OP<double>>& c) { return py::make_tuple(c.real(), c.imag()); },
                [](py::tuple t) { return std::complex<OP<double>>(t[0].cast<OP<double>>(), t[1].cast<OP<double>>()); }))
            .def("__repr__", [](const std::complex<OP<double>>& c) {
                std::ostringstream oss;
                oss << "CF64(" << shanks::profiling::to_string(c.real()) << ", "
                    << shanks::profiling::to_string(c.imag()) << ")";
                return oss.str();
            });
    bind_complex_props(cf64);

    auto cfLong =
        py::class_<std::complex<OP<long double>>>(m, "CFLong")
            .def(py::init<OP<long double>, OP<long double>>(), py::arg("real") = 0.0L, py::arg("imag") = 0.0L)
            .def(py::pickle([](const std::complex<OP<long double>>& c) { return py::make_tuple(c.real(), c.imag()); },
                            [](py::tuple t) {
                                return std::complex<OP<long double>>(t[0].cast<OP<long double>>(),
                                                                     t[1].cast<OP<long double>>());
                            }))
            .def("__repr__", [](const std::complex<OP<long double>>& c) {
                std::ostringstream oss;
                oss << "CFLong(" << shanks::profiling::to_string(c.real()) << ", "
                    << shanks::profiling::to_string(c.imag()) << ")";
                return oss.str();
            });
    bind_complex_props(cfLong);

    py::class_<OP<mpfr::mpreal>>(m, "Arb")
        .def(py::init<double, mp_prec_t>(), py::arg("d"), py::arg("prec"))
        .def(py::init<std::string, mp_prec_t>(), py::arg("s"), py::arg("prec"))
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(-py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self <= py::self)
        .def(py::self >= py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__abs__",
             [](const shanks::profiling::OperationCounting<mpfr::mpreal>& self) {
                 using shanks::profiling::abs;
                 return abs(self);
             })
        .def("__pow__",
             [](const shanks::profiling::OperationCounting<mpfr::mpreal>& self,
                const shanks::profiling::OperationCounting<mpfr::mpreal>& exp) {
                 using shanks::profiling::pow;
                 return pow(self, exp);
             })
        .def("sqrt",
             [](const shanks::profiling::OperationCounting<mpfr::mpreal>& self) {
                 using shanks::profiling::sqrt;
                 return sqrt(self);
             })
        .def("__hash__",
             [](const OP<mpfr::mpreal>& self) { return py::hash(py::str(shanks::profiling::to_string(self))); })
        .def("__repr__", [](const OP<mpfr::mpreal>& self) { return shanks::profiling::to_string(self); })
        .def(py::pickle([](const OP<mpfr::mpreal>& num) { return shanks::profiling::to_string(num); },
                        [](std::string s) { return OP<mpfr::mpreal>(s); }));

    auto cArb =
        py::class_<std::complex<OP<mpfr::mpreal>>>(m, "CArb")
            .def(py::init<OP<mpfr::mpreal>>())
            .def(py::init<OP<mpfr::mpreal>, OP<mpfr::mpreal>>())
            .def(py::pickle([](const std::complex<OP<mpfr::mpreal>>& c) { return py::make_tuple(c.real(), c.imag()); },
                            [](py::tuple t) {
                                return std::complex<OP<mpfr::mpreal>>(t[0].cast<OP<mpfr::mpreal>>(),
                                                                      t[1].cast<OP<mpfr::mpreal>>());
                            }))
            .def("__repr__", [](const std::complex<OP<mpfr::mpreal>>& c) {
                std::ostringstream oss;
                oss << "CArb(" << shanks::profiling::to_string(c.real()) << ", "
                    << shanks::profiling::to_string(c.imag()) << ")";
                return oss.str();
            });
    bind_complex_props(cArb);

    py::implicitly_convertible<double, OP<mpfr::mpreal>>();
    py::implicitly_convertible<long, OP<mpfr::mpreal>>();
    py::implicitly_convertible<std::string, OP<mpfr::mpreal>>();
    py::implicitly_convertible<float, std::complex<OP<float>>>();
    py::implicitly_convertible<double, std::complex<OP<double>>>();
    py::implicitly_convertible<long double, std::complex<OP<long double>>>();
    py::implicitly_convertible<mpfr::mpreal, std::complex<OP<mpfr::mpreal>>>();
}
