/**
 * @file registry_ffi.cpp
 * @brief Implementation of registry query functions using existing metadata.
 * @authors Sobolev Y. A.
 */

#include "shanks_ffi.hpp"
#include "../../core/include/lib.hpp"

#include <cstring>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

// Helper to allocate and copy string for FFI return
char* alloc_string(const std::string& s) {
    if (s.empty()) {
        char* ptr = new char[1];
        ptr[0] = '\0';
        return ptr;
    }
    char* ptr = new char[s.size() + 1];
    std::memcpy(ptr, s.c_str(), s.size() + 1);
    return ptr;
}

// Escape JSON string
std::string escape_json_string(const std::string& s) {
    std::ostringstream oss;
    for (char c : s) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

// Convert vector to JSON array
std::string vector_to_json_array(const std::vector<std::string>& vec) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << escape_json_string(vec[i]) << "\"";
    }
    oss << "]";
    return oss.str();
}

// Precision types
const std::vector<std::string> g_precision_names = {
    "F32", "F64", "FLong", "Arb",
    "CF32", "CF64", "CFLong", "CArb",
    "IntervalF32", "IntervalF64", "IntervalFLong", "IntervalArb",
    "CIntervalF32", "CIntervalF64", "CIntervalFLong", "CIntervalArb"
};

// Noise types (from noise_generator.hpp)
const std::vector<std::string> g_noise_types = {
    "Normal",
    "Uniform",
    "Poisson",
};

// Noise methods (from noise_generator.hpp)
const std::vector<std::string> g_noise_methods = {
    "jitter",
    "scaling",
};

// Series with extra parameters (from series_registry.def SERIES_ENTRY_ARGS)
const std::unordered_map<std::string, std::vector<std::string>> g_series_params = {
    {"BinSeries", {"alpha"}},
    {"IncompleteGammaFuncSeries", {"alpha"}},
    {"MFact1mxMp1InverseSeries", {"m"}},
};

// Build parameter metadata JSON for a series
std::string build_series_params_json(const std::string& name) {
    std::ostringstream oss;
    oss << "[";
    
    auto it = g_series_params.find(name);
    if (it != g_series_params.end()) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "{\"name\": \"" << it->second[i] << "\", \"type\": \"float\", \"required\": true}";
        }
    }
    
    oss << "]";
    return oss.str();
}

// Build parameter metadata JSON for an algorithm based on binding strategy
// This is a simplified version - the actual binding info is in transformation_registry.def
std::string build_accel_params_json(const std::string& name) {
    std::ostringstream oss;
    oss << "[";
    
    // Known algorithms with parameters
    std::vector<std::string> params;
    
    if (name.find("Levin") != std::string::npos || 
        name.find("Drummond") != std::string::npos) {
        params.push_back("{\"name\": \"remainder_type\", \"type\": \"enum\", \"values\": [\"u\", \"t\", \"v\", \"t_wave\", \"v_wave\"], \"default\": \"u\"}");
        params.push_back("{\"name\": \"recurrent\", \"type\": \"bool\", \"default\": false}");
    }
    if (name.find("Rho") != std::string::npos) {
        params.push_back("{\"name\": \"numerator_type\", \"type\": \"enum\", \"values\": [\"rho\", \"generalized\", \"gamma_rho\"], \"default\": \"rho\"}");
    }
    
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << params[i];
    }
    
    oss << "]";
    return oss.str();
}

} // anonymous namespace

// ============================================================================
// Registry Queries Implementation
// ============================================================================

extern "C" SHANKS_FFI_API char* shanks_list_series(void) {
    // Use existing metadata from core library (uses X-macros internally)
    auto names = shanks::series::series_registry_metadata::get_names();
    return alloc_string(vector_to_json_array(names));
}

extern "C" SHANKS_FFI_API char* shanks_list_accels(void) {
    // Use existing metadata from core library (uses X-macros internally)
    auto names = shanks::algos::transformation_registry_metadata::get_names();
    return alloc_string(vector_to_json_array(names));
}

extern "C" SHANKS_FFI_API char* shanks_list_precisions(void) {
    return alloc_string(vector_to_json_array(g_precision_names));
}

extern "C" SHANKS_FFI_API char* shanks_list_noises(void) {
    return alloc_string(vector_to_json_array(g_noise_types));
}

extern "C" SHANKS_FFI_API char* shanks_list_noise_methods(void) {
    return alloc_string(vector_to_json_array(g_noise_methods));
}

extern "C" SHANKS_FFI_API char* shanks_get_noise_info(const char* name) {
    if (!name) return nullptr;
    
    std::ostringstream oss;
    oss << "{";
    oss << "\"name\": \"" << escape_json_string(name) << "\"";
    
    // Parameter info based on noise type
    if (strcmp(name, "Normal") == 0) {
        oss << ", \"params\": ["
            << "{\"name\": \"mean\", \"type\": \"float\", \"default\": 0.0}, "
            << "{\"name\": \"stddev\", \"type\": \"float\", \"default\": 0.1}"
            << "]";
    } else if (strcmp(name, "Uniform") == 0) {
        oss << ", \"params\": ["
            << "{\"name\": \"min\", \"type\": \"float\", \"default\": 0.9}, "
            << "{\"name\": \"max\", \"type\": \"float\", \"default\": 1.1}"
            << "]";
    } else if (strcmp(name, "Poisson") == 0) {
        oss << ", \"params\": ["
            << "{\"name\": \"mean\", \"type\": \"float\", \"default\": 1.0}"
            << "]";
    } else {
        oss << ", \"params\": []";
    }
    
    oss << ", \"methods\": [\"jitter\", \"scaling\"]";
    oss << "}";
    return alloc_string(oss.str());
}

extern "C" SHANKS_FFI_API char* shanks_get_series_info(const char* name) {
    if (!name) return nullptr;
    
    // Get all names to verify this series exists
    auto names = shanks::series::series_registry_metadata::get_names();
    bool found = false;
    for (const auto& n : names) {
        if (n == name) {
            found = true;
            break;
        }
    }
    
    std::ostringstream oss;
    oss << "{";
    oss << "\"name\": \"" << escape_json_string(name) << "\"";
    oss << ", \"params\": " << build_series_params_json(name);
    oss << ", \"description\": \"\"";
    oss << "}";
    return alloc_string(oss.str());
}

extern "C" SHANKS_FFI_API char* shanks_get_accel_info(const char* name) {
    if (!name) return nullptr;
    
    // Get all names to verify this algorithm exists
    auto names = shanks::algos::transformation_registry_metadata::get_names();
    bool found = false;
    for (const auto& n : names) {
        if (n == name) {
            found = true;
            break;
        }
    }
    
    std::ostringstream oss;
    oss << "{";
    oss << "\"name\": \"" << escape_json_string(name) << "\"";
    oss << ", \"params\": " << build_accel_params_json(name);
    oss << ", \"description\": \"\"";
    oss << "}";
    return alloc_string(oss.str());
}
