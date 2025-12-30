#ifndef BINDINGS_HPP
#define BINDINGS_HPP
#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../shanks/include/custom_concepts.hpp"
#include "../../shanks/include/utils.hpp"

#include "../../shanks/include/methods.hpp"
#include "../../shanks/include/series.hpp"

#include "bind_series.hpp"
#include "bind_algos.hpp"

namespace py = pybind11;

template <FloatLike T>
constexpr void bind_complex_num(py::module_& m, const char* pyname);

template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_all(py::module_& m, const std::string& suffix);

#define OVERALL_ARB_TYPES 8
extern constinit const char* available_types[OVERALL_ARB_TYPES];

template<typename TupleOfTypes, std::size_t TypesIndex = 0, std::size_t NameIndex = 0>
constexpr void bind_comlex_types(py::module_& m){
    if constexpr (TypesIndex < std::tuple_size_v<TupleOfTypes> && NameIndex < OVERALL_ARB_TYPES){
        using type_from_tuple = std::tuple_element_t<TypesIndex, TupleOfTypes>;
        bind_complex_num<type_from_tuple>(m, available_types[NameIndex]);
        bind_comlex_types<TupleOfTypes, TypesIndex + 1, NameIndex + 1>(m);
    } else return;
}


template<typename TupleOfTypes, std::size_t TypesIndex = 0, std::size_t NameIndex = 0>
constexpr void bind_all_types(py::module_& m){
    if constexpr (TypesIndex < std::tuple_size_v<TupleOfTypes> && NameIndex < OVERALL_ARB_TYPES){
        using tuple_from_tuple = std::tuple_element_t<TypesIndex, TupleOfTypes>;
        using TypeOfT = std::tuple_element_t<0, tuple_from_tuple>;
        using TypeOfK = std::tuple_element_t<1, tuple_from_tuple>;
        bind_all<TypeOfT, TypeOfK>(m, available_types[NameIndex]);
        bind_all_types<TupleOfTypes, TypesIndex + 1, NameIndex + 1>(m);
    } else return;
}

template<typename T>
requires is_precisable<T>::value
void bind_arb_real_num(py::module_& m, const char* name){
    py::class_<T>(m, name)
        .def(
        py::init([](const double& d, const size_t precision){
            T value = utils::cast<T>(d); utils::set_precision(precision, value);
            return value;
        }), py::arg("d"), py::arg("precision"))
        .def(
        py::init([](const std::string& s, const size_t precision){
            T value = utils::cast<T>(s); utils::set_precision(precision, value);
            return value;
        }), py::arg("s"), py::arg("precision"))
        .def("__str__", [](const T &x){ return utils::to_string(x) ; })
        .def("__format__", [](const T &x, const std::string&){ return utils::to_string(x); })
        .def("__repr__", [&name](const T &x){ return std::string("<") + name + ": " + utils::to_string(x) + ">"; })
        .def(py::self + py::self) .def(py::self - py::self)
        .def(py::self * py::self) .def(py::self / py::self)
        .def(-py::self) .def(+py::self)
        .def("__abs__", [](const T& a){ return utils::abs(a); })
        .def(py::self == py::self) .def(py::self != py::self)
        .def(py::self <  py::self) .def(py::self <= py::self)
        .def(py::self >  py::self) .def(py::self >= py::self)
        .def("__float__", [](const T &x){ return utils::cast<double>(x); })
        .def("__int__",   [](const T &x){ return static_cast<long>(utils::cast<double>(x)); })
        .def("__index__", [](const T &x){ return static_cast<long>(utils::cast<double>(x)); })
        .def("__getstate__", [](const T& x){ return utils::to_string(x); })
        .def("__setstate__", [](T& self, const std::string& s){ new (&self) T(s); });

}

template <FloatLike T>
constexpr void bind_complex_num(py::module_& m, const char* pyname) {

    using C = std::complex<T>;

    using State = std::conditional_t<std::is_same_v<T, mpfr::mpreal>, std::pair<std::string, std::string>, std::pair<T, T>>;

    auto cls = py::class_<C>(m, pyname)
        .def(py::init<>())
        .def(py::init<T>(), py::arg("re"))
        .def(py::init<T, T>(), py::arg("re"), py::arg("im"))
        .def(py::init([](std::complex<double> z){
            return C(utils::cast<T>(z.real()), utils::cast<T>(z.imag()));
        }), py::arg("z"))
        .def_property("real",
            [](const C& z){ return z.real(); },
            [](C& z, const T& r){ z.real(r); })
        .def_property("imag",
            [](const C& z){ return z.imag(); },
            [](C& z, const T& i){ z.imag(i); })
        .def("__str__", [](const C& z){ return utils::to_string(z); })
        .def("__format__", [](const C& z, const std::string&){ return utils::to_string(z); })
        .def("__repr__", [](const C& z){return utils::to_string(z);})
        .def("__complex__", [](const C& z){ return std::complex<double>(utils::cast<double>(z.real()), utils::cast<double>(z.imag())); })
        .def(-py::self)
        .def(+py::self)
        .def("__abs__", [](const C& a){ return utils::abs(a); })
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__getstate__", [](const C& z)->State{
            if constexpr (is_precisable<T>::value) return {utils::to_string(z.real()), utils::to_string(z.imag())};
            else return { z.real(), z.imag() };
        })
        .def("__setstate__", [](C& z, const State& s){
            if constexpr (is_precisable<T>::value) new (&z) C(utils::cast<T>(s.first), utils::cast<T>(s.second));
            else new (&z) C(s.first, s.second);
        });
}

template <AcceptedLike T, UnsignedIntLike K>
constexpr void bind_all(py::module_& m, const std::string& suffix){
    bind_series<T,K>(m, suffix);
    bind_algos<T,K>(m, suffix);
}

#endif