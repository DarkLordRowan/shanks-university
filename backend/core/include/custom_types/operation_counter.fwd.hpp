#ifndef OPERATION_COUNTER_FWD_HPP
#define OPERATION_COUNTER_FWD_HPP
#pragma once

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

template <typename T>
class OperationCounting {
public:
    using value_type = T;

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
    explicit operator U() const;

    // Assignment
    OperationCounting& operator=(const T&);

    // Arithmetic Operators
    OperationCounting operator+(const OperationCounting&) const;
    OperationCounting operator-(const OperationCounting&) const;
    OperationCounting operator*(const OperationCounting&) const;
    OperationCounting operator/(const OperationCounting&) const;

    // Unary Operators
    OperationCounting operator-() const;

    // Compound Assignment
    OperationCounting& operator+=(const OperationCounting&);
    OperationCounting& operator-=(const OperationCounting&);
    OperationCounting& operator*=(const OperationCounting&);
    OperationCounting& operator/=(const OperationCounting&);

    // Comparison (No op count)
    auto operator<=>(const OperationCounting&) const;  // C++20

    bool operator==(const OperationCounting&) const;
    bool operator!=(const OperationCounting&) const;
    bool operator<(const OperationCounting&) const;
    bool operator>(const OperationCounting&) const;
    bool operator<=(const OperationCounting&) const;
    bool operator>=(const OperationCounting&) const;
};

}  // namespace profiling
}  // namespace shanks

#endif