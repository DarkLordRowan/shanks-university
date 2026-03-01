#ifndef SHANKS_UTILS_JSON_HPP
#define SHANKS_UTILS_JSON_HPP

#include <string>
#include <cctype>

namespace shanks::utils {

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

} // namespace shanks::utils

#endif // SHANKS_UTILS_JSON_HPP
