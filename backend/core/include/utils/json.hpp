#ifndef SHANKS_UTILS_JSON_HPP
#define SHANKS_UTILS_JSON_HPP

#include <cctype>
#include <string>

namespace shanks::utils_json {

/**
 * @brief Simple JSON value extractor for basic parameter parsing.
 */
inline std::string get_json_val(const std::string& json, const std::string& key) {
    auto pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return "";
    pos = json.find(":", pos);
    if (pos == std::string::npos) return "";
    pos++;

    // Skip spaces, tabs, and potential opening quote
    while (pos < json.size() && (std::isspace(json[pos]) || json[pos] == '\"')) pos++;

    auto end = pos;
    // Values end at comma, closing brace, or closing quote
    while (end < json.size() && json[end] != ',' && json[end] != '}' && json[end] != '\"') end++;

    return json.substr(pos, end - pos);
}

inline std::string get_json_val_required(const std::string& json, const std::string& key) {
    auto val = get_json_val(json, key);
    if (val.empty()) {
        throw std::runtime_error("Missing required JSON parameter: " + key);
    }
    return val;
}

inline bool parse_bool(const std::string& json, const std::string& key) {
    auto val = get_json_val_required(json, key);
    if (val == "true") return true;
    if (val == "false") return false;
    throw std::runtime_error("Invalid boolean value for " + key + ": " + val);
}

}  // namespace shanks::utils_json

#endif  // SHANKS_UTILS_JSON_HPP
