#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <math.h>

#include "libs/arbitrary_arithmetics/fprecision.h"
#include "libs/arbitrary_arithmetics/precisioncore.cpp"

#include "methods.hpp"
#include "series.hpp"

PYBIND11_MODULE(pyshanks, m) {

    m.doc() = "pybind11: polymorphic series + all Shanks‑type transformations (double + float_precision";

    py::enum_<remainder_type>(m, "RemainderType")
        .value("u_variant", remainder_type::u_variant)
        .value("t_variant", remainder_type::t_variant)
        .value("v_variant", remainder_type::v_variant)
        .value("t_wave_variant", remainder_type::t_wave_variant)
        .value("v_wave_variant", remainder_type::v_wave_variant)
        .export_values();
    
    py::enum_<numerator_type>(m, "NumeratorType")
        .value("rho_variant", numerator_type::rho_variant)
        .value("generalized_variant", numerator_type::generalized_variant)
        .value("gamma_rho_variant", numerator_type::gamma_rho_variant)
        .export_values();

    using T = double;
    using K = unsigned long long int;

    py::class_<SeriesResult<T>>(m, "SeriesResult")
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readwrite("Sn", &SeriesResult<T>::Sn)
        .def_readwrite("an", &SeriesResult<T>::an);

    py::class_<series_base<T, K>>(m, "SeriesBase")
        .def("get_x", &series_base<T,K>::get_x)
        .def("get_sum", &series_base<T,K>::get_sum)
        .def("generate_series", &series_base<T,K>::generateSeries, py::arg("size"));

    py::class_<exp_series<T,K>, series_base<T,K>>(m, "ExpSeries")
        .def(py::init<T>(), py::arg("x") = static_cast<T>(1));

    py::class_<series_acceleration<T,K>>(m, "SeriesAcceleration")
        .def("printInfo", &series_acceleration<T, K>::print_info)
        .def("__call__", &series_acceleration<T,K>::operator(), py::arg("n"), py::arg("order"), py::arg("data"), py::arg("offset") = static_cast<K>(0));

    //init algos
    py::class_<brezinski_theta_algorithm<T,K>, series_acceleration<T,K>>(m, "BrezinskiThetaAlgorithm")
        .def(py::init<>());

    py::class_<chang_wynn_algorithm<T,K>, series_acceleration<T,K>>(m, "ChangWynnAlgorithm")
        .def(py::init<>());
    
    py::class_<drummond_d_algorithm<T,K>, series_acceleration<T,K>>(m, "DrummondDAlgorithm")
        .def(py::init<remainder_type, bool>(), py::arg("remainder") = remainder_type::t_variant, py::arg("useRecurrentFormula") = false);

    py::class_<ford_sidi_2_algorithm<T,K>, series_acceleration<T,K>>(m, "FordSidi2Algorithm")
        .def(py::init<>());

    py::class_<ford_sidi_3_algorithm<T,K>, series_acceleration<T,K>>(m, "FordSidi3Algorithm")
        .def(py::init<>());

    py::class_<levin_algorithm<T,K>, series_acceleration<T,K>>(m, "LevinLAlgorithm")
        .def(py::init<remainder_type, bool, T>(), py::arg("remainder") = remainder_type::t_variant, py::arg("useRecurrentFormula") = false, py::arg("beta") = static_cast<T>(1)); //ADD
    
    py::class_<levin_sidi_m_algorithm<T,K>, series_acceleration<T,K>>(m, "LevinSidiMAlgorithm")
        .def(py::init<remainder_type, T>(), py::arg("remainder") = remainder_type::t_variant, py::arg("gamma") = static_cast<T>(10)); //ADD

    py::class_<levin_sidi_s_algorithm<T,K>, series_acceleration<T,K>>(m, "LevinSidiSAlgorithm")
        .def(py::init<remainder_type, bool, T>(), py::arg("remainder") = remainder_type::t_variant, py::arg("useRecurrentFormula") = false, py::arg("beta") = static_cast<T>(1)); //ADD

    py::class_<lubkin_w_algorithm<T,K>, series_acceleration<T,K>>(m, "LubkinWAlgorithm")
        .def(py::init<>());

    py::class_<richardson_algorithm<T,K>, series_acceleration<T,K>>(m, "RichardsonAlgorithm")
        .def(py::init<>());

    py::class_<shanks_algorithm<T,K>, series_acceleration<T,K>>(m, "ShanksAlgorithm")
        .def(py::init<>());

    py::class_<shanks_transform_alternating<T,K>, series_acceleration<T,K>>(m, "AlternatingShanksAlgorithm")
        .def(py::init<>());

    py::class_<weniger_algorithm<T,K>, series_acceleration<T,K>>(m, "WenigerAlgorithm")
        .def(py::init<>());

    py::class_<wynn_epsilon_1_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnEpsilon1Algorithm")
        .def(py::init<>());

    py::class_<wynn_epsilon_2_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnEpsilon2Algorithm")
        .def(py::init<>());

    py::class_<wynn_epsilon_3_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnEpsilon3Algorithm")
        .def(py::init<T>(), py::arg("epsilon_threshold") = static_cast<T>(1e-3));

    py::class_<wynn_rho_algorithm<T,K>, series_acceleration<T,K>>(m, "WynnRhoAlgorithm")
        .def(py::init<numerator_type, T, T>(), py::arg("numerator") = remainder_type::t_variant, py::arg("gamma") = static_cast<T>(-1), py::arg("rho") = static_cast<T>(1));


}