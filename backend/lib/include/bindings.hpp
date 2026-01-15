/**
 * @file bindings.hpp
 * @brief Template-based pybind11 bindings for the Shanks University library.
 *
 * This file provides a declarative way to export C++ series and acceleration
 * algorithms to Python. It uses C++20 concepts and template metaprogramming
 * to automatically generate bindings for multiple numerical precisions.
 *
 * @authors Sobolev Y. A., Naumov A.U.
 */

#ifndef BINDINGS_HPP
#define BINDINGS_HPP
#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

#include "../../src/include/custom_concepts.hpp"
#include "../../src/include/utils.hpp"
#include "../../src/include/methods.hpp"
#include "../../src/include/series.hpp"
#include "../../src/include/noise/noise_generator.hpp"
#include "../../src/include/filters/kolzur.hpp"
#include "../../src/include/filters/savgol.hpp"

namespace py = pybind11;

template<typename T>
struct RealTypeOf { using type = T; };

constexpr std::string create_name(const char* name, const char* suffix){ return std::string(name) + std::string(suffix);}

template<typename U>
struct RealTypeOf<std::complex<U>> { using type = U; };

/**
 * @brief Registers core numerical types (Arb, CArb, CF32, etc.) in the module.
 *
 * These types must be registered BEFORE any functions that use them as
 * default arguments to avoid "type not registered" errors in pybind11.
 */
inline void bind_types(py::module_& m) {
    // Helper lambda for complex properties
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

    // Standard complex aliases
    auto cf32 = py::class_<std::complex<float>>(m, "CF32")
        .def(py::init<float, float>(), py::arg("real") = 0.0f, py::arg("imag") = 0.0f)
        .def(py::pickle(
            [](const std::complex<float> &c) { return py::make_tuple(c.real(), c.imag()); },
            [](py::tuple t) { 
                if (t.size() != 2) throw std::runtime_error("Invalid state!");
                return std::complex<float>(t[0].cast<float>(), t[1].cast<float>()); 
            }
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
            [](py::tuple t) { 
                if (t.size() != 2) throw std::runtime_error("Invalid state!");
                return std::complex<double>(t[0].cast<double>(), t[1].cast<double>()); 
            }
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
            [](py::tuple t) { 
                if (t.size() != 2) throw std::runtime_error("Invalid state!");
                return std::complex<long double>(t[0].cast<long double>(), t[1].cast<long double>()); 
            }
        ))
        .def("__repr__", [](const std::complex<long double> &c) {
             std::ostringstream oss;
             oss << "(" << c.real() << (c.imag() >= 0 ? "+" : "") << c.imag() << "j)";
             return oss.str();
        });
    bind_complex_props(cfLong);

    // Arbitrary precision real type (MPFR)
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
        .def("__hash__", [](const mpfr::mpreal& self) {
            return py::hash(py::str(self.toString()));
        })
        .def("__repr__", [](const mpfr::mpreal& self) { return self.toString(); })
        .def(py::pickle(
            [](const mpfr::mpreal &num) { return num.toString(); },
            [](std::string s) { return mpfr::mpreal(s); }
        ));

    // Arbitrary precision complex type
    auto cArb = py::class_<std::complex<mpfr::mpreal>>(m, "CArb")
        .def(py::init<mpfr::mpreal>())
        .def(py::init<mpfr::mpreal, mpfr::mpreal>())
        .def(py::pickle(
            [](const std::complex<mpfr::mpreal> &c) { return py::make_tuple(c.real(), c.imag()); },
            [](py::tuple t) { 
                if (t.size() != 2) throw std::runtime_error("Invalid state!");
                return std::complex<mpfr::mpreal>(t[0].cast<mpfr::mpreal>(), t[1].cast<mpfr::mpreal>()); 
            }
        ))
        .def("__repr__", [](const std::complex<mpfr::mpreal> &c) {
             std::string r = c.real().toString();
             std::string i = c.imag().toString();
             if (i.empty() || i[0] != '-') return "(" + r + "+" + i + "j)";
             return "(" + r + i + "j)"; 
        });
    bind_complex_props(cArb);

    // Implicit conversions
    py::implicitly_convertible<double, mpfr::mpreal>();
    py::implicitly_convertible<long, mpfr::mpreal>();
    py::implicitly_convertible<std::string, mpfr::mpreal>();

    py::implicitly_convertible<float, std::complex<float>>();
    py::implicitly_convertible<double, std::complex<double>>();
    py::implicitly_convertible<long double, std::complex<long double>>();
    py::implicitly_convertible<mpfr::mpreal, std::complex<mpfr::mpreal>>();
}

/**
 * @brief Binds series result structures and series generators for a specific type.
 */
template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_series(pybind11::module_& m, const char* suffix){

    using MSeriesBase = shanks::series::series_base<T, K>;

    // Result container
    py::class_<series_result<T>>(m, create_name("SeriesResult", suffix).c_str())
        .def(py::init<>())
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readwrite("Sn", &series_result<T>::Sn)
        .def_readwrite("an", &series_result<T>::an);

    // Polymorphic base class
    py::class_<MSeriesBase>(m, create_name("SeriesBase", suffix).c_str())
        .def("get_x", &MSeriesBase::get_x)
        .def("get_name", &MSeriesBase::get_name)
        .def("get_sum", &MSeriesBase::get_sum)
        .def("generate", &MSeriesBase::generate, py::arg("n"));

    // Automatic binding of all registered series
#define BIND_SERIES_IMPL(snake, camel, tName, kName) \
    { \
        auto c = py::class_<shanks::series::snake##_iterator<T, K>, MSeriesBase>(m, create_name(camel, suffix).c_str()); \
        if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T>) \
            c.def(py::init<T>(), py::arg("x")); \
        if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T, K>) \
            c.def(py::init<T, T, K>(), py::arg("x"), py::arg(tName), py::arg(kName)); \
        if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T>) \
            c.def(py::init<T, T>(), py::arg("x"), py::arg(tName)); \
        if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, K>) \
            c.def(py::init<T, K>(), py::arg("x"), py::arg(kName)); \
        c.def("generateSeries", &shanks::series::snake##_iterator<T, K>::generate, py::arg("n")); \
    }

#define SERIES_ENTRY(snake, camel) BIND_SERIES_IMPL(snake, camel, "tParam", "kParam")
#define SERIES_LAST(snake, camel) SERIES_ENTRY(snake, camel)
#define SERIES_ENTRY_ARGS(snake, camel, tName, kName) BIND_SERIES_IMPL(snake, camel, tName, kName)
#define SERIES_LAST_ARGS(snake, camel, tName, kName) SERIES_ENTRY_ARGS(snake, camel, tName, kName)

#include "../../src/include/series_registry.def"

#undef BIND_SERIES_IMPL
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS
};

/**
 * @brief Binds sequence transformation algorithms as Python classes (functors).
 */
template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_algos(pybind11::module_& m, const char* suffix){

    using RealT = typename RealTypeOf<T>::type;

    // Shanks transformation
    py::class_<shanks::algos::shanks_algorithm<T, K>>(m, create_name("ShanksAlgorithm", suffix).c_str())
        .def(py::init<>())
        .def("__call__", &shanks::algos::shanks_algorithm<T, K>::operator());

    // Wynn Epsilon algorithms
    py::class_<shanks::algos::wynn_epsilon_1_algorithm<T, K>>(m, create_name("WynnEpsilon1Algorithm", suffix).c_str())
        .def(py::init<>())
        .def("__call__", &shanks::algos::wynn_epsilon_1_algorithm<T, K>::operator());

    py::class_<shanks::algos::wynn_epsilon_2_algorithm<T, K>>(m, create_name("WynnEpsilon2Algorithm", suffix).c_str())
        .def(py::init<>())
        .def("__call__", &shanks::algos::wynn_epsilon_2_algorithm<T, K>::operator());

    // Wynn Rho algorithm
    py::class_<shanks::algos::wynn_rho_algorithm<T, K>>(m, create_name("WynnRhoAlgorithm", suffix).c_str())
        .def(py::init<shanks::numerators::numerator_type, RealT, RealT>(),
             py::arg("numerator") = shanks::numerators::numerator_type::rho_type,
             py::arg("gamma") = RealT(-1.0),
             py::arg("RHO") = RealT(1.0))
        .def("__call__", &shanks::algos::wynn_rho_algorithm<T, K>::operator());

    // Levin transformation
    py::class_<shanks::algos::levin_algorithm<T, K>>(m, create_name("LevinAlgorithm", suffix).c_str())
        .def(py::init<shanks::remainders::remainder_type, bool, RealT>(),
             py::arg("remainder") = shanks::remainders::remainder_type::u_type,
             py::arg("useRecurrentFormula") = false,
             py::arg("beta") = RealT(1.0))
        .def("__call__", &shanks::algos::levin_algorithm<T, K>::operator());

    // Richardson extrapolation
    py::class_<shanks::algos::richardson_algorithm<T, K>>(m, create_name("RichardsonAlgorithm", suffix).c_str())
        .def(py::init<>())
        .def("__call__", &shanks::algos::richardson_algorithm<T, K>::operator());
}

/**
 * @brief Binds noise generator.
 */
template <AcceptedLike T>
constexpr void bind_noise(pybind11::module_& m, const char* suffix){

    m.def(create_name("applyNoise",suffix).c_str(),
        [](const series_result<T>& result, NoiseMethod method, NoiseType type, unsigned long long int seed, const T& p1, const T& p2) {
             unsigned long long int actual_seed = (seed == 0) ? pseudo_random_seed : seed;
             return apply_noise(result, method, type, actual_seed, p1, p2);
        },
        py::arg("result"), py::arg("method"), py::arg("type"), py::arg("seed") = 0, py::arg("param1"), py::arg("param2") = T()
    );
}

/**
 * @brief Binds filters
 */
template<AcceptedLike T>
constexpr void bind_filters(pybind11::module_& m, const char* suffix){

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

template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_all(pybind11::module_& m, const char* suffix){
    bind_series<T,K>(m, suffix);
    bind_algos<T,K>(m, suffix);
    bind_noise<T>(m, suffix);
    bind_filters<T>(m,suffix);
}

template<typename TupleOfTypes, size_t N, std::size_t TypesIndex = 0>
constexpr void bind_all_types(pybind11::module_& m, const std::array<const char*, N>& suffixes){
    if constexpr (TypesIndex < std::tuple_size_v<TupleOfTypes>){
        using tuple_from_tuple = std::tuple_element_t<TypesIndex, TupleOfTypes>;
        using TypeOfT = std::tuple_element_t<0, tuple_from_tuple>;
        using TypeOfK = std::tuple_element_t<1, tuple_from_tuple>;

        bind_all<TypeOfT, TypeOfK>(m, suffixes[TypesIndex]);
        bind_all_types<TupleOfTypes, N, TypesIndex + 1>(m, suffixes);
    }
}

#endif
