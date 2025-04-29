
/**
* @brief Maclaurin series of m! / (1 - x) ^ (m + 1)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>
#include <vector>

#include "../series_base.h"
#include "../recurrent_series_base.h"


template <typename T, typename K>
class m_fact_1mx_mp1_inverse_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	m_fact_1mx_mp1_inverse_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series, m The integer constant
	* @param m
	*/
	m_fact_1mx_mp1_inverse_series(T x, K m);

	/**
	* @brief Computes the nth term of the Maclaurin series of  m! / (1 - x) ^ (m + 1)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] T operator()(K n) const override;
private:

	/**
	* @brief The parameter m of the series
	* @authors Pashkov B.B.
	*/
	const K m;

	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the series
	*/
	T acsess_row(K n) override;
};

template <typename T, typename K>
m_fact_1mx_mp1_inverse_series<T, K>::m_fact_1mx_mp1_inverse_series(T x, K m) : series_base<T, K>(x, static_cast<T>(static_cast<T>(this->fact(m)) / pow(1 - x, m + 1))), recurrent_series_base<T, K>(static_cast<T>(this->fact(m))), m(m)
{
	if (!std::isfinite(series_base<T, K>::sum)) // sum = this->fact(m) / pow(1 - x, m + 1))
		throw std::overflow_error("sum is too big");
	if (std::abs(this->x) >= 1) // p. 564 typo
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
T m_fact_1mx_mp1_inverse_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * this->x * (this->m + i) / i);
	}

	return this->series_vector[n];
}

template <typename T, typename K>
T m_fact_1mx_mp1_inverse_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<m_fact_1mx_mp1_inverse_series*>(this)->acsess_row(n);
	return a;
}
