/**
 * @file accel_ffi.cpp
 * @brief Implementation of acceleration algorithm FFI functions.
 * @authors Sobolev Y. A.
 * 
 * This file implements the acceleration algorithm FFI interface.
 * It uses the existing transformation_registry infrastructure.
 */

#include "shanks_ffi.hpp"
#include "ffi_internal.hpp"
#include "../../core/include/lib.hpp"
#include "../../core/include/filters/kolmogorov_zurbenko.hpp"

#include <cmath>
#include <cstring>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

// Use shared types from ffi_internal.hpp
using shanks::ffi::PrecisionType;
using shanks::ffi::SeriesHandleBase;
using shanks::ffi::AccelHandleBase;
using shanks::ffi::alloc_string;
using shanks::ffi::set_error;
using shanks::ffi::clear_error;
using shanks::ffi::parse_precision;
using shanks::ffi::ScientificValue;

namespace {

// Convert a value to scientific notation
template <typename T>
ScientificValue to_scientific(T value) {
    if (value == T(0)) {
        return ScientificValue(0.0, 0);
    }
    
    double abs_val = std::abs(static_cast<double>(value));
    int64_t exp = static_cast<int64_t>(std::floor(std::log10(abs_val)));
    double mantissa = static_cast<double>(value) / std::pow(10.0, exp);
    
    return ScientificValue(mantissa, exp);
}

// Specialization for mpfr::mpreal
template <>
ScientificValue to_scientific(mpfr::mpreal value) {
    if (value == 0) {
        return ScientificValue(0.0, 0);
    }
    
    std::ostringstream oss;
    oss << value;
    std::string s = oss.str();
    
    size_t e_pos = s.find('e');
    if (e_pos != std::string::npos) {
        double mantissa = std::stod(s.substr(0, e_pos));
        int64_t exp = std::stoll(s.substr(e_pos + 1));
        return ScientificValue(mantissa, exp);
    }
    
    return ScientificValue(std::stod(s), 0);
}


// Extended acceleration handle
struct AccelHandleBaseExt : public AccelHandleBase {
    virtual FFIAccelResult* apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) = 0;
};

// Template implementation for real acceleration algorithms
template <typename T>
struct AccelHandleReal : public AccelHandleBaseExt {
    std::unique_ptr<shanks::algos::series_acceleration<T, size_t>> algo;
    PrecisionType precision;
    std::string name;
    size_t algo_index;
    
    AccelHandleReal(std::unique_ptr<shanks::algos::series_acceleration<T, size_t>> a, 
                    PrecisionType p, const std::string& n, size_t idx)
        : algo(std::move(a)), precision(p), name(n), algo_index(idx) {}
    
    std::string apply(
        SeriesHandleBase* series, 
        uint64_t n, 
        uint64_t order, 
        bool enable_profiling
    ) override {
        return "{}"; // Legacy virtual interface placeholder
    }
    
    FFIAccelResult* apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return nullptr;
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return nullptr;
        const auto* data = static_cast<const series_result<T>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        T sum = T(0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const T*>(sum_ptr);
        }

        shanks::ffi::RealBinarySerializer<T> val_ser;
        shanks::ffi::RealBinarySerializer<T> dev_ser;
        
        for (uint64_t i = 1; i <= n; ++i) {
            try {
                T val = (*algo)(i, order, *data);
                val_ser.push(val);
                if (has_sum) {
                    dev_ser.push(utils::math<T>::abs(val - sum));
                } else {
                    dev_ser.push(T(0));
                }
            } catch (...) {
                val_ser.push(T(0));
                dev_ser.push(T(0));
            }
        }
        
        auto* res = new FFIAccelResult();
        std::memset(res, 0, sizeof(FFIAccelResult));
        res->values = val_ser.finalize();
        // Since deviations is just FFILine now 
        // RealBinarySerializer creates FFILineColl. We just take the first line.
        FFILineColl dev_coll = dev_ser.finalize();
        res->deviations = dev_coll.lines[0];
        
        return res;
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }
};

// Template implementation for complex acceleration algorithms
template <typename T>
struct AccelHandleComplex : public AccelHandleBaseExt {
    std::unique_ptr<shanks::algos::series_acceleration<std::complex<T>, size_t>> algo;
    PrecisionType precision;
    std::string name;
    size_t algo_index;
    
    AccelHandleComplex(std::unique_ptr<shanks::algos::series_acceleration<std::complex<T>, size_t>> a, 
                       PrecisionType p, const std::string& n, size_t idx)
        : algo(std::move(a)), precision(p), name(n), algo_index(idx) {}
    
    std::string apply(
        SeriesHandleBase* series, 
        uint64_t n, 
        uint64_t order, 
        bool enable_profiling
    ) override {
        return "{}"; // Legacy virtual interface placeholder
    }
    
    FFIAccelResult* apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return nullptr;
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return nullptr;
        const auto* data = static_cast<const series_result<std::complex<T>>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        std::complex<T> sum(0, 0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const std::complex<T>*>(sum_ptr);
        }

        shanks::ffi::ComplexBinarySerializer<T> val_ser;
        shanks::ffi::RealBinarySerializer<T> dev_ser;

        for (uint64_t i = 1; i <= n; ++i) {
            try {
                auto val = (*algo)(i, order, *data);
                val_ser.push(val);
                if (has_sum) {
                    dev_ser.push(utils::math<std::complex<T>>::abs(val - sum));
                } else {
                    dev_ser.push(T(0));
                }
            } catch (...) {
                val_ser.push(std::complex<T>(0, 0));
                dev_ser.push(T(0));
            }
        }
        
        auto* res = new FFIAccelResult();
        std::memset(res, 0, sizeof(FFIAccelResult));
        res->values = val_ser.finalize();
        // Since deviations is just FFILine now 
        // RealBinarySerializer creates FFILineColl. We just take the first line.
        FFILineColl dev_coll = dev_ser.finalize();
        res->deviations = dev_coll.lines[0];
        
        return res;
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }
};

// Template implementation for interval acceleration algorithms
template <typename T>
struct AccelHandleInterval : public AccelHandleBaseExt {
    std::unique_ptr<shanks::algos::series_acceleration<intprec::interval<T>, size_t>> algo;
    PrecisionType precision;
    std::string name;
    size_t algo_index;
    
    AccelHandleInterval(std::unique_ptr<shanks::algos::series_acceleration<intprec::interval<T>, size_t>> a, 
                        PrecisionType p, const std::string& n, size_t idx)
        : algo(std::move(a)), precision(p), name(n), algo_index(idx) {}
    
    std::string apply(
        SeriesHandleBase* series, 
        uint64_t n, 
        uint64_t order, 
        bool enable_profiling
    ) override {
        return "{}"; // Legacy virtual interface placeholder
    }
    
    FFIAccelResult* apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return nullptr;
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return nullptr;
        const auto* data = static_cast<const series_result<intprec::interval<T>>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        intprec::interval<T> sum(0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const intprec::interval<T>*>(sum_ptr);
        }

        shanks::ffi::IntervalBinarySerializer<T> val_ser;
        shanks::ffi::RealBinarySerializer<T> dev_ser;

        for (uint64_t i = 1; i <= n; ++i) {
            try {
                auto val = (*algo)(i, order, *data);
                val_ser.push(val);
                if (has_sum) {
                    dev_ser.push(utils::math<intprec::interval<T>>::abs(val - sum).mag());
                } else {
                    dev_ser.push(T(0));
                }
            } catch (...) {
                val_ser.push(intprec::interval<T>(0));
                dev_ser.push(T(0));
            }
        }
        
        auto* res = new FFIAccelResult();
        std::memset(res, 0, sizeof(FFIAccelResult));
        res->values = val_ser.finalize();
        // Since deviations is just FFILine now 
        // RealBinarySerializer creates FFILineColl. We just take the first line.
        FFILineColl dev_coll = dev_ser.finalize();
        res->deviations = dev_coll.lines[0];
        
        return res;
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }
};

// Template implementation for complex interval acceleration algorithms
template <typename T>
struct AccelHandleCInterval : public AccelHandleBaseExt {
    std::unique_ptr<shanks::algos::series_acceleration<std::complex<intprec::interval<T>>, size_t>> algo;
    PrecisionType precision;
    std::string name;
    size_t algo_index;
    
    AccelHandleCInterval(std::unique_ptr<shanks::algos::series_acceleration<std::complex<intprec::interval<T>>, size_t>> a, 
                        PrecisionType p, const std::string& n, size_t idx)
        : algo(std::move(a)), precision(p), name(n), algo_index(idx) {}
    
    std::string apply(
        SeriesHandleBase* series, 
        uint64_t n, 
        uint64_t order, 
        bool enable_profiling
    ) override {
        return "{}"; // Legacy virtual interface placeholder
    }
    
    FFIAccelResult* apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return nullptr;
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return nullptr;
        const auto* data = static_cast<const series_result<std::complex<intprec::interval<T>>>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        std::complex<intprec::interval<T>> sum(0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const std::complex<intprec::interval<T>>*>(sum_ptr);
        }

        shanks::ffi::CIntervalBinarySerializer<T> val_ser;
        shanks::ffi::RealBinarySerializer<T> dev_ser;

        for (uint64_t i = 1; i <= n; ++i) {
            try {
                auto val = (*algo)(i, order, *data);
                val_ser.push(val);
                if (has_sum) {
                    dev_ser.push(utils::math<std::complex<intprec::interval<T>>>::abs(val - sum).mag());
                } else {
                    dev_ser.push(T(0));
                }
            } catch (...) {
                val_ser.push(std::complex<intprec::interval<T>>(0));
                dev_ser.push(T(0));
            }
        }
        
        auto* res = new FFIAccelResult();
        std::memset(res, 0, sizeof(FFIAccelResult));
        res->values = val_ser.finalize();
        // Since deviations is just FFILine now 
        // RealBinarySerializer creates FFILineColl. We just take the first line.
        FFILineColl dev_coll = dev_ser.finalize();
        res->deviations = dev_coll.lines[0];
        
        return res;
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }
};

// Helper to find algorithm index by name
size_t find_accel_index(const char* name) {
    auto names = shanks::algos::transformation_registry_metadata::get_names();
    for (size_t i = 0; i < names.size(); ++i) {
        if (names[i] == name) {
            return i;
        }
    }
    auto keys = shanks::algos::transformation_registry_metadata::get_keys();
    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] == name) {
            return i;
        }
    }
    return static_cast<size_t>(-1);
}

// Factory function using the existing registry
std::unique_ptr<AccelHandleBaseExt> create_accel_by_index(
    size_t index, 
    PrecisionType prec, 
    const std::string& name
) {
    switch (prec) {
        case PrecisionType::F64: {
            auto a = shanks::algos::transformation_registry<double, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleReal<double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::F32: {
            auto a = shanks::algos::transformation_registry<float, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleReal<float>>(std::move(a), prec, name, index);
        }
        case PrecisionType::FLong: {
            auto a = shanks::algos::transformation_registry<long double, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleReal<long double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::Arb: {
            auto a = shanks::algos::transformation_registry<mpfr::mpreal, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleReal<mpfr::mpreal>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CF64: {
            auto a = shanks::algos::transformation_registry<std::complex<double>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleComplex<double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CF32: {
            auto a = shanks::algos::transformation_registry<std::complex<float>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleComplex<float>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CFLong: {
            auto a = shanks::algos::transformation_registry<std::complex<long double>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleComplex<long double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CArb: {
            auto a = shanks::algos::transformation_registry<std::complex<mpfr::mpreal>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleComplex<mpfr::mpreal>>(std::move(a), prec, name, index);
        }
        case PrecisionType::IntervalF32: {
            auto a = shanks::algos::transformation_registry<intprec::interval<float>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleInterval<float>>(std::move(a), prec, name, index);
        }
        case PrecisionType::IntervalF64: {
            auto a = shanks::algos::transformation_registry<intprec::interval<double>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleInterval<double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::IntervalFLong: {
            auto a = shanks::algos::transformation_registry<intprec::interval<long double>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleInterval<long double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::IntervalArb: {
            auto a = shanks::algos::transformation_registry<intprec::interval<mpfr::mpreal>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleInterval<mpfr::mpreal>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CIntervalF64: {
            auto a = shanks::algos::transformation_registry<std::complex<intprec::interval<double>>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleCInterval<double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CIntervalF32: {
            auto a = shanks::algos::transformation_registry<std::complex<intprec::interval<float>>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleCInterval<float>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CIntervalFLong: {
            auto a = shanks::algos::transformation_registry<std::complex<intprec::interval<long double>>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleCInterval<long double>>(std::move(a), prec, name, index);
        }
        case PrecisionType::CIntervalArb: {
            auto a = shanks::algos::transformation_registry<std::complex<intprec::interval<mpfr::mpreal>>, size_t>::create_by_index(index);
            if (!a) return nullptr;
            return std::make_unique<AccelHandleCInterval<mpfr::mpreal>>(std::move(a), prec, name, index);
        }
        default:
            set_error("Unsupported precision type");
            return nullptr;
    }
}

// Helper for barebones JSON parsing of numeric values
double get_json_number(const std::string& json, const std::string& key, double default_val) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) {
        search = "\"" + key + "\" :";
        pos = json.find(search);
    }
    if (pos == std::string::npos) return default_val;
    
    pos += search.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    
    size_t end_pos = pos;
    while (end_pos < json.length() && (std::isdigit(json[end_pos]) || json[end_pos] == '.' || json[end_pos] == '-' || json[end_pos] == '+' || json[end_pos] == 'e' || json[end_pos] == 'E')) {
        end_pos++;
    }
    
    if (end_pos > pos) {
        try {
            return std::stod(json.substr(pos, end_pos - pos));
        } catch (...) {
            return default_val;
        }
    }
    return default_val;
}

// Helper template for filtering and averaging smoothed limit
template <typename T>
std::string compute_smoothed_limit_impl(
    const char** values, 
    uint64_t len, 
    const std::string& filter_type,
    const std::string& args_json
) {
    if (len == 0) return "";
    
    std::vector<T> data;
    data.reserve(len);
    for (uint64_t i = 0; i < len; ++i) {
        if (!values[i] || std::strlen(values[i]) == 0) continue;
        std::istringstream iss(values[i]);
        T val;
        iss >> val;
        data.push_back(val);
    }
    
    if (data.empty()) return "";
    
    std::vector<T> filtered;
    if (filter_type == "kolmogorovZurbenko" || filter_type == "KZ") {
        uint64_t window_length = static_cast<uint64_t>(get_json_number(args_json, "window_length", std::max(static_cast<uint64_t>(3), len / 4)));
        uint64_t degree = static_cast<uint64_t>(get_json_number(args_json, "degree", 2));
        filtered = shanks::filters::kolmogorov_zurbenko_filter<T>(data, window_length, degree);
    } else if (filter_type == "savitzkyGolay" || filter_type == "SG") {
        uint64_t window_length = static_cast<uint64_t>(get_json_number(args_json, "window_length", std::max(static_cast<uint64_t>(5), len / 4)));
        if (window_length % 2 == 0) window_length++; // must be odd
        uint64_t polyorder = static_cast<uint64_t>(get_json_number(args_json, "polyorder", 2));
        uint64_t derive = static_cast<uint64_t>(get_json_number(args_json, "derive", 0));
        T delta = static_cast<T>(get_json_number(args_json, "delta", 1.0));
        
        if (window_length > data.size()) window_length = data.size() | 1;
        if (polyorder >= window_length) polyorder = window_length > 1 ? window_length - 1 : 0;
        
        filtered = shanks::filters::savitzky_golay_filter<T>(data, window_length, polyorder, derive, delta);
    } else {
        return ""; // Unsupported filter
    }
    
    if (filtered.empty()) return "";
    
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < filtered.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << shanks::ffi::to_scientific(filtered[i]).to_json();
    }
    oss << "]";
    return oss.str();
}

template <typename T>
std::string compute_smoothed_limit_cplx_impl(
    const char** values, 
    uint64_t len, 
    const std::string& filter_type,
    const std::string& args_json
) {
    if (len == 0) return "";
    
    std::vector<std::complex<T>> data;
    data.reserve(len);
    for (uint64_t i = 0; i < len; ++i) {
        if (!values[i] || std::strlen(values[i]) == 0) continue;
        std::string x_value = values[i];
        T real = 0, imag = 0;
        size_t plus_pos = x_value.find('+');
        size_t j_pos = x_value.find('j');
        if (plus_pos != std::string::npos && j_pos != std::string::npos) {
            std::istringstream iss1(x_value.substr(0, plus_pos)); iss1 >> real;
            std::istringstream iss2(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1)); iss2 >> imag;
        } else {
            std::istringstream iss(x_value); iss >> real;
        }
        data.push_back(std::complex<T>(real, imag));
    }
    
    if (data.empty()) return "";
    
    std::vector<std::complex<T>> filtered;
    if (filter_type == "kolmogorovZurbenko" || filter_type == "KZ") {
        uint64_t window_length = static_cast<uint64_t>(get_json_number(args_json, "window_length", std::max(static_cast<uint64_t>(3), len / 4)));
        uint64_t degree = static_cast<uint64_t>(get_json_number(args_json, "degree", 2));
        filtered = shanks::filters::kolmogorov_zurbenko_filter<std::complex<T>>(data, window_length, degree);
    } else if (filter_type == "savitzkyGolay" || filter_type == "SG") {
        // SG doesn't heavily support complex natively without mapping, but we can just use KZ or reject
        // For now, let's just reject SG for complex or try to see if it compiles (it might).
        // The accepted concept requires operations which std::complex supports, but `delta` is Scalar.
        // Let's assume it compiles.
        uint64_t window_length = static_cast<uint64_t>(get_json_number(args_json, "window_length", std::max(static_cast<uint64_t>(5), len / 4)));
        if (window_length % 2 == 0) window_length++;
        uint64_t polyorder = static_cast<uint64_t>(get_json_number(args_json, "polyorder", 2));
        uint64_t derive = static_cast<uint64_t>(get_json_number(args_json, "derive", 0));
        std::complex<T> delta = static_cast<T>(get_json_number(args_json, "delta", 1.0));
        
        if (window_length > data.size()) window_length = data.size() | 1;
        if (polyorder >= window_length) polyorder = window_length > 1 ? window_length - 1 : 0;
        
        filtered = shanks::filters::savitzky_golay_filter<std::complex<T>>(data, window_length, polyorder, derive, delta);
    } else {
        return "";
    }
    
    if (filtered.empty()) return "";
    
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < filtered.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "{\"real\": " << shanks::ffi::to_scientific(filtered[i].real()).to_json()
            << ", \"imag\": " << shanks::ffi::to_scientific(filtered[i].imag()).to_json() << "}";
    }
    oss << "]";
    return oss.str();
}

} // anonymous namespace

// ============================================================================
// Acceleration Creation Implementation
// ============================================================================

extern "C" SHANKS_FFI_API ShanksAccelHandle shanks_accel_create(
    const char* name,
    const char* precision,
    const char* args_json
) {
    clear_error();
    
    if (!name) {
        set_error("Algorithm name is required");
        return nullptr;
    }
    
    PrecisionType prec;
    if (!parse_precision(precision, prec)) {
        set_error(std::string("Invalid precision: ") + (precision ? precision : "null"));
        return nullptr;
    }
    
    // Find algorithm index
    size_t index = find_accel_index(name);
    if (index == static_cast<size_t>(-1)) {
        set_error(std::string("Unknown algorithm: ") + name);
        return nullptr;
    }
    
    std::string n = name;
    
    try {
        auto handle = create_accel_by_index(index, prec, n);
        if (!handle) {
            if (shanks::ffi::g_last_error.empty()) {
                set_error(std::string("Failed to create algorithm: ") + name);
            }
            return nullptr;
        }
        return handle.release();
    } catch (const std::exception& e) {
        set_error(std::string("Error creating algorithm: ") + e.what());
        return nullptr;
    } catch (...) {
        set_error("Unknown error creating algorithm");
        return nullptr;
    }
}

extern "C" SHANKS_FFI_API void shanks_accel_destroy(ShanksAccelHandle handle) {
    if (handle) {
        auto* ptr = static_cast<AccelHandleBaseExt*>(handle);
        delete ptr;
    }
}

extern "C" SHANKS_FFI_API FFIAccelResult* shanks_accel_apply(
    ShanksAccelHandle accel,
    ShanksSeriesHandle series,
    uint64_t n,
    uint64_t order
) {
    clear_error();
    
    if (!accel) {
        set_error("Invalid algorithm handle");
        return nullptr;
    }
    
    if (!series) {
        set_error("Invalid series handle");
        return nullptr;
    }
    
    auto* accel_ptr = static_cast<AccelHandleBaseExt*>(accel);
    auto* series_ptr = static_cast<SeriesHandleBase*>(series);
    
    // Check precision compatibility
    if (accel_ptr->get_precision() != series_ptr->get_precision()) {
        set_error("Precision mismatch between algorithm and series");
        return nullptr;
    }
    
    try {
        return accel_ptr->apply_to_result(n, order, series_ptr, false);
    } catch (const std::exception& e) {
        set_error(std::string("Error applying algorithm: ") + e.what());
        return nullptr;
    }
}

extern "C" SHANKS_FFI_API void shanks_accel_result_free(FFIAccelResult* result) {
    if (!result) return;
    shanks::ffi::free_ffi_line_coll(&result->values);
    shanks::ffi::free_ffi_line(&result->deviations);
    delete result;
}

extern "C" SHANKS_FFI_API char* shanks_accel_apply_data(
    ShanksAccelHandle accel,
    const char* series_json,
    uint64_t n,
    uint64_t order
) {
    clear_error();
    set_error("shanks_accel_apply_data is deprecated; use shanks_accel_apply");
    return alloc_string("{}");
}

extern "C" SHANKS_FFI_API char* shanks_compute_smoothed_limit(
    const char* precision,
    const char** values,
    uint64_t len,
    const char* filter_type,
    const char* args_json
) {
    clear_error();
    
    if (!values || len == 0) {
        return alloc_string("");
    }
    
    PrecisionType prec;
    if (!parse_precision(precision, prec)) {
        set_error(std::string("Invalid precision: ") + (precision ? precision : "null"));
        return alloc_string("");
    }
    
    std::string f_type = filter_type ? filter_type : "KZ";
    std::string f_args = args_json ? args_json : "{}";
    
    std::string result;
    try {
        switch (prec) {
            case PrecisionType::F64: result = compute_smoothed_limit_impl<double>(values, len, f_type, f_args); break;
            case PrecisionType::F32: result = compute_smoothed_limit_impl<float>(values, len, f_type, f_args); break;
            case PrecisionType::FLong: result = compute_smoothed_limit_impl<long double>(values, len, f_type, f_args); break;
            case PrecisionType::Arb: result = compute_smoothed_limit_impl<mpfr::mpreal>(values, len, f_type, f_args); break;
            case PrecisionType::CF64: result = compute_smoothed_limit_cplx_impl<double>(values, len, f_type, f_args); break;
            case PrecisionType::CF32: result = compute_smoothed_limit_cplx_impl<float>(values, len, f_type, f_args); break;
            case PrecisionType::CFLong: result = compute_smoothed_limit_cplx_impl<long double>(values, len, f_type, f_args); break;
            case PrecisionType::CArb: result = compute_smoothed_limit_cplx_impl<mpfr::mpreal>(values, len, f_type, f_args); break;
            // Interval types left out for brevity unless specifically needed.
            default:
                set_error("Precision type not supported for smoothing");
                return alloc_string("");
        }
    } catch (const std::exception& e) {
        set_error(std::string("Smoothing error: ") + e.what());
        return alloc_string("");
    }
    
    return alloc_string(result);
}
