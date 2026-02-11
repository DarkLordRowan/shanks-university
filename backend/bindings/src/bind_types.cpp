#include "../include/bindings.hpp"

namespace py = pybind11;

void bind_types(py::module_& m) {
#ifdef SHANKS_ENABLE_PROFILING
    // 0. Bind profiling utils if enabled
    py::class_<shanks::profiling::OperationCounts>(m, "OperationCounts")
        .def_readonly("add", &shanks::profiling::OperationCounts::add)
        .def_readonly("mul", &shanks::profiling::OperationCounts::mul)
        .def_readonly("div", &shanks::profiling::OperationCounts::div)
        .def_readonly("special", &shanks::profiling::OperationCounts::special)
        .def("__repr__", [](const shanks::profiling::OperationCounts& oc) {
            std::ostringstream oss;
            oss << "OperationCounts(add=" << oc.add << ", mul=" << oc.mul << ", div=" << oc.div
                << ", special=" << oc.special << ")";
            return oss.str();
        });

    m.def("reset_operation_counts", &shanks::profiling::reset_counts);
    m.def("get_operation_counts", &shanks::profiling::get_counts);

    auto bind_real_props = []<typename T>(py::class_<OP<T>>& c) {
        c.def(py::self + py::self)
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
            .def("__abs__", [](const OP<T>& self) { return abs(self); })
            .def("__pow__", [](const OP<T>& self, const OP<T>& exp) { return pow(self, exp); })
            .def("sqrt", [](const OP<T>& self) { return sqrt(self); })
            .def("__hash__",
                 [](const OP<T>& self) { return py::hash(py::str(utils::helpers<OP<T>>::to_string(self))); })
            .def("__repr__", [](const OP<T>& self) { return utils::helpers<OP<T>>::to_string(self); })
            .def(py::pickle([](const OP<T>& num) { return num.value; }, [](T val) { return OP<T>(val); }));
    };

    auto f32 = py::class_<OP<float>>(m, "F32").def(py::init<float>());
    bind_real_props(f32);
    auto f64 = py::class_<OP<double>>(m, "F64").def(py::init<double>());
    bind_real_props(f64);
    auto fLong = py::class_<OP<long double>>(m, "FLong").def(py::init<long double>());
    bind_real_props(fLong);

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
                        oss << "CF32" + utils::helpers<OP<std::complex<OP<float>>>>::to_string(c);
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
                oss << "CF64" + utils::helpers<OP<std::complex<OP<double>>>>::to_string(c);
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
                oss << "CFLong" + utils::helpers<OP<std::complex<OP<long double>>>>::to_string(c);
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
        .def("__abs__", [](const OP<mpfr::mpreal>& self) { return abs(self); })
        .def("__pow__", [](const OP<mpfr::mpreal>& self, const OP<mpfr::mpreal>& exp) { return pow(self, exp); })
        .def("sqrt", [](const OP<mpfr::mpreal>& self) { return sqrt(self); })
        .def("__hash__",
             [](const OP<mpfr::mpreal>& self) {
                 return py::hash(py::str(utils::helpers<OP<mpfr::mpreal>>::to_string(self)));
             })
        .def("__repr__", [](const OP<mpfr::mpreal>& self) { return utils::helpers<OP<mpfr::mpreal>>::to_string(self); })
        .def(py::pickle([](const OP<mpfr::mpreal>& num) { return utils::helpers<OP<mpfr::mpreal>>::to_string(num); },
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
                oss << "CArb" + utils::helpers<std::complex<OP<mpfr::mpreal>>>::to_string(c);
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
#ifdef __MPREAL_H__
    m.def("set_default_precision", [](size_t bits) { mpfr::mpreal::set_default_prec(static_cast<mp_prec_t>(bits)); });
    m.def("get_default_precision", []() { return static_cast<size_t>(mpfr::mpreal::get_default_prec()); });
#endif
}
