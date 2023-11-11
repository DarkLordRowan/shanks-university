#pragma once
#define DEF_NO_TRANSFORM 0
#define NO_SERIES_GIVEN 0

#include <functional>
#include <iostream>
#include <exception>
#include <math.h>

// Base class series_acceleration
// It is not used on it's own, bit it is inherited by shanks_transformation and epsilon_algorithm to implement the corresponding methods.
// The class implementation provides everything needed for construction of an arbitrary series up to n terms and printing out the partial sum, 
//	the partial sum after transformation is used and the difference between the latter and the former.
template <typename T>
class series_acceleration
{
public:
	series_acceleration();

	/*constructor that receives:
		-series function which return the nth term of the given series at point x
		-point x*/
	series_acceleration(const std::function<T(const T, const int)> &series, const T x);

	virtual ~series_acceleration() = 0;

	/*methods for printing out certain quantities of the given series*/
	constexpr void print_s_n(const int n) const;
	constexpr void print_s_n(const int n, std::ostream& out) const;
	constexpr void print_t_n(const int n, const int order) const;
	constexpr void print_t_n(const int n, const int order, std::ostream& out) const;
	constexpr void print_diff_t_s(const int n, const int order) const;
	constexpr void print_diff_t_s(const int n, const int order, std::ostream& out) const;
protected:
	/*virtual method that returns the partial sum after transformation of the series*/
	virtual T transform(const int n, const int order) const;

	/*series function which return the nth term of the given series at point x*/
	std::function<T(const T, const int)> series;

	/*point x*/
	T x;

	/*partial sum of the series*/
	constexpr T S_n(int n) const;
};

template <typename T>
series_acceleration<T>::series_acceleration()
{
	series = [](const T x, const int n) -> T {return NO_SERIES_GIVEN; };
}

template <typename T>
series_acceleration<T>::series_acceleration(const std::function<T(const T, const int)>& series, const T x) : x(x), series(series)
{

}

template <typename T>
series_acceleration<T>::~series_acceleration()
{

}

template <typename T>
constexpr void series_acceleration<T>::print_s_n(const int n) const
{
	print_s_n(n, std::cout);
}

template <typename T>
constexpr void series_acceleration<T>::print_s_n(const int n, std::ostream& out) const
{
	out << "S_" << n << " : " << S_n(n) << std::endl;
}

template <typename T>
constexpr void series_acceleration<T>::print_t_n(const int n, const int order) const
{
	print_t_n(n, order, std::cout);
}

template <typename T>
constexpr void series_acceleration<T>::print_t_n(const int n, const int order, std::ostream& out) const
{
	out << "T_" << n << " of order " << order << " : " << transform(n, order) << std::endl;
}

template <typename T>
constexpr void series_acceleration<T>::print_diff_t_s(const int n, const int order) const
{
	print_diff_t_s(n, order, std::cout);
}

template <typename T>
constexpr void series_acceleration<T>::print_diff_t_s(const int n, const int order, std::ostream& out) const
{
	out << "T_" << n << " of order " << order << " - S_" << n
		<< " : " << transform(n, order) - S_n(n) << std::endl;
}

template <typename T>
constexpr T series_acceleration<T>::S_n(const int n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	T s_n = 0;
	for (int i = n; i >= 0; --i)
		s_n += series(x, i);
	return s_n;
}

template <typename T>
T series_acceleration<T>::transform(const int n, const int order) const
{
	return DEF_NO_TRANSFORM;
}