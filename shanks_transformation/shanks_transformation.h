#pragma once
#include "series.h"

template <typename T>
class shanks_transform : public series_acceleration<T>
{
public:
	shanks_transform();
	shanks_transform(std::function<T(T, int)> series, T x);
	~shanks_transform() override;
private:
	T transform(int n) const override;
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
T shanks_transform<T>::transform(int n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n > 0 ? this->S_n(n) + this->series(this->x, n) * 
					this->series(this->x, n + 1) / (this->series(this->x, n) - this->series(this->x, n + 1)) : 0;
}