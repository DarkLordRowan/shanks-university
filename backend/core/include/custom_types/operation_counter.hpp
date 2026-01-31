#ifndef OPERATION_COUNTER_HPP
#define OPERATION_COUNTER_HPP

#ifdef SHANKS_ENABLE_PROFILING

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
    return OperationCounting<T>(lhs.value + rhs);
}
// Right operand is OperationCounting
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator+(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.add++;
    return OperationCounting<T>(lhs + rhs.value);
}

template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator-(const OperationCounting<T>& lhs, const U& rhs) {
    global_operation_counts.add++;
    return OperationCounting<T>(lhs.value - rhs);
}
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator-(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.add++;
    return OperationCounting<T>(lhs - rhs.value);
}

template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator*(const OperationCounting<T>& lhs, const U& rhs) {
    global_operation_counts.mul++;
    return OperationCounting<T>(lhs.value * rhs);
}
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator*(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.mul++;
    return OperationCounting<T>(lhs * rhs.value);
}

template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator/(const OperationCounting<T>& lhs, const U& rhs) {
    global_operation_counts.div++;
    return OperationCounting<T>(lhs.value / rhs);
}
template <typename T, typename U>
    requires std::is_convertible_v<U, T>
OperationCounting<T> operator/(const U& lhs, const OperationCounting<T>& rhs) {
    global_operation_counts.div++;
    return OperationCounting<T>(lhs / rhs.value);
}

template<typename T>
std::istream& operator>>(std::istream& fin, OperationCounting<T>& x){
    fin >> x.value;
    return fin;
}

template<typename T>
std::ostream& operator<<(std::ostream& fout, const OperationCounting<T>& x){
    fout << x.value;
    return fout;
}

}  // namespace profiling
}  // namespace shanks


#endif  // SHANKS_ENABLE_PROFILING
#endif  // OPERATION_COUNTER_HPP
