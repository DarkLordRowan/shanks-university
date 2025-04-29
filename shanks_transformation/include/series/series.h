#pragma once
#define NO_X_GIVEN 0
#define NO_SERIES_EXPRESSION_GIVEN 0
#include <limits>
#include <vector>
#include <math.h>
#include <stdexcept>
#include <string>



/**
* @brief Taylor series of number pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class pi_series : public series_base<T, K>
{
public:
	pi_series();

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
pi_series<T, K>::pi_series() : series_base<T, K>(0) {}

template <typename T, typename K>
constexpr T pi_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return static_cast<T>(std::sqrt(12) * std::pow(-3, -n) / (2 * n + 1));
}


/**
* @brief Taylor series of function x - sqrt(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_min_sqrt_x_series : public series_base<T, K>
{
public:
	x_min_sqrt_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	*/
	x_min_sqrt_x_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
x_min_sqrt_x_series<T, K>::x_min_sqrt_x_series(T x) : series_base<T, K>(x, x - std::sqrt(x)) {}

template <typename T, typename K>
constexpr T x_min_sqrt_x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	T tempsum = 0;
	for (int m = 0; m < std::pow(2, n); m++)
		tempsum += static_cast<T>(static_cast<T>(std::pow(this->x, m)) * static_cast<T>(this->binomial_coefficient(static_cast<T>(std::pow(2, n + 1)), 2 * m + 1)));

	return static_cast<T>(std::pow(this->x - 1, std::pow(2, n)) / tempsum);
}


/**
* @brief arctan_x2 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class arctan_x2_series : public series_base<T, K>
{
public:
	arctan_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	arctan_x2_series(T x);

	/**
	* @brief Computes the nth term of the arctan_x2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the arctan_x2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
arctan_x2_series<T, K>::arctan_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T arctan_x2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>((std::pow(-1, n) * std::pow(this->x, 4 * n + 2)) / (2 * n + 1));
}


/**
* @brief ln1px4 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln1px4_series : public series_base<T, K>
{
public:
	ln1px4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	ln1px4_series(T x);

	/**
	* @brief Computes the nth term of the ln1px4 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the ln1px4 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln1px4_series<T, K>::ln1px4_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T ln1px4_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(-1, n + 2) * std::pow(this->x, 4 * (n + 1)) / (n + 1));
}


/**
* @brief ln1px4 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class sin_x2_series : public series_base<T, K>
{
public:
	sin_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	sin_x2_series(T x);

	/**
	* @brief Computes the nth term of the ln1px4 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the ln1px4 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
sin_x2_series<T, K>::sin_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T sin_x2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 4 * n + 2) / this->fact(2 * n + 2));
}

/**
* @brief arctanx3 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class arctan_x3_series : public series_base<T, K>
{
public:
	arctan_x3_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	arctan_x3_series(T x);

	/**
	* @brief Computes the nth term of the arctanx3 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the arctanx3 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
arctan_x3_series<T, K>::arctan_x3_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T arctan_x3_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>((std::pow(-1, n) * std::pow(this->x, 6 * n + 3)) / (2 * n + 2));
}

/**
* @brief arcsinx2 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class arcsin_x2_series : public series_base<T, K>
{
public:
	arcsin_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	arcsin_x2_series(T x);

	/**
	* @brief Computes the nth term of the arcsinx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the arcsinx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
arcsin_x2_series<T, K>::arcsin_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T arcsin_x2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>((this->fact(2 * n) * std::pow(this->x, 4 * n + 2)) / (std::pow(4, n) * std::pow(this->fact(n), 2) * (2 * n + 1)));
}


/**
* @brief ln1minx2 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln1_m_x2_series : public series_base<T, K>
{
public:
	ln1_m_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	ln1_m_x2_series(T x);

	/**
	* @brief Computes the nth term of the ln1minx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the ln1minx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln1_m_x2_series<T, K>::ln1_m_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T ln1_m_x2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(-std::pow(this->x, 2 * n + 1) / (n + 1));
}


/**
* @brief artanhx serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class artanh_x_series : public series_base<T, K>
{
public:
	artanh_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	artanh_x_series(T x);

	/**
	* @brief Computes the nth term of the artanhx series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the artanhx series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
artanh_x_series<T, K>::artanh_x_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T artanh_x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(this->x, 2 * n + 1) / (2 * n + 1));
}


/**
* @brief arcsinhx serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class arcsinh_x_series : public series_base<T, K>
{
public:
	arcsinh_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	arcsinh_x_series(T x);

	/**
	* @brief Computes the nth term of the ln1minx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the ln1minx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
arcsinh_x_series<T, K>::arcsinh_x_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T arcsinh_x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>((std::pow(-1, n) * this->fact(2 * n) * std::pow(this->x, 2 * n + 1)) / (std::pow(this->fact(n), 2) * std::pow(4, n) * (2 * n + 1)));
}


/**
* @brief cosx2 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class cos_x2_series : public series_base<T, K>
{
public:
	cos_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	cos_x2_series(T x);

	/**
	* @brief Computes the nth term of the cosx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the cosx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
cos_x2_series<T, K>::cos_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T cos_x2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 4 * n) / this->fact(2 * n));
}


/**
* @brief sinhx2 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class sinh_x2_series : public series_base<T, K>
{
public:
	sinh_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	sinh_x2_series(T x);

	/**
	* @brief Computes the nth term of the sinhx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the sinhx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
sinh_x2_series<T, K>::sinh_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T sinh_x2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(this->x, 4 * n + 2) / this->fact(2 * n + 1));
}


/**
* @brief arctanhx2 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class arctanh_x2_series : public series_base<T, K>
{
public:
	arctanh_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	arctanh_x2_series(T x);

	/**
	* @brief Computes the nth term of the arctanhx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the arctanhx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
arctanh_x2_series<T, K>::arctanh_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T arctanh_x2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(this->x, 4 * n + 2) / (2 * n + 1));
}

/**
/**
* @brief Taylor series of function cos(3x - 1) / x^2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class cos3xmin1_div_xsqare_series : public series_base<T, K>
{
public:
	cos3xmin1_div_xsqare_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	*/
	cos3xmin1_div_xsqare_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
cos3xmin1_div_xsqare_series<T, K>::cos3xmin1_div_xsqare_series(T x) : series_base<T, K>(x, (std::cos(3 * x) - 1) / (x * x)) {}

template <typename T, typename K>
constexpr T cos3xmin1_div_xsqare_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return static_cast<T>(this->minus_one_raised_to_power_n(n + 1) * std::pow(3, 2 * n + 2) * std::pow(this->x, 2 * n) / this->fact(2 * n + 2));
}



/**
/**
* @brief Maclaurin series of function 2^x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class two_degree_x_series : public series_base<T, K>
{
public:
	two_degree_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	*/
	two_degree_x_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
two_degree_x_series<T, K>::two_degree_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(2, x))) {}

template <typename T, typename K>
constexpr T two_degree_x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return static_cast<T>(std::pow(std::log(2), n) * std::pow(this->x, n) / this->fact(n));
}


/**
/**
* @brief Maclaurin series of function sqrt(1 + x) - 1 - x/2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class sqrt_1plusx_min_1_min_x_div_2_series : public series_base<T, K>
{
public:
	sqrt_1plusx_min_1_min_x_div_2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	*/
	sqrt_1plusx_min_1_min_x_div_2_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
sqrt_1plusx_min_1_min_x_div_2_series<T, K>::sqrt_1plusx_min_1_min_x_div_2_series(T x) : series_base<T, K>(x, std::sqrt(1 + x) - 1 - x / 2) {}

template <typename T, typename K>
constexpr T sqrt_1plusx_min_1_min_x_div_2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K temp = 1;
	for (K i = 1; 2 * i <= 2 * n; ++i)
		temp *= 2 * i + 1;
	return static_cast<T>(this->minus_one_raised_to_power_n(n + 3) * std::pow(this->x, n + 2) * temp / (this->fact(n + 2) * std::pow(2, n + 2)));
}


/**
/**
* @brief Fourier series of function (ln13 - ln7) / 7
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln13_min_ln7_div_7_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	*/
	ln13_min_ln7_div_7_series();

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln13_min_ln7_div_7_series<T, K>::ln13_min_ln7_div_7_series() : series_base<T, K>(0, (static_cast<T>(std::log(13 / 7) / 7))) {}

template <typename T, typename K>
constexpr T ln13_min_ln7_div_7_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return static_cast<T>(this->minus_one_raised_to_power_n(n + 2) * std::pow(6, n + 1) / ((n + 1) * std::pow(7, n + 2)));
}

/**
* @brief Taylor series of Bessel function J_b(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class Ja_x_series : public series_base<T, K>
{
public:
	Ja_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series, b The integer constant
	*/
	Ja_x_series(T x, T a);

	/**
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
private:
	const T a;
};

template <typename T, typename K>
Ja_x_series<T, K>::Ja_x_series(T x, T a) : series_base<T, K>(x), a(a) {}

template <typename T, typename K>
constexpr T Ja_x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this->minus_one_raised_to_power_n(n) * std::pow(this->x / 2, 2 * n + this->a) / (this->fact(n) * std::tgamma(n + this->a + 1));
}


/**
/**
* @brief Taylor series of function 1/sqrt(2) * sin(x/sqrt(2)) (����������� ����� �� ���� �������)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class one_div_sqrt2_sin_xdivsqrt2_series : public series_base<T, K>
{
public:
	one_div_sqrt2_sin_xdivsqrt2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	*/
	one_div_sqrt2_sin_xdivsqrt2_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
one_div_sqrt2_sin_xdivsqrt2_series<T, K>::one_div_sqrt2_sin_xdivsqrt2_series(T x) : series_base<T, K>(x, std::sqrt(1 + x) - 1 - x / 2) {}

template <typename T, typename K>
constexpr T one_div_sqrt2_sin_xdivsqrt2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

#ifdef _WIN32
	return static_cast<T>(std::pow(-1, n / 2) * _jn(static_cast<int>(2 * n + 1), this->x));
#else
	return static_cast<T>(std::pow(-1, n / 2) * jn(static_cast<T>(2 * n + 1), this->x));
#endif
}


/**
* @brief ln(1 + x)/(1 + x^2) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln_1plusx_div_1plusx2 : public series_base<T, K>
{
public:
	ln_1plusx_div_1plusx2() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	ln_1plusx_div_1plusx2(T x);

	/**
	* @brief Computes the nth term of the ln(1 + x)/(1 + x^2) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the ln(1 + x)/(1 + x^2) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln_1plusx_div_1plusx2<T, K>::ln_1plusx_div_1plusx2(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T ln_1plusx_div_1plusx2<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(this->minus_one_raised_to_power_n(n + 2) * std::pow(this->x, n + 1) / ((n + 1) * std::pow(1 + (this->x * this->x), n + 1)));
}



/**
* @brief cos(sqrt(x)) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class cos_sqrt_x : public series_base<T, K>
{
public:
	cos_sqrt_x() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	cos_sqrt_x(T x);

	/**
	* @brief Computes the nth term of the cos(sqrt(x)) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the cos(sqrt(x)) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
cos_sqrt_x<T, K>::cos_sqrt_x(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T cos_sqrt_x<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(-1, n) * std::pow(this->x, n) / this->fact(2 * n));
}


/**
* @brief ln(1 + x^3) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln_1_plus_x3 : public series_base<T, K>
{
public:
	ln_1_plus_x3() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	ln_1_plus_x3(T x);

	/**
	* @brief Computes the nth term of the ln(1 + x^3) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the ln(1 + x^3) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln_1_plus_x3<T, K>::ln_1_plus_x3(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T ln_1_plus_x3<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(this->minus_one_raised_to_power_n(n + 2) * std::pow(this->x, 3 * (n + 1)) / (n + 1));
}


/**
* @brief x / sqrt(1 - x) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_div_1minx : public series_base<T, K>
{
public:
	x_div_1minx() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	x_div_1minx(T x);

	/**
	* @brief Computes the nth term of the x / sqrt(1 - x) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the x / sqrt(1 - x) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
x_div_1minx<T, K>::x_div_1minx(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T x_div_1minx<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(this->fact(2 * n) * std::pow(this->x, n + 1) / (this->fact(n) * std::pow(4, this->x)));
}


/**
* @brief x / sqrt(1 - x) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_div_1minx2 : public series_base<T, K>
{
public:
	x_div_1minx2() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	x_div_1minx2(T x);

	/**
	* @brief Computes the nth term of the x / sqrt(1 - x) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @return nth term of the x / sqrt(1 - x) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
x_div_1minx2<T, K>::x_div_1minx2(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T x_div_1minx2<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	return static_cast<T>(std::pow(this->x, 2 * n + 1));
}

/**
* @brief using gamma serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class gamma_series : public series_base<T, K>
{
public:
	gamma_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	*/
	gamma_series(T t, T x);

	/**
	* @brief Computes the nth term of the using gamma series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @param t The parametr for series
	* @return nth term of the using gamma series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	T t;
	T x;

	// ������ ������� ��� ������������ a_k
	T a_k(K n) const;
};

template <typename T, typename K>
gamma_series<T, K>::gamma_series(T t, T x) : series_base<T, K>(x), t(t), x(x) {}

template <typename T, typename K>
T gamma_series<T, K>::a_k(K n) const
{
	// ���������� ������������������ a_k, ��������:
	return static_cast<T>(n + 1);  // � �������� ������� ����� a_k = n + 1
}

template <typename T, typename K>
constexpr T gamma_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("The term index must be a non-negative integer");

	T a_k_n = a_k(n);  // ��������� a_k
	return static_cast<T>(a_k_n * std::pow(this->t, n) * std::tgamma(a_k_n));
}

/**
* @brief function for testing new serieses or converting basic to
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class testing_series : public series_base<T, K>
{
public:
	testing_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	testing_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
testing_series<T, K>::testing_series(T x) : series_base<T, K>(x, 0) {}

template <typename T, typename K>
constexpr T testing_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return 0;
}


/**
* @brief function for testing new requrrive serieses or converting basic to
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class recurrent_testing_series : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	recurrent_testing_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	recurrent_testing_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the series
	*/
	T acsess_row(K n);
};

template <typename T, typename K>
recurrent_testing_series<T, K>::recurrent_testing_series(T x) : series_base<T, K>(x, 0), recurrent_series_base<T, K>(x) {}

template <typename T, typename K>
T recurrent_testing_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (K i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * (this->x * this->x) / (i * (4 * i + 2)));
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T recurrent_testing_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<recurrent_testing_series<T, K>*>(this)->acsess_row(n);
	return a;

}
