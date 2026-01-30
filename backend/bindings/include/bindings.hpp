/**
 * @file bindings.hpp
 * @brief Template-based pybind11 bindings for the library.
 * @authors Naumov A.U., Sobolev Y. A.
 */

#ifndef BINDINGS_HPP
#define BINDINGS_HPP
#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <array>
#include <complex>
#include <sstream>
#include <tuple>

#include "../../core/include/custom_concepts.hpp"
#include "../../core/include/custom_types/operation_counter.hpp"
#include "../../core/include/utils.hpp"

namespace py = pybind11;

// --- Shared Utilities ---

inline std::string create_name(const char* name, const char* suffix) { return std::string(name) + std::string(suffix); }

// Helper alias for conditional wrapper
#ifdef SHANKS_ENABLE_PROFILING
template <typename T>
using OP = shanks::profiling::OperationCounting<T>;
#else
template <typename T>
using OP = T;
#endif

// Common types tuples and suffixes for splitting compilation
using types_real = std::tuple<std::tuple<OP<float>, size_t>, std::tuple<OP<double>, size_t>,
                              std::tuple<OP<long double>, size_t>, std::tuple<OP<mpfr::mpreal>, size_t>>;

using types_complex =
    std::tuple<std::tuple<std::complex<OP<float>>, size_t>, std::tuple<std::complex<OP<double>>, size_t>,
               std::tuple<std::complex<OP<long double>>, size_t>, std::tuple<std::complex<OP<mpfr::mpreal>>, size_t>>;

constexpr std::array<const char*, 4> suffixes_real{"F32", "F64", "FLong", "Arb"};
constexpr std::array<const char*, 4> suffixes_complex{"CF32", "CF64", "CFLong", "CArb"};

// Common types tuple and suffixes for all binding units (kept for simple cases)
using types_to_bind =
    std::tuple<std::tuple<OP<float>, size_t>, std::tuple<OP<double>, size_t>, std::tuple<OP<long double>, size_t>,
               std::tuple<OP<mpfr::mpreal>, size_t>, std::tuple<std::complex<OP<float>>, size_t>,
               std::tuple<std::complex<OP<double>>, size_t>, std::tuple<std::complex<OP<long double>>, size_t>,
               std::tuple<std::complex<OP<mpfr::mpreal>>, size_t>>;

constexpr std::array<const char*, 8> suffixes{"F32", "F64", "FLong", "Arb", "CF32", "CF64", "CFLong", "CArb"};

// --- Binding Function Declarations ---

void bind_types(py::module_& m);
void bind_series_real(py::module_& m);
void bind_series_complex(py::module_& m);
void bind_algos_real(py::module_& m);
void bind_algos_complex(py::module_& m);
void bind_noise_all(py::module_& m);
void bind_filters_all(py::module_& m);

// --- Template Helper for multiple types ---

template <typename TupleOfTypes, size_t N, typename Func, std::size_t TypesIndex = 0>
constexpr void bind_each_type(py::module_& m, const std::array<const char*, N>& s, Func f) {
    if constexpr (TypesIndex < std::tuple_size_v<TupleOfTypes>) {
        using tuple_from_tuple = std::tuple_element_t<TypesIndex, TupleOfTypes>;
        using TypeOfT = std::tuple_element_t<0, tuple_from_tuple>;
        using TypeOfK = std::tuple_element_t<1, tuple_from_tuple>;
        f.template operator()<TypeOfT, TypeOfK>(m, s[TypesIndex]);
        bind_each_type<TupleOfTypes, N, Func, TypesIndex + 1>(m, s, f);
    }
}

#endif
