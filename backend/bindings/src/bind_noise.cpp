#include "../../core/include/noise/noise_generator.hpp"
#include "../include/bindings.hpp"

template <AcceptedLike T>
void bind_noise(pybind11::module_& m, const char* suffix)
{
    m.def(
        create_name("applyNoise", suffix).c_str(),
        [](const series_result<T>& result,
           NoiseMethod method,
           NoiseType type,
           unsigned long long int seed,
           const T& p1,
           const T& p2) {
            unsigned long long int actual_seed = (seed == 0) ? pseudo_random_seed : seed;
            return apply_noise(result, method, type, actual_seed, p1, p2);
        },
        py::arg("result"),
        py::arg("method"),
        py::arg("type"),
        py::arg("seed") = 0,
        py::arg("param1"),
        py::arg("param2") = T());
}

void bind_noise_all(py::module_& m)
{
    bind_each_type<types_to_bind>(
        m, suffixes, []<typename T, typename K>(py::module_& mod, const char* s) { bind_noise<T>(mod, s); });
}
