#ifndef OPERATION_COUNTER_HPP
#define OPERATION_COUNTER_HPP

#ifdef SHANKS_ENABLE_PROFILING

namespace shanks {
namespace profiling {

/**
 * @brief Wrapper class that counts operations performed on type T.
 * @tparam T Underlying floating-point type.
 */

// Generic explicit conversion operator
template <typename T>
template <typename U>
OperationCounting<T>::operator U() const {
    return static_cast<U>(value);
}

// Assignment
template <typename T>
OperationCounting<T>& OperationCounting<T>::operator=(const T& v) {
    value = v;
    return *this;
}

// Arithmetic Operators
template <typename T>
OperationCounting<T> OperationCounting<T>::operator+(const OperationCounting<T>& other) const {
    global_operation_counts.add++;
    return OperationCounting(value + other.value);
}
template <typename T>
OperationCounting<T> OperationCounting<T>::operator-(const OperationCounting<T>& other) const {
    global_operation_counts.add++;
    return OperationCounting(value - other.value);
}
template <typename T>
OperationCounting<T> OperationCounting<T>::operator*(const OperationCounting<T>& other) const {
    global_operation_counts.mul++;
    return OperationCounting(value * other.value);
}
template <typename T>
OperationCounting<T> OperationCounting<T>::operator/(const OperationCounting<T>& other) const {
    global_operation_counts.div++;
    return OperationCounting(value / other.value);
}

// Unary Operators
template <typename T>
OperationCounting<T> OperationCounting<T>::operator-() const {
    return OperationCounting<T>(-value);
}

// Compound Assignment
template <typename T>
OperationCounting<T>& OperationCounting<T>::operator+=(const OperationCounting<T>& other) {
    global_operation_counts.add++;
    value += other.value;
    return *this;
}
template <typename T>
OperationCounting<T>& OperationCounting<T>::operator-=(const OperationCounting<T>& other) {
    global_operation_counts.add++;
    value -= other.value;
    return *this;
}
template <typename T>
OperationCounting<T>& OperationCounting<T>::operator*=(const OperationCounting<T>& other) {
    global_operation_counts.mul++;
    value *= other.value;
    return *this;
}
template <typename T>
OperationCounting<T>& OperationCounting<T>::operator/=(const OperationCounting<T>& other) {
    global_operation_counts.div++;
    value /= other.value;
    return *this;
}

// Comparison (No op count)
template <typename T>
auto OperationCounting<T>::operator<=>(const OperationCounting<T>& other) const = default;  // C++20
template <typename T>
bool OperationCounting<T>::operator==(const OperationCounting<T>& other) const {
    return value == other.value;
}
template <typename T>
bool OperationCounting<T>::operator!=(const OperationCounting<T>& other) const {
    return value != other.value;
}
template <typename T>
bool OperationCounting<T>::operator<(const OperationCounting<T>& other) const {
    return value < other.value;
}
template <typename T>
bool OperationCounting<T>::operator>(const OperationCounting<T>& other) const {
    return value > other.value;
}
template <typename T>
bool OperationCounting<T>::operator<=(const OperationCounting<T>& other) const {
    return value <= other.value;
}
template <typename T>
bool OperationCounting<T>::operator>=(const OperationCounting<T>& other) const {
    return value >= other.value;
}

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

template <typename T>
std::istream& operator>>(std::istream& fin, OperationCounting<T>& x) {
    fin >> x.value;
    return fin;
}

template <typename T>
std::ostream& operator<<(std::ostream& fout, const OperationCounting<T>& x) {
    fout << utils::helpers<T>::to_string(x.value);
    return fout;
}

template <typename T>
OperationCounting<T> abs(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::abs(v);
}

template <typename T>
OperationCounting<T> sqrt(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::sqrt(v);
}

template <typename T>
OperationCounting<T> pow(const OperationCounting<T>& b, const OperationCounting<T>& e) {
    return utils::math<OperationCounting<T>>::pow(b, e);
}

template <typename T>
OperationCounting<T> atan2(const OperationCounting<T>& y, const OperationCounting<T>& x) {
    return utils::math<OperationCounting<T>>::atan2(x, y);
}

template <typename T>
OperationCounting<T> exp(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::exp(v);
}

template <typename T>
OperationCounting<T> log(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::log(v);
}

template <typename T>
OperationCounting<T> sin(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::sin(v);
}

template <typename T>
OperationCounting<T> cos(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::cos(v);
}

template <typename T>
OperationCounting<T> tan(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::tan(v);
}

template <typename T>
OperationCounting<T> asin(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::asin(v);
}

template <typename T>
OperationCounting<T> acos(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::acos(v);
}

template <typename T>
OperationCounting<T> atan(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::atan(v);
}

template <typename T>
OperationCounting<T> sinh(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::sinh(v);
}

template <typename T>
OperationCounting<T> cosh(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::cosh(v);
}

template <typename T>
OperationCounting<T> tanh(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::tanh(v);
}

template <typename T>
OperationCounting<T> asinh(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::asinh(v);
}

template <typename T>
OperationCounting<T> acosh(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::acosh(v);
}

template <typename T>
OperationCounting<T> atanh(const OperationCounting<T>& v) {
    return utils::math<OperationCounting<T>>::atanh(v);
}

}  // namespace profiling
}  // namespace shanks

#endif  // SHANKS_ENABLE_PROFILING
#endif  // OPERATION_COUNTER_HPP
