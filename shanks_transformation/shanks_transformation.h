#pragma once
#include "series.h"
#include <vector>

template <typename T>
class shanks_transform : public series_acceleration<T>
{
public:
	shanks_transform();
	shanks_transform(std::function<T(const T, const int)> series, T x);
	~shanks_transform() override;
private:
	T transform(const int n, const int order) const override;
	T epsilon_algorithm(const int n, const int order) const override;
};

template <typename T>
shanks_transform<T>::shanks_transform() : series_acceleration<T>()
{

}

template <typename T>
shanks_transform<T>::shanks_transform(std::function<T(T, int)> series, T x) : series_acceleration<T>(series, x)
{

}

template <typename T>
shanks_transform<T>::~shanks_transform()
{

}

template <typename T>
T shanks_transform<T>::transform(const int n, const int order) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	else if (n == 0 || n < order)
		return 0;
	else if (order == 1)
	{
		const auto a_n = this->series(this->x, n);
		const auto a_n_plus_1 = this->series(this->x, n + 1);
		return this->S_n(n) + a_n * a_n_plus_1 / (a_n - a_n_plus_1);
	}
	else //n > order >= 1
	{
		std::vector<T> T_n(n + order, 0);
		for (int i = n - order + 1; i <= n + order - 1; ++i) // n >= order - see transform method 
		{
			const auto a_n = this->series(this->x, i);
			const auto a_n_plus_1 = this->series(this->x, i + 1);
			T_n[i] = this->S_n(i) + a_n * a_n_plus_1 / (a_n - a_n_plus_1);
		}
		std::vector<T> T_n_plus_1(n + order, 0);
		for (int j = 2; j <= order; ++j)
		{
			for (int i = n - order + j; i <= n + order - j; ++i)
			{
				T_n_plus_1[i] = T_n[i] - (T_n[i] - T_n[i - 1]) * (T_n[i + 1] - T_n[i]) / (T_n[i + 1] - 2 * T_n[i] + T_n[i - 1]);
				T_n = T_n_plus_1;
			}
		}
		return T_n[n];
	}
}

template <typename T>
T shanks_transform<T>::epsilon_algorithm(const int n, const int order) const
{
	// computing eps_(2*order)(S_n) as it is Shanks's transformation e_order(S_n) 
	int m = 2 * order;
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	else if (n == 0)
		return 0;
	else if (order == 0)
		return this->S_n(n);

	std::vector<T> e(m + 1, 0);
	T diff, temp1, temp2;
	e[m] = this->S_n(n + m);
	temp2 = 0.0;

	for (int j = m; j > 0; j--)
	{
		temp1 = temp2;
		temp2 = e[j - 1];
		diff = e[j] - temp2; // TO DO: CHECK IF IT'S NOT ~ZERO
		e[j - 1] = temp1 + 1.0 / diff;
	}

	return e[0];
}