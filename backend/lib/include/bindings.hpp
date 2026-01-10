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
#include <pybind11/complex.h>

#include "../../src/include/custom_concepts.hpp"
#include "../../src/include/utils.hpp"
#include "../../src/include/methods.hpp"
#include "../../src/include/series.hpp"

namespace py = pybind11;

/**
 * @brief List of available type suffixes for Python exports.
 * Defined in names.cpp.
 */
#define OVERALL_ARB_TYPES 6
extern constinit const char* available_types[OVERALL_ARB_TYPES];

template<typename T>
struct RealTypeOf { using type = T; };

template<typename U>
struct RealTypeOf<std::complex<U>> { using type = U; };

template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_all(py::module_& m, const std::string& suffix);

/**
 * @brief Registers core numerical types (Arb, CArb, CF32, etc.) in the module.
 * 
 * These types must be registered BEFORE any functions that use them as
 * default arguments to avoid "type not registered" errors in pybind11.
 */
inline void bind_types(py::module_& m) {
    // Standard complex aliases for consistency with consumer code
    py::class_<std::complex<float>>(m, "CF32")
        .def(py::init<float, float>(), py::arg("real") = 0.0f, py::arg("imag") = 0.0f);
    py::class_<std::complex<double>>(m, "CF64")
        .def(py::init<double, double>(), py::arg("real") = 0.0, py::arg("imag") = 0.0);
    py::class_<std::complex<long double>>(m, "CFLong")
        .def(py::init<long double, long double>(), py::arg("real") = 0.0L, py::arg("imag") = 0.0L);

    // Arbitrary precision real type (MPFR)
    py::class_<mpfr::mpreal>(m, "Arb")
        .def(py::init<double>())
        .def(py::init<std::string>())
        .def(py::self + py::self).def(py::self - py::self)
        .def(py::self * py::self).def(py::self / py::self)
        .def("__repr__", [](const mpfr::mpreal& self) { return self.toString(); });

    // Arbitrary precision complex type
    py::class_<std::complex<mpfr::mpreal>>(m, "CArb")
        .def(py::init<mpfr::mpreal>())
        .def(py::init<mpfr::mpreal, mpfr::mpreal>())
        .def(py::self + py::self).def(py::self - py::self)
        .def(py::self * py::self).def(py::self / py::self);
}

/**
 * @brief Binds series result structures and series generators for a specific type.
 */
template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_series(pybind11::module_& m, const std::string& suffix){

    using MSeriesBase = shanks::series::series_base<T, K>;

    // Result container
    py::class_<series_result<T>>(m, (std::string("SeriesResult") + suffix).c_str())
        .def(py::init<>()) 
        .def(py::init<std::vector<T>, std::vector<T>>()) 
        .def_readwrite("Sn", &series_result<T>::Sn)
        .def_readwrite("an", &series_result<T>::an);

    // Polymorphic base class
    py::class_<MSeriesBase>(m, (std::string("SeriesBase") + suffix).c_str())
        .def("get_x", &MSeriesBase::get_x)
        .def("get_name", &MSeriesBase::get_name)
        .def("get_sum", &MSeriesBase::get_sum)
        .def("generate", &MSeriesBase::generate, py::arg("n"));

    // Automatic binding of all registered series
#define BIND_SERIES_IMPL(snake, camel, tName, kName) \
    { \
        auto c = py::class_<shanks::series::snake##_iterator<T, K>, MSeriesBase>(m, (std::string(camel) + suffix).c_str()); \
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
constexpr void bind_algos(pybind11::module_& m, const std::string& suffix){
    
    using RealT = typename RealTypeOf<T>::type;

    // Shanks transformation
    py::class_<shanks::algos::shanks_algorithm<T, K>>(m, (std::string("ShanksAlgorithm") + suffix).c_str())
        .def(py::init<>()) 
        .def("__call__", &shanks::algos::shanks_algorithm<T, K>::operator());

    // Wynn Epsilon algorithms
    py::class_<shanks::algos::wynn_epsilon_1_algorithm<T, K>>(m, (std::string("WynnEpsilon1Algorithm") + suffix).c_str())
        .def(py::init<>()) 
        .def("__call__", &shanks::algos::wynn_epsilon_1_algorithm<T, K>::operator());
    
    py::class_<shanks::algos::wynn_epsilon_2_algorithm<T, K>>(m, (std::string("WynnEpsilon2Algorithm") + suffix).c_str())
        .def(py::init<>()) 
        .def("__call__", &shanks::algos::wynn_epsilon_2_algorithm<T, K>::operator());

    // Wynn Rho algorithm
    py::class_<shanks::algos::wynn_rho_algorithm<T, K>>(m, (std::string("WynnRhoAlgorithm") + suffix).c_str())
        .def(py::init<shanks::numerators::numerator_type, RealT, RealT>(), 
             py::arg("numerator") = shanks::numerators::numerator_type::rho_type,
             py::arg("gamma") = RealT(-1.0),
             py::arg("RHO") = RealT(1.0))
        .def("__call__", &shanks::algos::wynn_rho_algorithm<T, K>::operator());

    // Levin transformation
    py::class_<shanks::algos::levin_algorithm<T, K>>(m, (std::string("LevinAlgorithm") + suffix).c_str())
        .def(py::init<shanks::remainders::remainder_type, bool, RealT>(), 
             py::arg("remainder") = shanks::remainders::remainder_type::u_type, 
             py::arg("useRecurrentFormula") = false,
             py::arg("beta") = RealT(1.0))
        .def("__call__", &shanks::algos::levin_algorithm<T, K>::operator());

    // Richardson extrapolation
    py::class_<shanks::algos::richardson_algorithm<T, K>>(m, (std::string("RichardsonAlgorithm") + suffix).c_str())
        .def(py::init<>()) 
        .def("__call__", &shanks::algos::richardson_algorithm<T, K>::operator());
}

template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_all(pybind11::module_& m, const std::string& suffix){
    bind_series<T,K>(m, suffix);
    bind_algos<T,K>(m, suffix);
}

template<typename TupleOfTypes, std::size_t TypesIndex = 0, std::size_t NameIndex = 0>
constexpr void bind_all_types(pybind11::module_& m){
    if constexpr (TypesIndex < std::tuple_size_v<TupleOfTypes> && NameIndex < OVERALL_ARB_TYPES){
        using tuple_from_tuple = std::tuple_element_t<TypesIndex, TupleOfTypes>;
        using TypeOfT = std::tuple_element_t<0, tuple_from_tuple>;
        using TypeOfK = std::tuple_element_t<1, tuple_from_tuple>;

        bind_all<TypeOfT, TypeOfK>(m, available_types[NameIndex]);
        bind_all_types<TupleOfTypes, TypesIndex + 1, NameIndex + 1>(m);
    }
}

#endif
