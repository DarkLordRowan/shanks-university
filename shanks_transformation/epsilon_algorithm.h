#pragma once
#include "series.h"
#include <vector>

template <typename T>
class epsilon_algorithm : public series_acceleration<T>
{
public:
	epsilon_algorithm();
	epsilon_algorithm(const std::function<T(const T, const int)> &series, const T x);
	~epsilon_algorithm() override;
private:
	T transform(const int n, const int order) const override;
};

template <typename T>
epsilon_algorithm<T>::epsilon_algorithm() : series_acceleration<T>()
{

}

template <typename T>
epsilon_algorithm<T>::epsilon_algorithm(const std::function<T(T, int)> &series, const T x) : series_acceleration<T>(series, x)
{

}

template <typename T>
epsilon_algorithm<T>::~epsilon_algorithm()
{

}

template <typename T>
T epsilon_algorithm<T>::transform(const int n, const int order) const
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