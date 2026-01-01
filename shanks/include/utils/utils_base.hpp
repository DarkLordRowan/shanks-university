#ifndef UTILS_BASE_HPP
#define UTILS_BASE_HPP

#include "../custom_concepts.hpp"
#include <vector>
#include <string>

/**
// * @brief Structure for more convenient passing and receiving data between algos and series
// * @tparam T AcceptedLike Type
*/
template<AcceptedLike T>
struct series_result{
	std::vector<T> Sn; /**< vector of partial sums S_{n} of type T*/
	std::vector<T> an; /**< vector of a_{n} = S_{n} - S_{n-1} */
};

/**
 * @brief Wraps the type in the TypeWrapper struct
 * @tparam T 
 */
template <typename T>
struct TypeWrapper {
    using value_type = T;
};

/**
 * @brief Get the value_type if the type has one, otherwise get the original type
 * @tparam T 
 */
template<typename T>
struct GetUnderlyingType {
	using value = typename std::conditional_t<isFloatLike<T>::value, TypeWrapper<T>, T>::value_type;
};

/**
 * @brief Adapter struct for more convenient integration of third party libraries with some custom integer like type
 *
 * consists of mathematical functions, helpers (isfinite, fma, to_string, ...), functions for acquiring precision if type is able to do it,
 * and setting precision if type is able to do it. If such functionality isn't implemented for given type nothing is done or methods return zero (for now).
*/
struct utils {

	//MATH FUNCTIONS
	/**
	 * @brief Calculates Euler's totient function(функция Эйлера)
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Euler%27s_totient_function
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param n 
	 * @return return result of phi(n) of type T
	 */
	template<AcceptedLike T, UnsignedIntLike K> constexpr static T phi(K n);

	/**
	 * @brief Calculates factorial of a natural number n!
	 * 
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param n 
	 * @return return result of n! of type T
	 */
	template<UnsignedIntLike K> constexpr static K fact(const K n);

	/**
	 * @brief Calculates double factorial of a natural number n!!
	 * 
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param n 
	 * @return return result of n!! type K
	 */
	template<UnsignedIntLike K> constexpr static K double_fact(const K n);

	/**
	 * @brief Calculates binomial coefficient (n, k)
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Binomial_coefficient
	 * @tparam K  UnsginedIntLike (std::integral or types similar to it)
	 * @param n 
	 * @param k 
	 * @return return binomial coefficient of type K
	 */
	template<UnsignedIntLike K> constexpr static K binomial_coefficient(const K n, const K k);

	/**
	 * @brief Returns (-1)^j
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param j 
	 * @return return (-1)^j of type T
	 */
	template<AcceptedLike T, UnsignedIntLike K> constexpr static T minus_one_raised_to_power_n(const K j);

	/**
	 * @brief Calculates x^y
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types) or std::integral
	 * @param x 
	 * @param y 
	 * @return return x^y
	 */
	template<typename T>
	requires AcceptedLike<T> || std::is_integral<T>::value
	static T pow(const T& x, const T& y);

	/**
	 * @brief Calculates a * b + c
	 * 
	 * Uses fma intructions for standard types, or if it is implemented for type T
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types) or std::integral
	 * @param a 
	 * @param b 
	 * @param c 
	 * @return return a * b + c
	 */
	template<typename T>
	requires AcceptedLike<T> || std::is_integral<T>::value
	static T fma(const T& a, const T& b, const T& c);

	/**
	 * @brief Calculates sqrt(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return The square root of a number of type T
	 */
	template<AcceptedLike T> static T sqrt(const T& x);

	/**
	 * @brief Calcualtes exp(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of exp(x) of type T
	 */
	template<AcceptedLike T> static T exp(const T& x);

	/**
	 * @brief Calculates log(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of log(x) of type T
	 */
	template<AcceptedLike T> static T log(const T& x);
	
	/**
	 * @brief Calcualtes sqrt(a^2 + b^2)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param a 
	 * @param b 
	 * @return returns result of hypot(a,b) of type T
	 */
	template<AcceptedLike T> static T hypot(const T& a, const T& b);

	/**
	 * @brief Calculates error function erf(x)
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Error_function
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of erf(x) of type T
	 */
	template<AcceptedLike T> static T erf(const T& x);

	/**
	 * @brief Calculates Riemann zeta function
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Riemann_zeta_function
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of zeta(x) of type T
	 */
	template<AcceptedLike T> static T zeta(const T& x);

	/**
	 * @brief Calculates trigonometric integral of cos
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Trigonometric_integral
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of ci_x(x) of type T
	 */
	template<AcceptedLike T> static T ci_x(const T& x);

	/**
	 * @brief Calculates trigonometric integral of sin
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Trigonometric_integral
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of si_x(x) of type T
	 */
	template<AcceptedLike T> static T si_x(const T& x);

	/** 
	 * @brief Calculates complete elliptical integral of the second kind
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Elliptic_integral
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T e_x(const T& x);

	/** 
	 * @brief Calculates complete elliptical integral of the first kind
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Elliptic_integral
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T k_x(const T& x);

	/**
	 * @brief Calculates lower incomplete gamma function
	 * 
	 * For more info see wiki page https://en.wikipedia.org/wiki/Incomplete_gamma_function
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @param alpha 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T inc_gamma(const T& x, const T& alpha);

	/**
	 * @brief Calculates Lambert's function pricipal branch (W_{0})
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Lambert_W_function
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T lambertW0(const T& x);

	/**
	 * @brief Calcaulates sin(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T sin(const T& x);

	/**
	 * @brief Calculates asin(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T asin(const T& x);

	/**
	 * @brief Calculates cos(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T cos(const T& x);

	/**
	 * @brief Calculates acos(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T acos(const T& x);

	/**
	 * @brief Calculates tan(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T tan(const T& x);

	/**
	 * @brief Calculates atan(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T atan(const T& x);

	/**
	 * @brief Calculates sinh(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T sinh(const T& x);

	/**
	 * @brief Calculates asinh(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T asinh(const T& x);

	/**
	 * @brief Calculates cosh(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T cosh(const T& x);

	/**
	 * @brief Calculates acosh(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T acosh(const T& x);

	/**
	 * @brief Calculates tanh(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T tanh(const T& x);

	/**
	 * @brief Calculates atanh(x)
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return result of type T
	 */
	template<AcceptedLike T> static T atanh(const T& x);


	/**
	 * @brief Calculates absolute value of a variable
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return return type T if it is Real numbers; in case T is complex, return ::value_type
	 */
	template<AcceptedLike T>
	static typename GetUnderlyingType<T>::value abs(const T& x);

	//SETTING PRECISION
	#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
	#define SET_PRECISION_SET
	/**
	 * @brief Sets precision of given variables
	 * 
	 * @tparam Arg type that have arbitrary precision
	 * @tparam Args variadic template with precisable type
	 *  
	 * @param precision
	 */
	template<typename Arg, typename... Args>
	requires (is_precisable<Args>::value && ...)
	static void set_precision(const size_t precision, Arg& precisable_arg, Args& ...precisable_args);
	static void set_precision(const size_t precision) {}
	#endif

	#ifdef _CL_FLOAT_CLASS_H
	#define SET_PRECISION_SET
	template<typename Arg, typename... Args>
	requires (is_precisable<Args>::value && ...)
	static void set_precision(const cln::float_format_t precision, Arg& precisable_arg, Args& ...precisable_args);
	static void set_precision(const cln::float_format_t precision) {}
	#endif

	#ifdef SET_PRECISION_SET
	/**
	 * @brief Set the vec precision object
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param vec 
	 * @param precision 
	 */
	template<AcceptedLike T>
	static void set_vec_precision(std::vector<T>& vec, const size_t precision);
	#endif

	//GETTING PRECISION
	#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
	/**
	 * @brief Get the precision of a variable
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return size_t precision of a given variable if it has one
	 */
	template<AcceptedLike T>
	static size_t get_precision(const T& x);
	#endif

	#ifdef _CL_FLOAT_CLASS_H
	template<AcceptedLike T>
	static cln::float_format_t get_precision(const T& x);
	#endif

	//CASTING
	/**
	 * @brief Wrapper around std::static_cast, has similar functionality
	 * 
	 * Used if custom type has limitations that does not allow usage of std::static_cast with no restrictions
	 * @tparam To 
	 * @tparam From 
	 * @param x 
	 * @return casted to type To value of x
	 */
	template<typename To, typename From >
	constexpr static To cast(const From& x);

	//HELPER
	/**
	 * @brief Similar functionality of std::to_string; wrapper around it
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return std::string 
	 */
	template<typename T>
	static std::string to_string(const T& x);

	/**
	 * @brief Wrapper around std::isfinite, has similar functionality
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return true 
	 * @return false 
	 */
	template<typename T>
	static bool isfinite(const T& x);

	/**
	 * @brief Wrapper around std::numeric_limits<T>::epsilon
	 * 
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @param x 
	 * @return returns the machine epsilon
	 */
	template<typename T>
	static T epsilon(const T& x);

};

#endif