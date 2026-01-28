#ifndef OPERATION_COUNTER_HPP
#define OPERATION_COUNTER_HPP

#include <cmath>
#include <complex>
#include <iostream>
#include <limits>
#include <tuple>

#include "custom_concepts.hpp"
#include "utils/utils_helper.hpp"

#ifdef SHANKS_ENABLE_PROFILING

namespace shanks {
namespace profiling {

template <typename T>
class OperationCounting;

}  // namespace profiling
}  // namespace shanks

template <typename T>
struct ADL_Unwrapper<shanks::profiling::OperationCounting<T>> {
    static const T& unwrap(const shanks::profiling::OperationCounting<T>& x) { return x.value; }
};

namespace shanks {
namespace profiling {

/**
 * @brief Structure to hold operation counts.
 */
struct OperationCounts {
    unsigned long long add = 0;      // Addition and Subtraction
    unsigned long long mul = 0;      // Multiplication
    unsigned long long div = 0;      // Division
    unsigned long long special = 0;  // Special functions (sin, exp, sqrt, etc.)

    void reset() {
        add = 0;
        mul = 0;
        div = 0;
        special = 0;
    }

    OperationCounts& operator+=(const OperationCounts& other) {
        add += other.add;
        mul += other.mul;
        div += other.div;
        special += other.special;
        return *this;
    }
};

// Thread-local global counter instance
inline thread_local OperationCounts global_operation_counts;

/**
 * @brief Resets the global operation counters for the current thread.
 */
inline void reset_counts() { global_operation_counts.reset(); }

/**
 * @brief Gets the current operation counts.
 */
inline OperationCounts get_counts() { return global_operation_counts; }

/**
 * @brief Wrapper class that counts operations performed on type T.
 * @tparam T Underlying floating-point type.
 */
template <typename T>
class OperationCounting {
public:
    T value;

    // Constructors
    OperationCounting() : value(T(0)) {}
    OperationCounting(const T& v) : value(v) {}
    OperationCounting(T&& v) : value(std::move(v)) {}

    // Generic constructor for implicit conversions (e.g. int -> T ->
    // OperationCounting) Constrained to avoid execution when U is same as T (use
    // direct constructor) or when U is OperationCounting (use copy/move ctor)
    template <typename U>
        requires std::is_constructible_v<T, U> && (!std::is_same_v<std::remove_cvref_t<U>, OperationCounting>) &&
                 (!std::is_same_v<std::remove_cvref_t<U>, T>)
    OperationCounting(const U& v) : value(static_cast<T>(v)) {}

    // Generic forwarding constructor for multi-argument initialization (e.g.
    // mpreal(d, prec)) Constrained to require multiple arguments to avoid
    // conflict with single-arg constructors
    template <typename... Args>
        requires std::is_constructible_v<T, Args...> && (sizeof...(Args) > 1)
    OperationCounting(Args&&... args) : value(std::forward<Args>(args)...) {}

    // Generic explicit conversion operator
    template <typename U>
    explicit operator U() const {
        return static_cast<U>(value);
    }

    // Assignment
    OperationCounting& operator=(const T& v) {
        value = v;
        return *this;
    }

    // Arithmetic Operators
    OperationCounting operator+(const OperationCounting& other) const {
        global_operation_counts.add++;
        return OperationCounting(value + other.value);
    }

    OperationCounting operator-(const OperationCounting& other) const {
        global_operation_counts.add++;
        return OperationCounting(value - other.value);
    }

    OperationCounting operator*(const OperationCounting& other) const {
        global_operation_counts.mul++;
        return OperationCounting(value * other.value);
    }

    OperationCounting operator/(const OperationCounting& other) const {
        global_operation_counts.div++;
        return OperationCounting(value / other.value);
    }

    // Unary Operators
    OperationCounting operator-() const { return OperationCounting(-value); }

    // Compound Assignment
    OperationCounting& operator+=(const OperationCounting& other) {
        global_operation_counts.add++;
        value += other.value;
        return *this;
    }

    OperationCounting& operator-=(const OperationCounting& other) {
        global_operation_counts.add++;
        value -= other.value;
        return *this;
    }

    OperationCounting& operator*=(const OperationCounting& other) {
        global_operation_counts.mul++;
        value *= other.value;
        return *this;
    }

    OperationCounting& operator/=(const OperationCounting& other) {
        global_operation_counts.div++;
        value /= other.value;
        return *this;
    }

    // Comparison (No op count)
    auto operator<=>(const OperationCounting& other) const = default;  // C++20

    bool operator==(const OperationCounting& other) const { return value == other.value; }
    bool operator!=(const OperationCounting& other) const { return value != other.value; }
    bool operator<(const OperationCounting& other) const { return value < other.value; }
    bool operator>(const OperationCounting& other) const { return value > other.value; }
    bool operator<=(const OperationCounting& other) const { return value <= other.value; }
    bool operator>=(const OperationCounting& other) const { return value >= other.value; }
};

// Mixed mode arithmetic (Scalar op T)
// Left operand is OperationCounting
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator+(const OperationCounting<T>& lhs, const U& rhs) {
    global_operation_counts.add++;
    return OperationCounting<T>(lhs.value + static_cast<T>(rhs));
}
// Right operand is OperationCounting
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator+(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.add++;
    return OperationCounting<T>(static_cast<T>(lhs) + rhs.value);
}

template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator-(const OperationCounting<T>& lhs, const U& rhs) {
    global_operation_counts.add++;
    return OperationCounting<T>(lhs.value - static_cast<T>(rhs));
}
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator-(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.add++;
    return OperationCounting<T>(static_cast<T>(lhs) - rhs.value);
}

template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator*(const OperationCounting<T>& lhs, const U& rhs) {
    global_operation_counts.mul++;
    return OperationCounting<T>(lhs.value * static_cast<T>(rhs));
}
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator*(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.mul++;
    return OperationCounting<T>(static_cast<T>(lhs) * rhs.value);
}

template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator/(const OperationCounting<T>& lhs, const U& rhs) {
    global_operation_counts.div++;
    return OperationCounting<T>(lhs.value / static_cast<T>(rhs));
}
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator/(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.div++;
    return OperationCounting<T>(static_cast<T>(lhs) / rhs.value);
}

// Special functions
template <typename T>
OperationCounting<T> abs(const OperationCounting<T>& v) {
    using std::abs;
    return OperationCounting<T>(abs(v.value));
}

template <typename T>
OperationCounting<T> sqrt(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::sqrt;
    return OperationCounting<T>(sqrt(v.value));
}

template <typename T>
OperationCounting<T> pow(const OperationCounting<T>& b, const OperationCounting<T>& e) {
    global_operation_counts.special++;
    using std::pow;
    return OperationCounting<T>(pow(b.value, e.value));
}

template <typename T>
OperationCounting<T> atan2(const OperationCounting<T>& y, const OperationCounting<T>& x) {
    global_operation_counts.special++;
    using std::atan2;
    return OperationCounting<T>(atan2(y.value, x.value));
}

template <typename T>
OperationCounting<T> exp(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::exp;
    return OperationCounting<T>(exp(v.value));
}

template <typename T>
OperationCounting<T> log(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::log;
    return OperationCounting<T>(log(v.value));
}

template <typename T>
OperationCounting<T> sin(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::sin;
    return OperationCounting<T>(sin(v.value));
}

template <typename T>
OperationCounting<T> cos(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::cos;
    return OperationCounting<T>(cos(v.value));
}

template <typename T>
OperationCounting<T> tan(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::tan;
    return OperationCounting<T>(tan(v.value));
}

template <typename T>
OperationCounting<T> asin(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::asin;
    return OperationCounting<T>(asin(v.value));
}

template <typename T>
OperationCounting<T> acos(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::acos;
    return OperationCounting<T>(acos(v.value));
}

template <typename T>
OperationCounting<T> atan(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::atan;
    return OperationCounting<T>(atan(v.value));
}

template <typename T>
OperationCounting<T> sinh(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::sinh;
    return OperationCounting<T>(sinh(v.value));
}

template <typename T>
OperationCounting<T> cosh(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::cosh;
    return OperationCounting<T>(cosh(v.value));
}

template <typename T>
OperationCounting<T> tanh(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::tanh;
    return OperationCounting<T>(tanh(v.value));
}

template <typename T>
OperationCounting<T> asinh(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::asinh;
    return OperationCounting<T>(asinh(v.value));
}

template <typename T>
OperationCounting<T> acosh(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::acosh;
    return OperationCounting<T>(acosh(v.value));
}

template <typename T>
OperationCounting<T> atanh(const OperationCounting<T>& v) {
    global_operation_counts.special++;
    using std::atanh;
    return OperationCounting<T>(atanh(v.value));
}

// Helper functions for utils
template <typename T>
bool isfinite(const OperationCounting<T>& v) {
    if constexpr (std::is_floating_point<T>::value)
        return std::isfinite(v.value);
    else
        return ::utils::isfinite(v.value);
}

template <typename T>
std::string to_string(const OperationCounting<T>& v) {
    return ::utils::to_string(v.value);
}

}  // namespace profiling
}  // namespace shanks

// Specialization of std::numeric_limits
namespace std {
template <typename T>
struct numeric_limits<shanks::profiling::OperationCounting<T>> : public numeric_limits<T> {
    static constexpr bool is_specialized = true;
};

// Specializations for classification functions - Overloading std namespace
template <typename T>
bool isfinite(const shanks::profiling::OperationCounting<T>& v) {
    return shanks::profiling::isfinite(v);
}

template <typename T>
bool isnan(const shanks::profiling::OperationCounting<T>& v) {
    using std::isnan;
    if constexpr (std::is_same_v<T, mpfr::mpreal>)
        return isnan(v.value);  // mpreal might expect ADL or global
    else
        return std::isnan(v.value);
}

template <typename T>
bool isinf(const shanks::profiling::OperationCounting<T>& v) {
    using std::isinf;
    if constexpr (std::is_same_v<T, mpfr::mpreal>)
        return isinf(v.value);
    else
        return std::isinf(v.value);
}

}  // namespace std

#else

// Dummy Implementation when Profiling is Disabled
namespace shanks {
namespace profiling {

struct OperationCounts {
    unsigned long long add = 0;
    unsigned long long mul = 0;
    unsigned long long div = 0;
    unsigned long long special = 0;
};

// Aliasing OperationCounting to T directly
template <typename T>
using OperationCounting = T;

inline void reset_counts() {}
inline OperationCounts get_counts() { return {}; }

// Helper functions for utils
template <typename T>
bool isfinite(const T& v) {
    if constexpr (std::is_floating_point<T>::value)
        return std::isfinite(v);
    else
        return ::utils::isfinite(v);
}

template <typename T>
std::string to_string(const T& v) {
    return ::utils::to_string(v);
}

// Special functions proxies
template <typename T>
T abs(const T& v) {
    return ::utils::abs(v);
}

template <typename T>
T sqrt(const T& v) {
    return ::utils::sqrt(v);
}

template <typename T>
T pow(const T& b, const T& e) {
    return ::utils::pow(b, e);
}

template <typename T>
T atan2(const T& y, const T& x) {
    return ::utils::atan2(y, x);
}

template <typename T>
T exp(const T& v) {
    return ::utils::exp(v);
}

template <typename T>
T log(const T& v) {
    return ::utils::log(v);
}

template <typename T>
T sin(const T& v) {
    return ::utils::sin(v);
}

template <typename T>
T cos(const T& v) {
    return ::utils::cos(v);
}

template <typename T>
T tan(const T& v) {
    return ::utils::tan(v);
}

template <typename T>
T asin(const T& v) {
    return ::utils::asin(v);
}

template <typename T>
T acos(const T& v) {
    return ::utils::acos(v);
}

template <typename T>
T atan(const T& v) {
    return ::utils::atan(v);
}

template <typename T>
T sinh(const T& v) {
    return ::utils::sinh(v);
}

template <typename T>
T cosh(const T& v) {
    return ::utils::cosh(v);
}

template <typename T>
T tanh(const T& v) {
    return ::utils::tanh(v);
}

template <typename T>
T asinh(const T& v) {
    return ::utils::asinh(v);
}

template <typename T>
T acosh(const T& v) {
    return ::utils::acosh(v);
}

template <typename T>
T atanh(const T& v) {
    return ::utils::atanh(v);
}

}  // namespace profiling
}  // namespace shanks

// Forward declaration
namespace shanks {
namespace profiling {
#ifdef SHANKS_ENABLE_PROFILING
template <typename T>
class OperationCounting;
#endif
}  // namespace profiling
}  // namespace shanks

/**
 * @brief Type trait to check if a type is OperationCounting wrapper.
 */
template <typename T>
struct is_operation_counting : public std::false_type {};
#ifdef SHANKS_ENABLE_PROFILING
template <typename T>
struct is_operation_counting<shanks::profiling::OperationCounting<T>> : public std::true_type {};
#endif

/**
 * @brief Type trait to get the underlying type of a wrapped type.
 */
template <typename T>
struct get_wrapped_type {
    using type = T;
};
#ifdef SHANKS_ENABLE_PROFILING
template <typename T>
struct get_wrapped_type<shanks::profiling::OperationCounting<T>> {
    using type = T;
};
#endif

/**
 * @brief Utility to unwrap types (default: no-op).
 */
template <typename T>
struct ADL_Unwrapper {
    static const T& unwrap(const T& x) { return x; }
};

/**
 * @brief Utility to wrap types (default: no-op).
 */
template <typename T>
struct ADL_Wrapper {
    template <typename U>
    static T wrap(U&& x) {
        return T(std::forward<U>(x));
    }
};

/**
 * @brief Type trait to check if a type behaves like a floating-point number.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to inspect.
 */

template <typename T>
struct isFloatLike : public std::false_type {};

template <std::floating_point T>
struct isFloatLike<T> : public std::true_type {};
#ifdef SHANKS_ENABLE_PROFILING
template <typename T>
struct isFloatLike<shanks::profiling::OperationCounting<T>> : public std::true_type {};
#endif
#ifdef __MPREAL_H__
template <>
struct isFloatLike<mpfr::mpreal> : public std::true_type {};
#endif

/**
 * @brief Concept for types that behave like floating-point numbers.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
concept FloatLike = isFloatLike<T>::value;

/**
 * @brief Type trait to check if a type is a standard complex type with floating-point components.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_complex_t : public std::false_type {};

template <std::floating_point U>
struct is_complex_t<std::complex<U>> : public std::true_type {};
#ifdef SHANKS_ENABLE_PROFILING
template <typename U>
struct is_complex_t<std::complex<shanks::profiling::OperationCounting<U>>> : public std::true_type {};
#endif

/**
 * @brief Type trait to check if a type supports explicit precision settings.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_precisable : public std::false_type {};
#ifdef __MPREAL_H__
template <>
struct is_precisable<mpfr::mpreal> : public std::true_type {};
template <>
struct is_precisable<std::complex<mpfr::mpreal>> : public std::true_type {};
#ifdef SHANKS_ENABLE_PROFILING
template <typename T>
struct is_precisable<shanks::profiling::OperationCounting<T>> : public is_precisable<T> {};
#endif
#endif

#endif  // SHANKS_ENABLE_PROFILING

#endif  // OPERATION_COUNTER_HPP
