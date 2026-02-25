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

// JSON helper for complex values
std::string complex_to_json(double real, double imag) {
    std::ostringstream oss;
    oss << "{\"real\": " << to_scientific(real).to_json() 
        << ", \"imag\": " << to_scientific(imag).to_json() << "}";
    return oss.str();
}

// Extended acceleration handle
struct AccelHandleBaseExt : public AccelHandleBase {
    virtual std::string apply_to_result(
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
        return apply_to_result(n, order, series, enable_profiling);
    }
    
    std::string apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return "{}";
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return "{}";
        const auto* data = static_cast<const series_result<T>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        T sum = T(0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const T*>(sum_ptr);
        }

        std::vector<ScientificValue> deviations;
        std::vector<unsigned long long> trace_add, trace_mul, trace_div, trace_special;

        if (enable_profiling) {
            trace_add.reserve(n); trace_mul.reserve(n); trace_div.reserve(n); trace_special.reserve(n);
        }

#ifdef SHANKS_ENABLE_PROFILING
        using ProfT = shanks::profiling::OperationCounting<T>;
        std::unique_ptr<shanks::algos::series_acceleration<ProfT, size_t>> algo_prof;
        ::series_result<ProfT> data_prof;
        
        if (enable_profiling) {
            algo_prof = shanks::algos::transformation_registry<ProfT, size_t>::create_by_index(algo_index);
            data_prof.Sn.reserve(data->Sn.size());
            data_prof.an.reserve(data->an.size());
            for (const auto& val : data->Sn) data_prof.Sn.push_back(ProfT(val));
            for (const auto& val : data->an) data_prof.an.push_back(ProfT(val));
        }
#endif

        std::ostringstream oss;
        oss << "{\"values\": [";
        bool first = true;
        for (uint64_t i = 1; i <= n; ++i) {
            if (!first) oss << ", ";
            first = false;
            
            try {
                T val = (*algo)(i, order, *data);
                
#ifdef SHANKS_ENABLE_PROFILING
                if (enable_profiling && algo_prof) {
                    shanks::profiling::reset_counts();
                    auto before = shanks::profiling::get_counts();
                    (*algo_prof)(i, order, data_prof);
                    auto after = shanks::profiling::get_counts();
                    
                    trace_add.push_back(after.add - before.add);
                    trace_mul.push_back(after.mul - before.mul);
                    trace_div.push_back(after.div - before.div);
                    trace_special.push_back(after.special - before.special);
                }
#endif
                
                oss << shanks::ffi::to_scientific(val).to_json();
                if (has_sum) {
                    deviations.push_back(shanks::ffi::to_scientific(utils::math<T>::abs(val - sum)));
                } else {
                    deviations.push_back(ScientificValue(0.0, 0));
                }
            } catch (...) {
                oss << "null";
                deviations.push_back(ScientificValue(0.0, 0));
                if (enable_profiling) {
                    trace_add.push_back(0); trace_mul.push_back(0); trace_div.push_back(0); trace_special.push_back(0);
                }
            }
        }
        oss << "], \"deviations\": [";
        for (size_t i = 0; i < deviations.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << deviations[i].to_json();
        }
        oss << "]";
        
#ifdef SHANKS_ENABLE_PROFILING
        if (enable_profiling) {
            auto append_v = [&](const std::vector<unsigned long long>& v) {
                oss << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << v[i];
                }
                oss << "]";
            };
            oss << ", \"profiling\": {\"add\": "; append_v(trace_add);
            oss << ", \"mul\": "; append_v(trace_mul);
            oss << ", \"div\": "; append_v(trace_div);
            oss << ", \"special\": "; append_v(trace_special);
            oss << "}";
            shanks::profiling::reset_counts();
        }
#endif
        
        oss << "}";
        return oss.str();
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
        return apply_to_result(n, order, series, enable_profiling);
    }
    
    std::string apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return "{}";
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return "{}";
        const auto* data = static_cast<const series_result<std::complex<T>>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        std::complex<T> sum(0, 0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const std::complex<T>*>(sum_ptr);
        }

        std::vector<ScientificValue> deviations;
        std::vector<unsigned long long> trace_add, trace_mul, trace_div, trace_special;

        if (enable_profiling) {
            trace_add.reserve(n); trace_mul.reserve(n); trace_div.reserve(n); trace_special.reserve(n);
        }

#ifdef SHANKS_ENABLE_PROFILING
        using ProfRealT = shanks::profiling::OperationCounting<T>;
        using ProfT = std::complex<ProfRealT>;
        std::unique_ptr<shanks::algos::series_acceleration<ProfT, size_t>> algo_prof;
        ::series_result<ProfT> data_prof;
        
        if (enable_profiling) {
            algo_prof = shanks::algos::transformation_registry<ProfT, size_t>::create_by_index(algo_index);
            data_prof.Sn.reserve(data->Sn.size());
            data_prof.an.reserve(data->an.size());
            for (const auto& val : data->Sn) data_prof.Sn.push_back(ProfT(ProfRealT(val.real()), ProfRealT(val.imag())));
            for (const auto& val : data->an) data_prof.an.push_back(ProfT(ProfRealT(val.real()), ProfRealT(val.imag())));
        }
#endif

        std::ostringstream oss;
        oss << "{\"values\": [";
        bool first = true;
        for (uint64_t i = 1; i <= n; ++i) {
            if (!first) oss << ", ";
            first = false;
            
            try {
                auto val = (*algo)(i, order, *data);
                
#ifdef SHANKS_ENABLE_PROFILING
                if (enable_profiling && algo_prof) {
                    shanks::profiling::reset_counts();
                    auto before = shanks::profiling::get_counts();
                    (*algo_prof)(i, order, data_prof);
                    auto after = shanks::profiling::get_counts();
                    
                    trace_add.push_back(after.add - before.add);
                    trace_mul.push_back(after.mul - before.mul);
                    trace_div.push_back(after.div - before.div);
                    trace_special.push_back(after.special - before.special);
                }
#endif
                
                oss << shanks::ffi::complex_to_json(val.real(), val.imag());
                if (has_sum) {
                    deviations.push_back(shanks::ffi::to_scientific(utils::math<std::complex<T>>::abs(val - sum)));
                } else {
                    deviations.push_back(ScientificValue(0.0, 0));
                }
            } catch (...) {
                oss << "null";
                deviations.push_back(ScientificValue(0.0, 0));
                if (enable_profiling) {
                    trace_add.push_back(0); trace_mul.push_back(0); trace_div.push_back(0); trace_special.push_back(0);
                }
            }
        }
        oss << "], \"deviations\": [";
        for (size_t i = 0; i < deviations.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << deviations[i].to_json();
        }
        oss << "]";
        
#ifdef SHANKS_ENABLE_PROFILING
        if (enable_profiling) {
            auto append_v = [&](const std::vector<unsigned long long>& v) {
                oss << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << v[i];
                }
                oss << "]";
            };
            oss << ", \"profiling\": {\"add\": "; append_v(trace_add);
            oss << ", \"mul\": "; append_v(trace_mul);
            oss << ", \"div\": "; append_v(trace_div);
            oss << ", \"special\": "; append_v(trace_special);
            oss << "}";
            shanks::profiling::reset_counts();
        }
#endif
        
        oss << "}";
        return oss.str();
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
        return apply_to_result(n, order, series, enable_profiling);
    }
    
    std::string apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return "{}";
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return "{}";
        const auto* data = static_cast<const series_result<intprec::interval<T>>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        intprec::interval<T> sum(0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const intprec::interval<T>*>(sum_ptr);
        }

        std::vector<ScientificValue> deviations;

        std::ostringstream oss;
        oss << "{\"values\": [";
        bool first = true;
        for (uint64_t i = 1; i <= n; ++i) {
            if (!first) oss << ", ";
            first = false;
            
            try {
                auto val = (*algo)(i, order, *data);
                
                oss << shanks::ffi::interval_to_json(val);
                if (has_sum) {
                    // Deviation mapping: absolute magnitude of the interval difference
                    deviations.push_back(shanks::ffi::to_scientific(utils::math<intprec::interval<T>>::abs(val - sum).mag()));
                } else {
                    deviations.push_back(ScientificValue(0.0, 0));
                }
            } catch (...) {
                oss << "null";
                deviations.push_back(ScientificValue(0.0, 0));
            }
        }
        oss << "], \"deviations\": [";
        for (size_t i = 0; i < deviations.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << deviations[i].to_json();
        }
        oss << "]";
        
#ifdef SHANKS_ENABLE_PROFILING
        if (enable_profiling) {
            auto counts = shanks::profiling::get_counts();
            oss << ", \"profiling\": {"
                << "\"add\": [" << counts.add << "], "
                << "\"mul\": [" << counts.mul << "], "
                << "\"div\": [" << counts.div << "], "
                << "\"special\": [" << counts.special << "]}";
            shanks::profiling::reset_counts();
        }
#endif
        
        oss << "}";
        return oss.str();
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
        return apply_to_result(n, order, series, enable_profiling);
    }
    
    std::string apply_to_result(
        uint64_t n, 
        uint64_t order, 
        SeriesHandleBase* series, 
        bool enable_profiling
    ) override {
        if (!algo) {
            set_error("Algorithm is null");
            return "{}";
        }
        
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif

        const void* raw_data = series->get_raw_data(n + 3 * order + 1);
        if (!raw_data) return "{}";
        const auto* data = static_cast<const series_result<std::complex<intprec::interval<T>>>*>(raw_data);
        
        const void* sum_ptr = series->get_native_sum();
        std::complex<intprec::interval<T>> sum(0);
        bool has_sum = (sum_ptr != nullptr);
        if (has_sum) {
            sum = *static_cast<const std::complex<intprec::interval<T>>*>(sum_ptr);
        }

        std::vector<ScientificValue> deviations;

        std::ostringstream oss;
        oss << "{\"values\": [";
        bool first = true;
        for (uint64_t i = 1; i <= n; ++i) {
            if (!first) oss << ", ";
            first = false;
            
            try {
                auto val = (*algo)(i, order, *data);
                
                oss << shanks::ffi::complex_interval_to_json(val);
                if (has_sum) {
                    // Deviation mapping: absolute magnitude of the complex interval difference
                    deviations.push_back(shanks::ffi::to_scientific(utils::math<std::complex<intprec::interval<T>>>::abs(val - sum).mag()));
                } else {
                    deviations.push_back(ScientificValue(0.0, 0));
                }
            } catch (...) {
                oss << "null";
                deviations.push_back(ScientificValue(0.0, 0));
            }
        }
        oss << "], \"deviations\": [";
        for (size_t i = 0; i < deviations.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << deviations[i].to_json();
        }
        oss << "]";
        
#ifdef SHANKS_ENABLE_PROFILING
        if (enable_profiling) {
            auto counts = shanks::profiling::get_counts();
            oss << ", \"profiling\": {"
                << "\"add\": [" << counts.add << "], "
                << "\"mul\": [" << counts.mul << "], "
                << "\"div\": [" << counts.div << "], "
                << "\"special\": [" << counts.special << "]}";
            shanks::profiling::reset_counts();
        }
#endif
        
        oss << "}";
        return oss.str();
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
            // Complex arbitrary precision - may not be supported
            set_error("Complex arbitrary precision not yet supported for algorithms");
            return nullptr;
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
            set_error("Complex arbitrary interval precision not yet supported for algorithms");
            return nullptr;
        }
        default:
            set_error("Unsupported precision type");
            return nullptr;
    }
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
            set_error(std::string("Failed to create algorithm: ") + name);
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

extern "C" SHANKS_FFI_API char* shanks_accel_apply(
    ShanksAccelHandle accel,
    ShanksSeriesHandle series,
    uint64_t n,
    uint64_t order,
    int enable_profiling
) {
    clear_error();
    
    if (!accel) {
        set_error("Invalid algorithm handle");
        return alloc_string("{}");
    }
    
    if (!series) {
        set_error("Invalid series handle");
        return alloc_string("{}");
    }
    
    auto* accel_ptr = static_cast<AccelHandleBaseExt*>(accel);
    auto* series_ptr = static_cast<SeriesHandleBase*>(series);
    
    // Check precision compatibility
    if (accel_ptr->get_precision() != series_ptr->get_precision()) {
        set_error("Precision mismatch between algorithm and series");
        return alloc_string("{}");
    }
    
    try {
        std::string result = accel_ptr->apply(series_ptr, n, order, enable_profiling != 0);
        return alloc_string(result);
    } catch (const std::exception& e) {
        set_error(std::string("Error applying algorithm: ") + e.what());
        return alloc_string("{}");
    }
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
