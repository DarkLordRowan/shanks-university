#pragma once

#include "rust/cxx.h"
#include <memory>
#include <string>


namespace shanks::ffi {
enum class PrecisionType;
}

namespace shanks::ffi::bridge {
void shanks_force_link_gslcblas();
// Forward declarations of cxx-generated structs
struct RawArr;
struct RawValue;

/**
 * @brief Opaque handle for a series implementation.
 * Rust holds a UniquePtr<CSeries>. The actual implementation resides in bridge.cpp.
 */
class CSeries {
public:
    virtual ~CSeries() = default;

    // Methods that must be implemented by the template specializations
    virtual shanks::ffi::PrecisionType precision_type() const = 0;
    virtual const void* raw_result() const = 0;

    virtual RawArr get_sn() const = 0;
    virtual RawArr get_an() const = 0;
    virtual RawArr get_deviation() const = 0;
    virtual RawValue get_limit() const = 0;

    virtual std::unique_ptr<CSeries> apply_noise(rust::Str name, rust::Str params_json, uint64_t start_n) const = 0;
    virtual std::unique_ptr<CSeries> run_algo(rust::Str name, rust::Str params_json, size_t m, rust::Slice<const int32_t> n) const = 0;
    virtual RawArr filter(rust::Str name, rust::Str params_json, uint64_t start_n) const = 0;
};

// Meta info
rust::Vec<rust::String> list_series();
rust::Vec<rust::String> list_accels();
rust::Vec<rust::String> list_precisions();
rust::Vec<rust::String> list_noises();
rust::Vec<rust::String> list_noise_methods();

// Factory functions
std::unique_ptr<CSeries> mk_series(rust::Str name, rust::Str precision, rust::Str params_json, size_t n, rust::Str x);
std::unique_ptr<CSeries> apply_noise(const CSeries& series, rust::Str name, rust::Str params_json, uint64_t start_n);
std::unique_ptr<CSeries> run_algo(const CSeries& series, rust::Str name, rust::Str params_json, size_t m, rust::Slice<const int32_t> n);

// Getters 
RawArr get_sn(const CSeries& series);
RawArr get_an(const CSeries& series);
RawArr get_deviation(const CSeries& series);
RawValue get_limit(const CSeries& series);

RawArr filter(const CSeries& series, rust::Str name, rust::Str params_json, uint64_t start_n);

}
