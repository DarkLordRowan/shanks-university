

template <AcceptedLike T, UnsignedIntLike K>
static T phi(K n);
template <typename K>
    requires UnsignedIntLike<K> || is_standard_types<K>::value
static K fact(const K n);
template <UnsignedIntLike K>
static K double_fact(const K n);
template <UnsignedIntLike K>
static K binomial_coefficient(const K n, const K k);
template <AcceptedLike T, UnsignedIntLike K>
static T minus_one_raised_to_power_n(const K j);
template <typename T>
    requires AcceptedLike<T> || is_standard_types<T>::value
static T pow(const T& x, const T& y);
template <typename T>
static T atan2(const T& y, const T& x);
template <typename T>
    requires AcceptedLike<T> || std::is_integral<T>::value
static T fma(const T& a, const T& b, const T& c);
template <AcceptedLike T>
static T sqrt(const T& x);
template <AcceptedLike T>
static T exp(const T& x);
template <AcceptedLike T>
static T log(const T& x);
template <AcceptedLike T>
static T log10(const T& x);
template <AcceptedLike T>
static T hypot(const T& a, const T& b);
template <AcceptedLike T>
static T erf(const T& x);
template <AcceptedLike T>
static T zeta(const T& x);
template <AcceptedLike T>
static T ci_x(const T& x);
template <AcceptedLike T>
static T si_x(const T& x);
template <AcceptedLike T>
static T e_x(const T& x);
template <AcceptedLike T>
static T k_x(const T& x);
template <AcceptedLike T>
static T inc_gamma(const T& x, const T& alpha);
template <AcceptedLike T>
static T lambertW0(const T& x);
template <AcceptedLike T>
static T sin(const T& x);
template <AcceptedLike T>
static T asin(const T& x);
template <AcceptedLike T>
static T cos(const T& x);
template <AcceptedLike T>
static T acos(const T& x);
template <AcceptedLike T>
static T tan(const T& x);
template <AcceptedLike T>
static T atan(const T& x);
template <AcceptedLike T>
static T sinh(const T& x);
template <AcceptedLike T>
static T asinh(const T& x);
template <AcceptedLike T>
static T cosh(const T& x);
template <AcceptedLike T>
static T acosh(const T& x);
template <AcceptedLike T>
static T tanh(const T& x);
template <AcceptedLike T>
static T atanh(const T& x);
template <typename T>
    requires AcceptedLike<T> || UnsignedIntLike<T> || is_standard_types<T>::value
static typename real_of<T>::value abs(const T& x);
template <FloatLike T>
static T nextafter(const T& x, const T& y);
template <FloatLike T>
static T fmod(const T& x, const T& y);
template <FloatLike T>
static T floor(const T& x);
template <FloatLike T>
static T ceil(const T& x);
template <FloatLike T>
static T get_nan(const T& x);
template <AcceptedLike T>
static size_t get_precision(const T& x);
template <typename To, typename From>
static To cast(const From& x, size_t precision = size_t{0});
template <typename T>
static std::string to_string(const T& x);
template <typename T>
static bool isfinite(const T& x);
template <AcceptedLike T>
static bool isnan(const T& x);
template <typename T>
static bool isinf(const T& x);
template <typename T>
static T epsilon(const T& x);
template <typename T>
static T numeric_max(size_t precision = size_t{0});