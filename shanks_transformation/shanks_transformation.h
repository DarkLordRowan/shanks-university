#pragma once
#include "series.h"
#include <vector>

template <typename T>
class shanks_transform : public series_acceleration<T>
{
public:
	shanks_transform();
	shanks_transform(const std::function<T(const T, const int)> &series, const T x);
	~shanks_transform() override;
private:
	T transform(const int n, const int order) const override;
};

template <typename T>
shanks_transform<T>::shanks_transform() : series_acceleration<T>()
{

}

template <typename T>
shanks_transform<T>::shanks_transform(const std::function<T(T, int)> &series, const T x) : series_acceleration<T>(series, x)
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