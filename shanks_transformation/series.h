/**
 * @file series.h
 * List of series currently avaiable:
 * 1 - exp_series
 * 2 - cos_series
 * 3 - sin_series
 * 4 - cosh_series
 * 5 - sinh_series
 * 6 - bin_series
 * 7 - four_arctan_series
 * 8 - ln1mx_series
 * 9 - mean_sinh_sin_series
 * 10 - exp_squared_erf_series
 * 11 - xmb_Jb_two_series
 * 12 - half_asin_two_x_series
 * @brief This file contains series base class and derived classes of various serieses (e.g. exp(x), ch(x))
 */

#pragma once
#define NO_X_GIVEN 0
#define NO_SERIES_EXPRESSION_GIVEN 0
#define MINUS_ONE_RAISED_TO_POWER_N (1 - ((n & 1) << 1)) //(-1)^n



/**
* @brief Abstract class for series
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class series_base
{
public:
	/**
	* @brief Base constructor
	* @authors Bolshakov M.P.
	*/
	series_base();

	/**
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	series_base(T x);

	/**
	* @brief Virtual distructor
	* @authors Bolshakov M.P.
	*/
	virtual ~series_base() = 0;

	/**
	* @brief Computes partial sum of the first n terms
	* @authors Bolshakov M.P.
	* @param n The amount of terms in the partial sum
	* @return Partial sum of the first n terms
	*/
	constexpr virtual T S_n(const K n) const;

	/**
	* @brief Computes nth term of the series
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;

	/**
	* @brief x getter
	* @authors Bolshakov M.P.
	*/
	constexpr const T get_x() const;

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	constexpr const T get_sum() const;
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
	* @brief factorial n!
	* @authors Bolshakov M.P.
	* @return n!
	*/
	constexpr const K fact(const K n) const;

	/**
	* @brief binomial coefficient C^n_k
	* @authors Bolshakov M.P.
	* @return combinations(n,k)
	*/
	constexpr const T binomial_coefficient(const T n, const K k) const;
};

template <typename T, typename K>
series_base<T, K>::series_base() : x(NO_X_GIVEN), sum(0) {}

template <typename T, typename K>
series_base<T, K>::series_base(T x) : x(x), sum(0) {}

template <typename T, typename K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum) {}

template <typename T, typename K>
constexpr T series_base<T, K>::S_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	T sum = a_n(n);
	for (int i = 0; i < n; ++i)
		sum += a_n(i);
	return sum;
}

template <typename T, typename K>
constexpr T series_base<T, K>::a_n(const K n) const
{
	return NO_SERIES_EXPRESSION_GIVEN;
}

template <typename T, typename K>
constexpr const T series_base<T, K>::get_x() const
{
	return x;
}

template <typename T, typename K>
constexpr const T series_base<T, K>::get_sum() const
{
	return sum;
}

template <typename T, typename K>
series_base<T, K>::~series_base() {}

template <typename T, typename K>
constexpr const K series_base<T, K>::fact(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	int f = 1;
	for (int i = 2; i <= n; i++)
		f *= i;
	return f;
}

template <typename T, typename K>
constexpr const T series_base<T, K>::binomial_coefficient(const T n, const K k) const
{
	T b_c = 1;
	for (int i = 0; i < k; ++i)
		b_c = b_c * (n - static_cast<T>(i)) / (i + 1);
	return b_c;
}

/**
* @brief Maclaurin series of exponent
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class exp_series : public series_base<T, K>
{
public:
	exp_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	exp_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the exponent
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the exponent
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
exp_series<T, K>::exp_series(T x) : series_base<T,K>(x, std::exp(x)) {}

template <typename T, typename K>
constexpr T exp_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return std::pow(this->x, n) / this->fact(n);
}

/**
* @brief Maclaurin series of cosine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class cos_series : public series_base<T, K>
{
public:
	cos_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	cos_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
cos_series<T, K>::cos_series(T x) : series_base<T, K>(x, std::cos(x)) {}

template <typename T, typename K>
constexpr T cos_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return MINUS_ONE_RAISED_TO_POWER_N * pow(this->x, 2 * n) / this->fact(2 * n);
}

/**
* @brief Maclaurin series of sine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class sin_series : public series_base<T, K>
{
public:
	sin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	sin_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
sin_series<T, K>::sin_series(T x) : series_base<T, K>(x, std::sin(x)) {}

template <typename T, typename K>
constexpr T sin_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return MINUS_ONE_RAISED_TO_POWER_N * pow(this->x, 2 * n + 1) / this->fact(2 * n + 1);
}

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class cosh_series : public series_base<T, K>
{
public:
	cosh_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.	
	* @param x The argument for function series
	*/
	cosh_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of hyperbolic cosine
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x)) {}

template <typename T, typename K>
constexpr T cosh_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(this->x, 2 * n) / this->fact(2 * n);
}

/**
* @brief Maclaurin series of sinh function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class sinh_series : public series_base<T, K>
{
public:
	sinh_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	sinh_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sinh function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sinh functions
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
sinh_series<T, K>::sinh_series(T x) : series_base<T, K>(x, std::sinh(x)) {}

template <typename T, typename K>
constexpr T sinh_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(this->x, 2 * n + 1) / this->fact(2 * n + 1);
}

/**
* @brief Binomial series ( (1+x)^a maclaurin series)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class bin_series : public series_base<T, K>
{
public:
	bin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series, b The integer constant
	*/
	bin_series(T x, T alpha);

	/**
	* @brief Computes the nth term of the Binomial series
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
private:

	/**
	* @brief The power
	* @authors Bolshakov M.P.
	*/
	const T alpha;
};

template <typename T, typename K>
bin_series<T, K>::bin_series(T x, T alpha) : series_base<T, K>(x, std::pow(1 + x, alpha)), alpha(alpha)
{
	if (std::abs(x) >= 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T bin_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this->binomial_coefficient(alpha, n) * pow(this->x, n);
}

/**
* @brief Maclaurin series of arctan multiplied by four
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class four_arctan_series : public series_base<T, K>
{
public:
	four_arctan_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	four_arctan_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the arctan multiplied by four
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, std::exp(x))
{
	if (std::abs(x) > 1)
		throw std::domain_error("the arctan series diverge at x = " + std::to_string(x));
}

template <typename T, typename K>
constexpr T four_arctan_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return 4 * MINUS_ONE_RAISED_TO_POWER_N * pow(this->x, 2 * n + 1) / (2 * n + 1);
}

/**
* @brief Maclaurin series of -ln(1 - x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln1mx_series : public series_base<T, K>
{
public:
	ln1mx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	ln1mx_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of -ln(1 - x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
ln1mx_series<T, K>::ln1mx_series(T x) : series_base<T, K>(x, -std::log(1 - x))
{
	if (std::abs(this->x) > 1 || this->x == 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T ln1mx_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(this->x, n + 1) / (n + 1);
}

/**
* @brief Maclaurin series of (sinh(x) + sin(x)) / 2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class mean_sinh_sin_series : public series_base<T, K>
{
public:
	mean_sinh_sin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	mean_sinh_sin_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of (sinh(x) + sin(x)) / 2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, 0.5 * (std::sinh(x) + std::sin(x))) {}

template <typename T, typename K>
constexpr T mean_sinh_sin_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(this->x, 4 * n + 1) / this->fact(4 * n + 1);
}

/**
* @brief Maclaurin series of exp(x^2)*erf(x) where erf(x) is error function of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class exp_squared_erf_series : public series_base<T, K>
{
public:
	exp_squared_erf_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	exp_squared_erf_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of exp(x^2)*erf(x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, std::exp(x * x)* std::erf(x)) {}

template <typename T, typename K>
constexpr T exp_squared_erf_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(this->x, 2 * n + 1) / std::tgamma(n + 1.5);
}

/**
* @brief Maclaurin series of x^(-b) * J_b(2x) where J_b(x) is Bessel function of the first kind of order b
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class xmb_Jb_two_series : public series_base<T, K>
{
public:
	xmb_Jb_two_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series, b The integer constant 
	*/
	xmb_Jb_two_series(T x, K b);

	/**
	* @brief Computes the nth term of the Maclaurin series of x^(-b) * J_b(2x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
private:

	/**
	* @brief The order of Bessel function
	* @authors Pashkov B.B.
	*/
	const K mu;
};

template <typename T, typename K>
xmb_Jb_two_series<T, K>::xmb_Jb_two_series(T x, K b) : series_base<T, K>(x, std::pow(x, -b)* std::cyl_bessel_j(b, 2 * x)), mu(b) {}

template <typename T, typename K>
constexpr T xmb_Jb_two_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return MINUS_ONE_RAISED_TO_POWER_N * pow(this->x, 2 * n) / (this->fact(n) * this->fact(n + this->mu));
}

/**
* @brief Maclaurin series of 0.5 * asin(2x) where asin(x) is inverse sine of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class half_asin_two_x_series : public series_base<T, K>
{
public:
	half_asin_two_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	half_asin_two_x_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of 0.5 * asin(2x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, 0.5 * std::asin(2 * x))
{
	if (std::abs(this->x) > 0.5)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T half_asin_two_x_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this->fact(2 * n) * pow(this->x, 2 * n + 1) / (this->fact(n) * this->fact(n) * (2 * n + 1));
}