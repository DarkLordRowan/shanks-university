#pragma once
#define DEF_UNDEFINED_SUM 0

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
	else if (order == 0)
		return this->S_n(n);
	else if (n == 0 || n < order)
		return DEF_UNDEFINED_SUM;
	else if (order == 1)
	{
		const auto a_n = this->series(this->x, n);
		const auto a_n_plus_1 = this->series(this->x, n + 1);
		if (isnan(abs(a_n - a_n_plus_1)))
			throw std::overflow_error("division by zero");
		return std::fma(a_n * a_n_plus_1, 1 / (a_n - a_n_plus_1), this->S_n(n));
	}
	else //n > order >= 1
	{
		std::vector<T> T_n(n + order, 0);
		for (int i = n - order + 1; i <= n + order - 1; ++i) // n >= order - see transform method 
		{
			const auto a_n = this->series(this->x, i);
			const auto a_n_plus_1 = this->series(this->x, i + 1);
			if (isnan(abs(a_n - a_n_plus_1)))
				throw std::overflow_error("division by zero");
			T_n[i] = std::fma(a_n * a_n_plus_1, 1 / (a_n - a_n_plus_1), this->S_n(i));
		}
		std::vector<T> T_n_plus_1(n + order, 0);
		T a, b, c;
		for (int j = 2; j <= order; ++j)
		{
			for (int i = n - order + j; i <= n + order - j; ++i)
			{
				a = T_n[i];
				b = T_n[i - 1];
				c = T_n[i + 1];
				//T_n_plus_1[i] = T_n[i] - (T_n[i] - T_n[i - 1]) * (T_n[i + 1] - T_n[i]) / (T_n[i + 1] - 2 * T_n[i] + T_n[i - 1]);
				/*if (isnan(abs(2 * T_n[i] - T_n[i - 1] - T_n[i + 1])))
					throw std::overflow_error("division by zero");
				T_n_plus_1[i] = std::fma(std::fma(T_n[i], T_n[i+1] + T_n[i-1] - T_n[i], -T_n[i-1]*T_n[i+1]), 1 / (2 * T_n[i] - T_n[i - 1] - T_n[i+1]), T_n[i]);*/

				if (isnan(abs(2 * a - b - c)))
					throw std::overflow_error("division by zero");
				T_n_plus_1[i] = std::fma(std::fma(a, c + b - a, -b * c), 1 / (2 * a - b - c), a);
				T_n = T_n_plus_1;
			}
		}
		return T_n[n];
	}
}