/**
 * @file shanks_ffi.hpp
 * @brief C-compatible FFI interface for the Shanks library.
 * @authors Sobolev Y. A.
 * 
 * This header provides a C-compatible interface for calling the Shanks library
 * from Rust (or any other language with FFI support). It uses opaque handles
 * and JSON strings for data exchange.
 * 
 * All strings returned by functions are allocated by the library and must be
 * freed using shanks_free_string().
 */

#ifndef SHANKS_FFI_HPP
#define SHANKS_FFI_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// ============================================================================
// Platform-specific export macros
// ============================================================================

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef SHANKS_FFI_EXPORTS
        #define SHANKS_FFI_API __declspec(dllexport)
    #else
        #define SHANKS_FFI_API __declspec(dllimport)
    #endif
#else
    #define SHANKS_FFI_API __attribute__((visibility("default")))
#endif

// ============================================================================
// Opaque Handles
// ============================================================================

/**
 * @brief Opaque handle to a series instance.
 * 
 * Series are created with specific parameters and can be used to generate
 * partial sums and terms.
 */
typedef void* ShanksSeriesHandle;

/**
 * @brief Opaque handle to an acceleration algorithm instance.
 * 
 * Acceleration algorithms transform series data to improve convergence.
 */
typedef void* ShanksAccelHandle;

// ============================================================================
// Error Handling
// ============================================================================

/**
 * @brief Get the last error message.
 * 
 * Returns a thread-local error message from the last failed operation.
 * The string is valid until the next FFI call in the same thread.
 * 
 * @return Error message string, or NULL if no error.
 */
SHANKS_FFI_API const char* shanks_last_error(void);

/**
 * @brief Clear the last error message.
 */
SHANKS_FFI_API void shanks_clear_error(void);

// ============================================================================
// Library Management
// ============================================================================

/**
 * @brief Get library version string.
 * 
 * @return Version string (e.g., "1.0.0"). Does not need to be freed.
 */
SHANKS_FFI_API const char* shanks_get_version(void);

// ============================================================================
// Memory Management
// ============================================================================

/**
 * @brief Free a string allocated by the library.
 * 
 * Use this to free strings returned by functions like shanks_list_series(),
 * shanks_series_generate(), etc.
 * 
 * @param ptr Pointer to string to free. Safe to pass NULL.
 */
SHANKS_FFI_API void shanks_free_string(char* ptr);

// ============================================================================
// Registry Queries
// ============================================================================

/**
 * @brief Get list of available series names.
 * 
 * @return JSON array of series names: ["SeriesName1", "SeriesName2", ...]
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_list_series(void);

/**
 * @brief Get list of available acceleration algorithm names.
 * 
 * @return JSON array of algorithm names: ["shanks", "levin", ...]
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_list_accels(void);

/**
 * @brief Get list of supported precision types.
 * 
 * @return JSON array of precision names: ["F32", "F64", "FLong", "Arb", "CF32", ...]
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_list_precisions(void);

/**
 * @brief Get list of available noise types.
 * 
 * @return JSON array of noise type names: ["Normal", "Uniform", "Poisson"]
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_list_noises(void);

/**
 * @brief Get list of available noise application methods.
 * 
 * @return JSON array of method names: ["jitter", "scaling"]
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_list_noise_methods(void);

/**
 * @brief Get noise type metadata.
 * 
 * @param name Noise type name ("Normal", "Uniform", "Poisson").
 * @return JSON object with noise metadata including parameters, or NULL if not found.
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_get_noise_info(const char* name);

/**
 * @brief Get series metadata (parameters, description).
 * 
 * @param name Series name.
 * @return JSON object with series metadata, or NULL if not found.
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_get_series_info(const char* name);

/**
 * @brief Get acceleration algorithm metadata.
 * 
 * @param name Algorithm name.
 * @return JSON object with algorithm metadata, or NULL if not found.
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_get_accel_info(const char* name);

// ============================================================================
// Series Creation and Destruction
// ============================================================================

/**
 * @brief Create a series instance.
 * 
 * @param name Series name from registry (e.g., "PiSeries", "ArcsinXSeries").
 * @param precision Precision type: "F32", "F64", "FLong", "Arb", "CF32", etc.
 * @param x_value The x parameter as a string (for arbitrary precision support).
 * @param args_json JSON object with additional arguments: {"tParam": "1.0", "kParam": "10"}.
 *                  Pass NULL or "{}" for default values.
 * @return Handle to series instance, or NULL on error.
 */
SHANKS_FFI_API ShanksSeriesHandle shanks_series_create(
    const char* name,
    const char* precision,
    const char* x_value,
    const char* args_json
);

SHANKS_FFI_API ShanksSeriesHandle shanks_series_create_with_noise(
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

/**
 * @brief Destroy a series instance.
 * 
 * @param handle Series handle to destroy. Safe to pass NULL.
 */
SHANKS_FFI_API void shanks_series_destroy(ShanksSeriesHandle handle);

/**
 * @brief Get the analytical sum (limit) of the series, if known.
 * 
 * @param handle Series handle.
 * @return The sum as a string (for arbitrary precision), or NULL if unknown.
 *         Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_series_get_sum(ShanksSeriesHandle handle);

/**
 * @brief Get the x value of the series.
 * 
 * @param handle Series handle.
 * @return The x value as a string. Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_series_get_x(ShanksSeriesHandle handle);

// ============================================================================
// Data Structures for Binary FFI Transfer
// ============================================================================

/**
 * @brief Represents a single sequence of scientific notation numbers.
 */
typedef struct {
    double* mantissas;
    int64_t* exponents;
    uint64_t len;
} FFILine;

/**
 * @brief Represents a collection of lines (real, complex, interval, or complex-interval).
 */
typedef struct {
    uint32_t type; // 0=Real, 1=Complex, 2=Interval, 3=CInterval
    FFILine lines[4]; 
} FFILineColl;

/**
 * @brief Result from generating a series.
 */
typedef struct {
    FFILineColl sn;
    FFILineColl an;
    uint32_t has_sum;  // 1 if sum exists, 0 otherwise
    uint32_t sum_type; // same enum as FFILineColl.type
    double sum_m[4];   // up to 4 mantissas for the sum point
    int64_t sum_e[4];  // up to 4 exponents for the sum point
    FFILineColl deviations; // same type layout as sn, usually 1 real line
} FFISeriesResult;

/**
 * @brief Result from applying an acceleration algorithm.
 */
typedef struct {
    FFILineColl values;
    FFILine deviations; // Always Real (double)
} FFIAccelResult;

// ============================================================================
// Series Computation
// ============================================================================

/**
 * @brief Generate partial sums and terms for a series.
 * 
 * @param handle Series handle.
 * @param n Number of terms to generate.
 * @return Pointer to a dynamically allocated FFISeriesResult.
 *         Must be freed with shanks_series_result_free().
 */
SHANKS_FFI_API FFISeriesResult* shanks_series_generate(
    ShanksSeriesHandle handle,
    uint64_t n
);

/**
 * @brief Free an FFISeriesResult structure allocated by the library.
 */
SHANKS_FFI_API void shanks_series_result_free(FFISeriesResult* result);

// ============================================================================
// Acceleration Algorithm Creation and Destruction
// ============================================================================

/**
 * @brief Create an acceleration algorithm instance.
 * 
 * @param name Algorithm name from registry (e.g., "shanks", "levin", "wynn_epsilon").
 * @param precision Precision type (must match series precision).
 * @param args_json Algorithm parameters: {"m": "10", "remainder": "u", "beta": "1.0"}.
 *                  Pass NULL or "{}" for default values.
 * @return Handle to algorithm instance, or NULL on error.
 */
SHANKS_FFI_API ShanksAccelHandle shanks_accel_create(
    const char* name,
    const char* precision,
    const char* args_json
);

/**
 * @brief Destroy an acceleration algorithm instance.
 * 
 * @param handle Algorithm handle to destroy. Safe to pass NULL.
 */
SHANKS_FFI_API void shanks_accel_destroy(ShanksAccelHandle handle);

// ============================================================================
// Acceleration Computation
// ============================================================================

/**
 * @brief Apply acceleration algorithm to series data.
 * 
 * @param accel Algorithm handle.
 * @param series Series handle (must have same precision).
 * @param n Number of terms to use from series.
 * @param order Order parameter for the algorithm.
 * @return Pointer to dynamically allocated FFIAccelResult.
 *         Must be freed with shanks_accel_result_free().
 */
SHANKS_FFI_API FFIAccelResult* shanks_accel_apply(
    ShanksAccelHandle accel,
    ShanksSeriesHandle series,
    uint64_t n,
    uint64_t order
);

/**
 * @brief Free an FFIAccelResult structure allocated by the library.
 */
SHANKS_FFI_API void shanks_accel_result_free(FFIAccelResult* result);

// ============================================================================
// Post-Processing
// ============================================================================

/**
 * @brief Compute a smoothed limit from a divergent tail using Kolmogorov-Zurbenko filter.
 * 
 * @param precision Precision type (e.g., "F64", "Arb").
 * @param values Array of string values representing the tail.
 * @param len Number of values in the array.
 * @param filter_type Filter type string (e.g. "kolmogorovZurbenko").
 * @param args_json JSON string of arguments.
 * @return Smoothed limit as a string. Must be freed with shanks_free_string().
 */
SHANKS_FFI_API char* shanks_compute_smoothed_limit(
    const char* precision,
    const char** values,
    uint64_t len,
    const char* filter_type,
    const char* args_json
);

#ifdef __cplusplus
}
#endif

#endif // SHANKS_FFI_HPP
