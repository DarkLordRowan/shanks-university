#pragma once
#define NO_X_GIVEN 0
#define NO_SERIES_EXPRESSION_GIVEN 0 

template <typename T, typename K>
class series_base
{
public:
	series_base();
	series_base(T x);
	~series_base();
	constexpr virtual T S_n(const K n) const;
	constexpr virtual T a_n(const K n) const;
	constexpr const T get_x() const;
protected:
	series_base(T x, T sum);
	const T x;
	const T sum;
};

template <typename T, typename K>
series_base<T, K>::series_base() : x(NO_X_GIVEN), sum(0)
{
	
}

template <typename T, typename K>
series_base<T, K>::series_base(T x) : x(x), sum(0)
{

}

template <typename T, typename K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum)
{

}

template <typename T, typename K>
constexpr T series_base<T, K>::S_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	T sum = a_n(n);
	for (int i = 0; i < n; ++i)
		sum += a_n(i);
	return sum;
}

template <typename T, typename K>
constexpr T series_base<T, K>::a_n(const K n) const
{
	return NO_SERIES_EXPRESSION_GIVEN;
}

template <typename T, typename K>
constexpr const T series_base<T, K>::get_x() const
{
	return x;
}

template <typename T, typename K>
series_base<T, K>::~series_base()
{

}

template <typename T, typename K>
class exp_series : public series_base<T, K>
{
public:
	exp_series();
	exp_series(T x);
	~exp_series();
	constexpr virtual T a_n(const K n) const;
private:
	const int fact(const K n) const;
};

template <typename T, typename K>
exp_series<T, K>::exp_series() : series_base<T, K>()
{

}

template <typename T, typename K>
exp_series<T, K>::exp_series(T x) : series_base<T,K>(x, std::exp(x))
{
}

template <typename T, typename K>
exp_series<T, K>::~exp_series()
{

}

template <typename T, typename K>
constexpr T exp_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return std::pow(this->x, n) / this->fact(n);
	for (int i = 0; i < 5; ++i)
		std::cout << "DALE COOPER" << std::endl;
}

template <typename T, typename K>
const int exp_series<T, K>::fact(const K n) const
{
	/*if (n < 0)
		throw std::domain_error("negative integer in the input");*/ //it's impossible for n to be <0 since this method is only used in 
																	//constexpr T exp_series<T>::a_n(const int n) where there is a verification of nonegativity of n
	int f = 1;
	for (int i = 2; i <= n; i++)
		f *= i;
	return f;
}