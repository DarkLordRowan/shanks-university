#ifndef SHANKS_FILTERS_FILTER_GENERATOR_HPP
#define SHANKS_FILTERS_FILTER_GENERATOR_HPP

#include <string>
#include <vector>

#include "kolmogorov_zurbenko.hpp"
#include "savitzky_golay.hpp"
#include "utils/json.hpp"

namespace shanks::filters {

/**
 * @brief High-level entry point for applying filters using a JSON parameter string.
 */
template <AcceptedLike T>
std::vector<T> apply_filter(const std::vector<T>& data, const std::string& name, const std::string& params_json,
                            uint64_t start_n) {
    std::vector<T> tail_data = data;
    if (start_n > 0 && start_n < data.size()) {
        tail_data.erase(tail_data.begin(), tail_data.begin() + start_n);
    }

    if (name == "kz" || name == "Kolmogorov-Zurbenko") {
        size_t m = ::shanks::utils_json::parse_ulong(::shanks::utils_json::get_json_val_required(params_json, "window_length"));
        size_t k = ::shanks::utils_json::parse_ulong(::shanks::utils_json::get_json_val_required(params_json, "degree"));
        return kolmogorov_zurbenko_filter<T>(tail_data, m, k);
    } else if (name == "sg" || name == "Savitzky-Golay") {
        size_t wl = ::shanks::utils_json::parse_ulong(::shanks::utils_json::get_json_val_required(params_json, "window_length"));
        size_t po = ::shanks::utils_json::parse_ulong(::shanks::utils_json::get_json_val_required(params_json, "polyorder"));
        size_t der = ::shanks::utils_json::parse_ulong(::shanks::utils_json::get_json_val_required(params_json, "derive"));
        double delta = ::shanks::utils_json::parse_double(::shanks::utils_json::get_json_val_required(params_json, "delta"));

        if constexpr (!is_interval_v<T> && !is_complex_interval_v<T>) {
            return savitzky_golay_filter<T>(tail_data, wl, po, der, T(delta));
        } else {
            return tail_data;
        }
    }
    throw std::runtime_error("Unknown filter: " + name);
}

}  // namespace shanks::filters

#endif  // SHANKS_FILTERS_FILTER_GENERATOR_HPP
