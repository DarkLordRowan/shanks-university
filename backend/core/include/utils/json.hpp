#ifndef SHANKS_UTILS_JSON_HPP
#define SHANKS_UTILS_JSON_HPP

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <typeinfo>

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

// ---------------------------------------------------------------------------
// Safe numeric parsers — NO silent fallback. Throw on partial/failed parse.
// ---------------------------------------------------------------------------

/// Parse a floating-point value from a string, requiring full consumption.
/// Unlike std::stod which silently parses a prefix, this throws if the entire
/// string is not consumed.
inline double parse_double(const std::string& s) {
    size_t pos = 0;
    double val = std::stod(s, &pos);
    if (pos != s.size()) {
        throw std::runtime_error("parse_double: trailing junk in '" + s + "' (consumed " + std::to_string(pos) + " of " + std::to_string(s.size()) + " chars)");
    }
    return val;
}

/// Parse an unsigned long from a string, requiring full consumption.
inline unsigned long parse_ulong(const std::string& s) {
    size_t pos = 0;
    unsigned long val = std::stoul(s, &pos);
    if (pos != s.size()) {
        throw std::runtime_error("parse_ulong: trailing junk in '" + s + "' (consumed " + std::to_string(pos) + " of " + std::to_string(s.size()) + " chars)");
    }
    return val;
}

/// Parse an unsigned long long from a string, requiring full consumption.
inline unsigned long long parse_ulonglong(const std::string& s) {
    size_t pos = 0;
    unsigned long long val = std::stoull(s, &pos);
    if (pos != s.size()) {
        throw std::runtime_error("parse_ulonglong: trailing junk in '" + s + "' (consumed " + std::to_string(pos) + " of " + std::to_string(s.size()) + " chars)");
    }
    return val;
}

/// Parse a typed value from a string using istringstream, requiring full consumption.
/// Works for double, float, complex<T>, interval<T>, etc. — anything with operator>>.
/// Throws if the stream fails or if trailing characters remain.
template <typename T>
T parse_istream(const std::string& s) {
    std::istringstream iss(s);
    T value;
    iss >> value;
    if (iss.fail()) {
        throw std::runtime_error("parse_istream<" + std::string(typeid(T).name()) + ">: failed to parse '" + s + "'");
    }
    // Check that the entire input was consumed (skip trailing whitespace)
    char c;
    while (iss.get(c)) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            throw std::runtime_error("parse_istream<" + std::string(typeid(T).name()) + ">: trailing junk in '" + s + "'");
        }
    }
    return value;
}

}  // namespace shanks::utils_json

#endif  // SHANKS_UTILS_JSON_HPP
