#ifndef UTILS_BASE_HPP
#define UTILS_BASE_HPP

#include "../custom_concepts.hpp"
#include <vector>
#include <string>

template<AcceptedLike T>
struct series_result{
	std::vector<T> Sn;
	std::vector<T> an;
};

template <typename T>
struct TypeWrapper {
    using value_type = T;
};

template<typename T>
struct GetUnderlyingType {
	using value = typename std::conditional_t<isFloatLike<T>::value, TypeWrapper<T>, T>::value_type;
};

struct utils {

	//MATH FUNCTIONS
	template<AcceptedLike T, UnsignedIntLike K> constexpr static const T phi(K n);
	template<UnsignedIntLike K> constexpr static const K fact(const K n);
	template<UnsignedIntLike K> constexpr static const K double_fact(const K n);
	template<UnsignedIntLike K> constexpr static const K binomial_coefficient(const K n, const K k);
	template<AcceptedLike T, UnsignedIntLike K> constexpr static const T minus_one_raised_to_power_n(const K j);
	template<typename T>
	requires AcceptedLike<T> || std::is_integral<T>::value
	static T pow(const T& x, const T& y);
	template<typename T>
	requires AcceptedLike<T> || std::is_integral<T>::value
	static T fma(const T& a, const T& b, const T& c);
	template<AcceptedLike T> static T sqrt(const T& x);
	template<AcceptedLike T> static T exp(const T& x);
	template<AcceptedLike T> static T log(const T& x);
	

	template<AcceptedLike T> static T hypot(const T& a, const T& b);
	template<AcceptedLike T> static T erf(const T& x);
	template<AcceptedLike T> static T zeta(const T& x);

	template<AcceptedLike T> static T ci_x(const T& x);
	template<AcceptedLike T> static T si_x(const T& x);
	template<AcceptedLike T> static T e_x(const T& x);
	template<AcceptedLike T> static T k_x(const T& x);
	template<AcceptedLike T> static T inc_gamma(const T& x, const T& alpha);
	template<AcceptedLike T> static T lambertW0(const T& x);

	template<AcceptedLike T> static T sin(const T& x);
	template<AcceptedLike T> static T asin(const T& x);
	template<AcceptedLike T> static T cos(const T& x);
	template<AcceptedLike T> static T acos(const T& x);
	template<AcceptedLike T> static T tan(const T& x);
	template<AcceptedLike T> static T atan(const T& x);

	template<AcceptedLike T> static T sinh(const T& x);
	template<AcceptedLike T> static T asinh(const T& x);
	template<AcceptedLike T> static T cosh(const T& x);
	template<AcceptedLike T> static T acosh(const T& x);
	template<AcceptedLike T> static T tanh(const T& x);
	template<AcceptedLike T> static T atanh(const T& x);
	
	template<AcceptedLike T>
	static typename GetUnderlyingType<T>::value abs(const T& x);

	//SETTING PRECISION
	#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
	#define SET_PRECISION_SET
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
	template<AcceptedLike T>
	static void set_vec_precision(std::vector<T>& vec, const size_t precision);
	#endif

	//GETTING PRECISION
	#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
	template<AcceptedLike T>
	static size_t get_precision(const T& x);
	#endif

	#ifdef _CL_FLOAT_CLASS_H
	template<AcceptedLike T>
	static cln::float_format_t get_precision(const T& x);
	#endif

	//CASTING
	template<typename To, typename From >
	static To cast(const From& x);

	//HELPER
	template<typename T>
	static std::string to_string(const T& x);

	template<typename T>
	static bool isfinite(const T& x);

	template<typename T>
	static T epsilon(const T& x);

};

#endif