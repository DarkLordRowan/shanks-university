#pragma once
#define DEF_NO_TRANSFORM 0

#include <functional>
#include <iostream>
#include <exception>
#include <math.h>

template <typename T>
class series_acceleration
{
public:
	series_acceleration();
	series_acceleration(const std::function<T(const T, const int)> &series, const T x);
	virtual ~series_acceleration() = 0;
	constexpr void print_s_n(const int n) const;
	constexpr void print_t_n(const int n, const int order) const;
	constexpr void print_diff_t_s(const int n, const int order) const;
protected:
	virtual T transform(const int n, const int order) const;
	std::function<T(const T, const int)> series;
	T x;
	constexpr T S_n(int n) const;
};

template <typename T>
series_acceleration<T>::series_acceleration()
{
	series = [](const T x, const int n) -> T {return 0; };
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
	std::cout << "S_" << n << " : " << S_n(n) << std::endl;
}

template <typename T>
constexpr void series_acceleration<T>::print_t_n(const int n, const int order) const
{
	std::cout << "T_" << n << " of order " << order << " : " << transform(n, order) << std::endl; 
}

template <typename T>
constexpr void series_acceleration<T>::print_diff_t_s(const int n, const int order) const
{
	std::cout << "T_" << n << " of order " << order << " - S_" << n
		<< " : " << transform(n, order) - S_n(n) << std::endl;	//to do : ostream instead just cout
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