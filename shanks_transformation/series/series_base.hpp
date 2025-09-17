#pragma once

#include <math.h>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <concepts>
#include <string>
#include <numbers>

#include "../custom_concepts.hpp"

using std::isfinite;


 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <Accepted T, std::unsigned_integral K>
class series_base
{
public:
	virtual ~series_base() = default;

	/**
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/

	series_base(T x = 0);

	/**
    * @brief Throws domain error with unified message format
    * @authors Maximov A.K.
    * @param condition Description of the divergence condition
    * @throws std::domain_error with formatted message containing series name, x value and condition
    */
	void throw_domain_error(const std::string& condition) const {
		throw std::domain_error(series_name + " series diverges at x = " + to_string(x) + " (" + condition + ")");
	}

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
	[[nodiscard]] constexpr virtual T operator()(K n) const = 0;

	/**
	* @brief x getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_x() const;

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_sum() const;

	/**
	* @brief factorial k!
	* @authors Bolshakov M.P.
	* @return k!
	*/
	[[nodiscard]] constexpr static const K fact(K k);

	/**
	* @brief double factorial k!!
	* @authors Bolshakov M.P.
	* @return k!!
	*/
	[[nodiscard]] constexpr static const K double_fact(K k);

	/**
	* @brief binomial coefficient C^n_k
	* @authors Bolshakov M.P.
	* @return combinations(n,k)
	*/
	[[nodiscard]] constexpr static const T binomial_coefficient(const T n, const K k);


	/**
	* @brief evaluates (-1)^n
	* @authors Bolshakov M.P.
	* @return (-1)^n
	*/
	[[nodiscard]] constexpr static const T minus_one_raised_to_power_n(K n);


	/**
	* @brief evaluates Euler function by n
	* @authors Trudolyubov N.A.
	* @return phi(n)
	*/
	[[nodiscard]] constexpr static const T phi(K n);

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

	/**
    * @brief Name of the series for unified error messages
    * Used to generate consistent error output format
    * @authors Maximov A.K.
    */
	std::string series_name;
};

template <Accepted T, std::unsigned_integral K>
series_base<T, K>::series_base(T x) : x(x), sum(0), series_name("unknown") {}

template <Accepted T, std::unsigned_integral K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum), series_name("unknown") {}

template <Accepted T, std::unsigned_integral K>
constexpr T series_base<T, K>::S_n(K n) const
{
	T sum = operator()(n);
	for (K i = 0; i < n; ++i)
		sum += operator()(i);
	return sum;
}

template <Accepted T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_x() const
{
	return x;
}

template <Accepted T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_sum() const
{
	return sum;
}

template <Accepted T, std::unsigned_integral K>
constexpr const K series_base<T, K>::fact(K n)
{
	K f = 1;
	for (K i = 2; i <= n; ++i)
		f *= i;
	return f;
}

template <Accepted T, std::unsigned_integral K>
constexpr const K series_base<T, K>::double_fact(K n)
{
	if (n == 0 || n == 1)
		return 1;
	return n * series_base<T, K>::double_fact(n - 2);
}

template <Accepted T, std::unsigned_integral K>
constexpr const T series_base<T, K>::binomial_coefficient(const T n, const K k)
{
	T b_c = 1;
	for (K i = 0; i < k; ++i)
		b_c *= (n - static_cast<T>(i)) / static_cast<T>(i + 1);
	return b_c;
}

template <Accepted T, std::unsigned_integral K>
constexpr const T series_base<T, K>::minus_one_raised_to_power_n(K n)
{
	return static_cast<T>(n & 1 ? -1.0 : 1.0);
}

template <Accepted T, std::unsigned_integral K>
constexpr const T series_base<T, K>::phi(K n)
{
	K result = n;
	for (K i = 2; i * i <= n; ++i)
		if (n % i == 0) {
			while (n % i == 0)
				n /= i;
			result -= result / i;
		}

	result -= n > 1 ? result / n : 0;
	return static_cast<T>(result);
}


/**
* @brief Abstract class for recurrent series
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class recurrent_series_base
{
public:
	/**
	* @brief Parameterized constructor to initialize the recurrent series with function argument
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	recurrent_series_base(T x);

	/**
	* @brief Parameterized constructor to initialize the recurrent series with vector, containing elements of series
	* @authors Kreynin R.G.
	* @param row The first elements of the series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	recurrent_series_base(std::vector<T> row);

	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	virtual T access_row(K n) = 0;

	/**
	* @brief Vector, containing elements of the series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* It's empty by default
	* @authors Kreynin R.G.
	*/
	std::vector<T> series_vector;

protected:
	/**
	* @brief Throws domain error with unified message format
	* @param condition Description of the divergence condition
	* @throws std::domain_error with formatted message
	* @authors Maximov A.K.
	*/
	void throw_domain_error(const std::string& condition) const {
		throw std::domain_error("recurrent series diverges (" + condition + ")");
	}
};

template <Accepted T, std::unsigned_integral K>
recurrent_series_base<T, K>::recurrent_series_base(T x)
{
	this->series_vector.push_back(x);
};


template <Accepted T, std::unsigned_integral K>
recurrent_series_base<T, K>::recurrent_series_base(std::vector<T> row) : series_vector(std::move(row))
{
	if (this->series_vector.empty()) {
		this->throw_domain_error("empty initial row");
	}
};
