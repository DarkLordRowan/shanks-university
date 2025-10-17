#pragma once

#include "custom_concepts.hpp"
#include <numbers>

template<AcceptedLike T>
struct series_result{
	std::vector<T> Sn;
	std::vector<T> an;
};

template <AcceptedLike T, UnsignedIntLike K>
constexpr const T phi(K n)
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

template<UnsignedIntLike K>
constexpr const K fact(const K n) {
	K fact = static_cast<K>(1);
	for(K j = static_cast<K>(2); j <= n; ++j){
		fact *= j;
	}
	return fact;
}

template<UnsignedIntLike K>
constexpr const K double_fact(const K n) {

	K double_fact = static_cast<K>(1);

	for (K j = n & static_cast<K>(1) + static_cast<K>(2); j <= n; j+=2){
		double_fact *= j;
	}

	return double_fact;
}

template<UnsignedIntLike K>
constexpr const K binomial_coefficient(const K n, const K k) {

	if(n<k)
		throw std::invalid_argument("n>k");

	if (n==k || k==static_cast<K>(0))
		return static_cast<K>(1);

	K productNminusK = n; //n(n-1)(n-2)...(n-k)
	K factK = k;

	for(K j = static_cast<K>(1); j < k; j++){
		factK*=j;
		productNminusK*=(n-j);
	}

	return productNminusK/factK;

}

template<AcceptedLike T, UnsignedIntLike K>
T minus_one_raised_to_power_n(K j){
    return static_cast<T>(j & 1 ? -1 : 1);
}