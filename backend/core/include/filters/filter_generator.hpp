#ifndef SHANKS_FILTERS_FILTER_GENERATOR_HPP
#define SHANKS_FILTERS_FILTER_GENERATOR_HPP

#include "kolmogorov_zurbenko.hpp"
#include "savitzky_golay.hpp"
#include "utils/json.hpp"
#include <string>
#include <vector>

namespace shanks::filters {

/**
 * @brief High-level entry point for applying filters using a JSON parameter string.
 */
template <AcceptedLike T>
std::vector<T> apply_filter(const std::vector<T>& data, const std::string& name,
                            const std::string& params_json, uint64_t start_n) {
    std::vector<T> tail_data = data;
    if (start_n > 0 && start_n < data.size()) {
        tail_data.erase(tail_data.begin(), tail_data.begin() + start_n);
    }

    if (name == "kz" || name == "Kolmogorov-Zurbenko") {
        size_t m = std::stoul(::shanks::utils::get_json_val(params_json, "window_length"));
        size_t k = std::stoul(::shanks::utils::get_json_val(params_json, "degree"));
        return kolmogorov_zurbenko_filter<T>(tail_data, m, k);
    } else if (name == "sg" || name == "Savitzky-Golay") {
        size_t wl = std::stoul(::shanks::utils::get_json_val(params_json, "window_length"));
        size_t po = std::stoul(::shanks::utils::get_json_val(params_json, "polyorder"));
        size_t der = std::stoul(::shanks::utils::get_json_val(params_json, "derive"));
        double delta = std::stod(::shanks::utils::get_json_val(params_json, "delta"));

        if constexpr (!is_interval_v<T> && !is_complex_interval_v<T>) {
            return savitzky_golay_filter<T>(tail_data, wl, po, der, T(delta));
        } else {
            return tail_data;
        }
    }
    return tail_data;
}

} // namespace shanks::filters

#endif // SHANKS_FILTERS_FILTER_GENERATOR_HPP
