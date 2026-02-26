/**
 * @file shanks_ffi.cpp
 * @brief Implementation of core FFI functions (error handling, memory management).
 * @authors Sobolev Y. A.
 */

#include "shanks_ffi.hpp"
#include "ffi_internal.hpp"
#include <cstring>
#include <mutex>
#include <sstream>
#include <string>

namespace {


// Helper to allocate and copy string for FFI return
char* alloc_string(const std::string& s) {
    if (s.empty()) {
        return nullptr;
    }
    char* ptr = new char[s.size() + 1];
    std::memcpy(ptr, s.c_str(), s.size() + 1);
    return ptr;
}

// Helper to convert value to scientific notation JSON
template <typename T>
std::string value_to_json(const T& value);

// Helper to parse precision string
int parse_precision(const char* precision) {
    if (!precision) return -1;
    
    if (strcmp(precision, "F32") == 0) return 0;
    if (strcmp(precision, "F64") == 0) return 1;
    if (strcmp(precision, "FLong") == 0) return 2;
    if (strcmp(precision, "Arb") == 0) return 3;
    if (strcmp(precision, "CF32") == 0) return 4;
    if (strcmp(precision, "CF64") == 0) return 5;
    if (strcmp(precision, "CFLong") == 0) return 6;
    if (strcmp(precision, "CArb") == 0) return 7;
    
    return -1;
}

bool is_complex_precision(const char* precision) {
    if (!precision) return false;
    return strncmp(precision, "C", 1) == 0;
}

} // anonymous namespace

// ============================================================================
// Error Handling
// ============================================================================

extern "C" SHANKS_FFI_API const char* shanks_last_error(void) {
    return shanks::ffi::g_last_error.empty() ? nullptr : shanks::ffi::g_last_error.c_str();
}

extern "C" SHANKS_FFI_API void shanks_clear_error(void) {
    shanks::ffi::clear_error();
}

// ============================================================================
// Library Management
// ============================================================================

extern "C" SHANKS_FFI_API const char* shanks_get_version(void) {
    return "0.1.0";
}

// ============================================================================
// Memory Management
// ============================================================================

extern "C" SHANKS_FFI_API void shanks_free_string(char* ptr) {
    if (ptr) {
        delete[] ptr;
    }
}

// ============================================================================
// Registry Queries (stubs - implemented in registry_ffi.cpp)
// ============================================================================

// These are declared here as weak symbols so they can be overridden
extern "C" SHANKS_FFI_API char* shanks_list_series(void);
extern "C" SHANKS_FFI_API char* shanks_list_accels(void);
extern "C" SHANKS_FFI_API char* shanks_list_precisions(void);
extern "C" SHANKS_FFI_API char* shanks_get_series_info(const char* name);
extern "C" SHANKS_FFI_API char* shanks_get_accel_info(const char* name);

// ============================================================================
// Series Creation (stubs - implemented in series_ffi.cpp)
// ============================================================================

extern "C" SHANKS_FFI_API ShanksSeriesHandle shanks_series_create(
    const char* name,
    const char* precision,
    const char* x_value,
    const char* args_json
);

extern "C" SHANKS_FFI_API ShanksSeriesHandle shanks_series_create_with_noise(
    const char* name,
    const char* precision,
    const char* x_value,
    const char* args_json,
    const char* noise_type,
    const char* noise_method,
    double param1,
    double param2,
    uint64_t seed
);

extern "C" SHANKS_FFI_API void shanks_series_destroy(ShanksSeriesHandle handle);

extern "C" SHANKS_FFI_API char* shanks_series_get_sum(ShanksSeriesHandle handle);

extern "C" SHANKS_FFI_API char* shanks_series_get_x(ShanksSeriesHandle handle);

// Removes duplicate declaration

// ============================================================================
// Acceleration (stubs - implemented in accel_ffi.cpp)
// ============================================================================

extern "C" SHANKS_FFI_API ShanksAccelHandle shanks_accel_create(
    const char* name,
    const char* precision,
    const char* args_json
);

extern "C" SHANKS_FFI_API void shanks_accel_destroy(ShanksAccelHandle handle);

// Removes duplicate declaration

extern "C" SHANKS_FFI_API char* shanks_accel_apply_data(
    ShanksAccelHandle accel,
    const char* series_json,
    uint64_t n,
    uint64_t order
);
