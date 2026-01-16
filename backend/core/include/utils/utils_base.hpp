#ifndef UTILS_BASE_HPP
#define UTILS_BASE_HPP

#include "../custom_concepts.hpp"
#include <vector>
#include <string>

/**
 * @file utils_base.hpp
 * @brief This file contains the base utility definitions and the main utils struct.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Structure for more convenient passing and receiving data between algos and series
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T AcceptedLike Type
*/
template<AcceptedLike T>
struct series_result{
	std::vector<T> Sn; /**< vector of partial sums S_{n} of type T*/
	std::vector<T> an; /**< vector of a_{n} = S_{n} - S_{n-1} */
};


/**
 * @brief Get the value_type if the type has one, otherwise get the original type
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Input type
 */
template<typename T>
struct GetUnderlyingType { using value = T;};

template<typename T>
struct GetUnderlyingType<std::complex<T>>{ using value = T; };

/**
 * @brief Adapter struct for more convenient integration of third party libraries with some custom integer like type
 *
 * consists of mathematical functions, helpers (isfinite, fma, to_string, ...), functions for acquiring precision if type is able to do it,
 * and setting precision if type is able to do it. If such functionality isn't implemented for given type nothing is done or methods return zero (for now).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
*/
struct utils {

	//MATH FUNCTIONS
	/**
	 * @brief Calculates Euler's totient function
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Euler%27s_totient_function
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param n
	 * @return return result of phi(n) of type T
	 */
	template<AcceptedLike T, UnsignedIntLike K> constexpr static T phi(K n);

	/**
	 * @brief Calculates factorial of a natural number n!
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param n
	 * @return return result of n! of type T
	 */
	template<UnsignedIntLike K> constexpr static K fact(const K n);

	/**
	 * @brief Calculates double factorial of a natural number n!!
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param n
	 * @return return result of n!! type K
	 */
	template<UnsignedIntLike K> constexpr static K double_fact(const K n);

	/**
	 * @brief Calculates binomial coefficient (n, k)
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Binomial_coefficient
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @tparam K  UnsginedIntLike (std::integral or types similar to it)
	 * @param n
	 * @param k
	 * @return return binomial coefficient of type K
	 */
	template<UnsignedIntLike K> constexpr static K binomial_coefficient(const K n, const K k);

	/**
	 * @brief Returns (-1)^j
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types)
	 * @tparam K UnsginedIntLike (std::integral or types similar to it)
	 * @param j
	 * @return return (-1)^j of type T
	 */
	template<AcceptedLike T, UnsignedIntLike K> constexpr static T minus_one_raised_to_power_n(const K j);

	/**
	 * @brief Calculates x^y
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types) or std::integral
	 * @param x (T), y (T)
	 * @return T (result of power)
	 */
	template<typename T>
	requires AcceptedLike<T> || std::is_integral<T>::value
	static T pow(const T& x, const T& y);

	/**
	 * @brief Calculates atan2(y, x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 */
	template<typename T>
	static T atan2(const T& y, const T& x);

	/**
	 * @brief Calculates a * b + c
	 *
	 * Uses fma intructions for standard types, or if it is implemented for type T
	 * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex types) or std::integral
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param a (T), b (T), c (T)
	 * @return T a * b + c
	 */
	template<typename T>
	requires AcceptedLike<T> || std::is_integral<T>::value
	static T fma(const T& a, const T& b, const T& c);

	/**
	 * @brief Calculates sqrt(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (square root)
	 */
	template<AcceptedLike T> static T sqrt(const T& x);

	/**
	 * @brief Calcualtes exp(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (exponent)
	 */
	template<AcceptedLike T> static T exp(const T& x);

	/**
	 * @brief Calculates log(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (natural logarithm)
	 */
	template<AcceptedLike T> static T log(const T& x);

	/**
	 * @brief Calcualtes sqrt(a^2 + b^2)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param a (T), b (T)
	 * @return T (hypotenuse)
	 */
	template<AcceptedLike T> static T hypot(const T& a, const T& b);

	/**
	 * @brief Calculates error function erf(x)
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Error_function
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (erf result)
	 */
	template<AcceptedLike T> static T erf(const T& x);

	/**
	 * @brief Calculates Riemann zeta function
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Riemann_zeta_function
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (zeta result)
	 */
	template<AcceptedLike T> static T zeta(const T& x);

	/**
	 * @brief Calculates trigonometric integral of cos
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Trigonometric_integral
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (Ci(x) result)
	 */
	template<AcceptedLike T> static T ci_x(const T& x);

	/**
	 * @brief Calculates trigonometric integral of sin
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Trigonometric_integral
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (Si(x) result)
	 */
	template<AcceptedLike T> static T si_x(const T& x);

	/**
	 * @brief Calculates complete elliptical integral of the second kind
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Elliptic_integral
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (E(x) result)
	 */
	template<AcceptedLike T> static T e_x(const T& x);

	/**
	 * @brief Calculates complete elliptical integral of the first kind
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Elliptic_integral
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (K(x) result)
	 */
	template<AcceptedLike T> static T k_x(const T& x);

	/**
	 * @brief Calculates lower incomplete gamma function
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Incomplete_gamma_function
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T), alpha (T)
	 * @return T (gamma result)
	 */
	template<AcceptedLike T> static T inc_gamma(const T& x, const T& alpha);

	/**
	 * @brief Calculates Lambert's function pricipal branch (W_{0})
	 *
	 * For more info see wiki page https://en.wikipedia.org/wiki/Lambert_W_function
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (W0 result)
	 */
	template<AcceptedLike T> static T lambertW0(const T& x);

	/**
	 * @brief Calcaulates sin(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (sine result)
	 */
	template<AcceptedLike T> static T sin(const T& x);

	/**
	 * @brief Calculates asin(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (arcsine result)
	 */
	template<AcceptedLike T> static T asin(const T& x);

	/**
	 * @brief Calculates cos(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (cosine result)
	 */
	template<AcceptedLike T> static T cos(const T& x);

	/**
	 * @brief Calculates acos(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (arccosine result)
	 */
	template<AcceptedLike T> static T acos(const T& x);

	/**
	 * @brief Calculates tan(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (tangent result)
	 */
	template<AcceptedLike T> static T tan(const T& x);

	/**
	 * @brief Calculates atan(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (arctangent result)
	 */
	template<AcceptedLike T> static T atan(const T& x);

	/**
	 * @brief Calculates sinh(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (hyperbolic sine result)
	 */
	template<AcceptedLike T> static T sinh(const T& x);

	/**
	 * @brief Calculates asinh(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (hyperbolic arcsine result)
	 */
	template<AcceptedLike T> static T asinh(const T& x);

	/**
	 * @brief Calculates cosh(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (hyperbolic cosine result)
	 */
	template<AcceptedLike T> static T cosh(const T& x);

	/**
	 * @brief Calculates acosh(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (hyperbolic arccosine result)
	 */
	template<AcceptedLike T> static T acosh(const T& x);

	/**
	 * @brief Calculates tanh(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (hyperbolic tangent result)
	 */
	template<AcceptedLike T> static T tanh(const T& x);

	/**
	 * @brief Calculates atanh(x)
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (hyperbolic arctangent result)
	 */
	template<AcceptedLike T> static T atanh(const T& x);


	/**
	 * @brief Calculates absolute value of a variable
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return Underlying type value (absolute value)
	 */
	template<AcceptedLike T>
	static typename GetUnderlyingType<T>::value abs(const T& x);

	//SETTING PRECISION
	#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
	#define SET_PRECISION_SET
	/**
	 * @brief Sets precision of given variables
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param precision (size_t), precisable_arg (Arg), precisable_args (Args)
	 */
	template<typename Arg, typename... Args>
	requires (is_precisable<Args>::value && ...)
	static void set_precision(const size_t precision, Arg& precisable_arg, Args& ...precisable_args);
	static void set_precision(const size_t precision) {}
	#endif

	#ifdef _CL_FLOAT_CLASS_H
	#define SET_PRECISION_SET
	/**
	 * @brief Sets precision of given variables for CLN types
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param precision (cln::float_format_t), precisable_arg (Arg), precisable_args (Args)
	 */
	template<typename Arg, typename... Args>
	requires (is_precisable<Args>::value && ...)
	static void set_precision(const cln::float_format_t precision, Arg& precisable_arg, Args& ...precisable_args);
	static void set_precision(const cln::float_format_t precision) {}
	#endif

	#ifdef SET_PRECISION_SET
	/**
	 * @brief Set the vector precision
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param vec (std::vector<T>), precision (size_t)
	 */
	template<AcceptedLike T>
	static void set_vec_precision(std::vector<T>& vec, const size_t precision);
	#endif

	//GETTING PRECISION
	#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
	/**
	 * @brief Get the precision of a variable
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return size_t precision
	 */
	template<AcceptedLike T>
	static size_t get_precision(const T& x);
	#endif

	#ifdef _CL_FLOAT_CLASS_H
	/**
	 * @brief Get the precision of a variable for CLN types
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return cln::float_format_t precision
	 */
	template<AcceptedLike T>
	static cln::float_format_t get_precision(const T& x);
	#endif

	//CASTING
	/**
	 * @brief Wrapper around std::static_cast, has similar functionality
	 *
	 * Used if custom type has limitations that does not allow usage of std::static_cast with no restrictions
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (From)
	 * @return To (casted value)
	 */
	template<typename To, typename From >
	constexpr static To cast(const From& x);

	//HELPER
	/**
	 * @brief Similar functionality of std::to_string; wrapper around it
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return std::string representation
	 */
	template<typename T>
	static std::string to_string(const T& x);

	/**
	 * @brief Wrapper around std::isfinite, has similar functionality
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return bool (is finite)
	 */
	template<typename T>
	static bool isfinite(const T& x);

	/**
	 * @brief Wrapper around std::numeric_limits<T>::epsilon
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x (T)
	 * @return T (epsilon value)
	 */
	template<typename T>
	static T epsilon(const T& x);

};

#endif
