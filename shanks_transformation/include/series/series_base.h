
 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
#pragma once
#include <limits>
#include <stdexcept>

 template <typename T, typename K>
class series_base
{
public:
	virtual ~series_base() = default;

	/**
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	explicit series_base(T x = 0);

	/**
	* @brief Computes partial sum of the first n terms
	* @authors Bolshakov M.P.
	* @param n The amount of terms in the partial sum
	* @return Partial sum of the first n terms
	*/
	[[nodiscard]] constexpr T S_n(K n) const;

	/**
	* @brief Computes nth term of the series
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] virtual T operator()(K n) const = 0;

	/**
	* @brief x getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr T get_x() const;

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr T get_sum() const;

	/**
	* @brief factorial n!
	* @authors Bolshakov M.P.
	* @return n!
	*/
	[[nodiscard]] constexpr static K fact(K n);

	/**
	* @brief binomial coefficient C^n_k
	* @authors Bolshakov M.P.
	* @return combinations(n,k)
	*/
	[[nodiscard]] constexpr static T binomial_coefficient(T n, K k);


	/**
	* @brief evaluates (-1)^n
	* @authors Bolshakov M.P.
	* @return (-1)^n
	*/
	[[nodiscard]] constexpr static T minus_one_raised_to_power_n(K n);


	/**
	* @brief evaluates Euler function by n
	* @authors Trudolyubov N.A.
	* @return phi(n)
	*/
	[[nodiscard]] constexpr static T phi(K n);

protected:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum of the series
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	* @param sum The sum of the series
	*/
	series_base(T x, T sum);

	/**
	* @brief function series argument
	* It's set to 0 by default
	* @authors Bolshakov M.P.
	*/
	const T x;

	/**
	* @brief sum of the series
	* It's set to 0 by default
	* @authors Bolshakov M.P.
	*/
	const T sum;
};

template <typename T, typename K>
series_base<T, K>::series_base(T x) : x(x), sum(0)
{
	static_assert(std::is_floating_point_v<T>);
	static_assert(std::numeric_limits<K>::is_integer);
}

template <typename T, typename K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum)
{
	static_assert(std::is_floating_point_v<T>);
	static_assert(std::numeric_limits<K>::is_integer);
}

template <typename T, typename K>
constexpr T series_base<T, K>::S_n(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	T sum = operator()(n);
	for (K i = 0; i < n; ++i)
		sum += operator()(i);
	return sum;
}

template <typename T, typename K>
constexpr T series_base<T, K>::get_x() const
{
	return x;
}

template <typename T, typename K>
constexpr T series_base<T, K>::get_sum() const {
	return sum;
}

template <typename T, typename K>
constexpr K series_base<T, K>::fact(K n) {
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	K f = 1;
	for (K i = 2; i <= n; ++i)
		f *= i;
	return f;
}

template <typename T, typename K>
constexpr T series_base<T, K>::binomial_coefficient(const T n, const K k)
{
	T b_c = 1;
	for (K i = 0; i < k; ++i)
		b_c = b_c * (n - static_cast<T>(i)) / (i + 1);
	return b_c;
}

template <typename T, typename K>
constexpr T series_base<T, K>::minus_one_raised_to_power_n(K n)
{
	return static_cast<T>(n % 2 ? -1.0 : 1.0);
}


template <typename T, typename K>
constexpr T series_base<T, K>::phi(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	K result = n;
	for (K i = 2; i * i <= n; ++i)
		if (n % i == 0) {
			while (n % i == 0)
				n /= i;
			result -= result / i;
		}
	if (n > 1)
		result -= result / n;
	return static_cast<T>(result);
}
