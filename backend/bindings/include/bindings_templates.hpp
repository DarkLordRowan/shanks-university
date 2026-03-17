/**
 * @file binding_templates.hpp
 * @brief All the pybind11 bindings.
 * @authors Naumov A.U., Sobolev Y. A.
 */

#ifndef BINDINGS_TEMPLATES_HPP
#define BINDINGS_TEMPLATES_HPP
#pragma once

#include "bindings.hpp"

/**
 * @brief Template implementation for binding series.
 */
template <AcceptedLike T, UnsignedIntLike K>
void bind_series(pybind11::module_& m, const char* suffix) {
    py::class_<series_result<T>>(m, create_name("SeriesResult", suffix).c_str())
        .def(py::init<>())
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readwrite("Sn", &series_result<T>::Sn)
        .def_readwrite("an", &series_result<T>::an);

#define BIND_SERIES_IMPL(snake, camel, tName, kName)                                 \
    m.def(                                                                           \
        create_name(camel, suffix).c_str(),                                          \
        [](K n, T x, T t, K k) {                                                     \
            using S = shanks::series::snake##_iterator<T, K>;                        \
            std::unique_ptr<S> it;                                                   \
            if constexpr (std::is_constructible_v<S, T, T, K>)                       \
                it = std::make_unique<S>(x, t, k);                                   \
            else if constexpr (std::is_constructible_v<S, T, T>)                     \
                it = std::make_unique<S>(x, t);                                      \
            else if constexpr (std::is_constructible_v<S, T, K>)                     \
                it = std::make_unique<S>(x, k);                                      \
            else                                                                     \
                it = std::make_unique<S>(x);                                         \
            return std::make_pair(it->generate(n), it->get_sum());                   \
        },                                                                           \
        py::arg("n"), py::arg("x") = T(0), py::arg(tName ? tName : "tParam") = T(1), \
        py::arg(kName ? kName : "kParam") = K(1));

#define SERIES_ENTRY(snake, camel) BIND_SERIES_IMPL(snake, camel, nullptr, nullptr)
#define SERIES_LAST(snake, camel) SERIES_ENTRY(snake, camel)
#define SERIES_ENTRY_ARGS(snake, camel, tName, kName) BIND_SERIES_IMPL(snake, camel, tName, kName)
#define SERIES_LAST_ARGS(snake, camel, tName, kName) SERIES_ENTRY_ARGS(snake, camel, tName, kName)

#include "../../core/include/series_registry.def"

#undef BIND_SERIES_IMPL
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS
}

/**
 * @brief Template implementation for binding algorithms.
 */
template <AcceptedLike T, UnsignedIntLike K>
void bind_algos(pybind11::module_& m, const char* suffix) {
    using RealT = typename real_of<T>::value;

#define BIND_DEFAULT(cls, name)                                                                                \
    m.def(                                                                                                     \
        create_name(name, suffix).c_str(),                                                                     \
        [](K n, K order, const series_result<T>& data) { return shanks::algos::cls<T, K>()(n, order, data); }, \
        py::arg("n"), py::arg("order"), py::arg("data"))

#define BIND_REM(cls, name)                                                                            \
    m.def(                                                                                             \
        create_name(name, suffix).c_str(),                                                             \
        [](K n, K order, const series_result<T>& data, shanks::remainders::remainder_type remainder) { \
            return shanks::algos::cls<T, K>(remainder)(n, order, data);                                \
        },                                                                                             \
        py::arg("n"), py::arg("order"), py::arg("data"),                                               \
        py::arg("remainder") = shanks::remainders::remainder_type::u_type)

#define BIND_REM_BOOL(cls, name)                                                                     \
    m.def(                                                                                           \
        create_name(name, suffix).c_str(),                                                           \
        [](K n, K order, const series_result<T>& data, shanks::remainders::remainder_type remainder, \
           bool useRecurrentFormula) {                                                               \
            return shanks::algos::cls<T, K>(remainder, useRecurrentFormula)(n, order, data);         \
        },                                                                                           \
        py::arg("n"), py::arg("order"), py::arg("data"),                                             \
        py::arg("remainder") = shanks::remainders::remainder_type::u_type, py::arg("useRecurrentFormula") = false)

#define BIND_LEVIN_BASE(cls, name)                                                                                 \
    m.def(                                                                                                         \
        create_name(name, suffix).c_str(),                                                                         \
        [](K n, K order, const series_result<T>& data, shanks::remainders::remainder_type remainder,               \
           bool useRecurrentFormula,                                                                               \
           RealT beta) { return shanks::algos::cls<T, K>(remainder, useRecurrentFormula, beta)(n, order, data); }, \
        py::arg("n"), py::arg("order"), py::arg("data"),                                                           \
        py::arg("remainder") = shanks::remainders::remainder_type::u_type, py::arg("useRecurrentFormula") = false, \
        py::arg("beta") = RealT(1.0))

#define BIND_F_BASE(cls, name)                                                                         \
    m.def(                                                                                             \
        create_name(name, suffix).c_str(),                                                             \
        [](K n, K order, const series_result<T>& data, shanks::remainders::remainder_type remainder) { \
            return shanks::algos::cls<T, K>(remainder)(n, order, data);                                \
        },                                                                                             \
        py::arg("n"), py::arg("order"), py::arg("data"),                                               \
        py::arg("remainder") = shanks::remainders::remainder_type::u_type)

#define BIND_PJ_BASE(cls, name)                                                                             \
    m.def(                                                                                                  \
        create_name(name, suffix).c_str(),                                                                  \
        [](K n, K order, const series_result<T>& data, shanks::remainders::remainder_type remainder, int p, \
           RealT beta) { return shanks::algos::cls<T, K>(remainder, p, beta)(n, order, data); },            \
        py::arg("n"), py::arg("order"), py::arg("data"),                                                    \
        py::arg("remainder") = shanks::remainders::remainder_type::u_type, py::arg("p") = 2,                \
        py::arg("beta") = RealT(1.0))

#define BIND_RHO(cls, name)                                                                                       \
    m.def(                                                                                                        \
        create_name(name, suffix).c_str(),                                                                        \
        [](K n, K order, const series_result<T>& data, shanks::numerators::numerator_type numerator, RealT gamma, \
           RealT RHO) { return shanks::algos::cls<T, K>(numerator, gamma, RHO)(n, order, data); },                \
        py::arg("n"), py::arg("order"), py::arg("data"),                                                          \
        py::arg("numerator") = shanks::numerators::numerator_type::rho_type, py::arg("gamma") = RealT(-1.0),      \
        py::arg("RHO") = RealT(1.0))

#define BIND_NONE(cls, name)

#define TRANSFORMATION_ENTRY(id, name, camel, cls, binding, ...) binding(cls, camel);

#include "../../core/include/transformation_registry.def"

#undef TRANSFORMATION_ENTRY
#undef BIND_DEFAULT
#undef BIND_REM
#undef BIND_REM_BOOL
#undef BIND_LEVIN_BASE
#undef BIND_RHO
#undef BIND_NONE
}

template <AcceptedLike T>
void bind_noise(pybind11::module_& m, const char* suffix) {
    m.def(
        create_name("applyNoise", suffix).c_str(),
        [](const series_result<T>& result, shanks::NoiseMethod method, shanks::NoiseType type,
           unsigned long long int seed, const typename GetUnderlyingType<T>::value& p1,
           const GetUnderlyingType<T>::value& p2) {
            unsigned long long int actual_seed = (seed == 0) ? pseudo_random_seed : seed;
            switch (type) {
                case shanks::uniform:
                    return shanks::apply_uniform_noise(result, method, seed,
                                                       utils::cast<double, typename GetUnderlyingType<T>::value>()(p1),
                                                       utils::cast<double, typename GetUnderlyingType<T>::value>()(p2));
                case shanks::normal:
                    return shanks::apply_normal_noise(result, method, seed,
                                                      utils::cast<double, typename GetUnderlyingType<T>::value>()(p1),
                                                      utils::cast<double, typename GetUnderlyingType<T>::value>()(p2));
                case shanks::poisson:
                    return shanks::apply_poisson_noise(result, method, seed,
                                                       utils::cast<double, typename GetUnderlyingType<T>::value>()(p1));
                default:
                    throw std::invalid_argument("Invalid noise type");
            }
        },
        py::arg("result"), py::arg("method"), py::arg("type"), py::arg("seed") = 0, py::arg("param1"),
        py::arg("param2") = T());
}

template <AcceptedLike T>
void bind_filters(pybind11::module_& m, const char* suffix) {
    m.def(create_name("kolmogorovZurbenkoFilter", suffix).c_str(), &shanks::filters::kolmogorov_zurbenko_filter<T>,
          py::arg("result"), py::arg("windowLength"), py::arg("degree"));

    m.def(create_name("savitzkyGolayFilter", suffix).c_str(), &shanks::filters::savitzky_golay_filter<T>,
          py::arg("result"), py::arg("windowLength"), py::arg("polyorder"), py::arg("derive"), py::arg("delta"));
}

#endif
