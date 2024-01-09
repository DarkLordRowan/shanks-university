/**
 * @file series.h
 * List of series currently avaiable:
 * 1 - exp_series
 * 2 - four_arctan_series
 * 3 - cosh_series
 * 4 - ln1mx_series
 * 5 - mean_sinh_sin_series
 * 6 - exp_squared_erf_series
 * 7 - xmb_Jb_two_series
 * 8 - half_asin_two_x_series
 * @brief This file contains series base class and derived classes of various serieses (e.g. exp(x), ch(x))
 */
#pragma once
#define NO_X_GIVEN 0
#define NO_SERIES_EXPRESSION_GIVEN 0



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
	* @brief factorial
	* @authors Bolshakov M.P.
	* @return n!
	*/
	const int fact(const K n) const;
};

template <typename T, typename K>
series_base<T, K>::series_base() : x(NO_X_GIVEN), sum(0)
{
	
}

template <typename T, typename K>
series_base<T, K>::series_base(T x) : x(x), sum(0)
{

}

template <typename T, typename K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum)
{

}

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
series_base<T, K>::~series_base()
{

}

template <typename T, typename K>
const int series_base<T, K>::fact(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	int f = 1;
	for (int i = 2; i <= n; i++)
		f *= i;
	return f;
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
	/**
	* @brief Base constructor
	* @authors Bolshakov M.P.
	*/
	exp_series();
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	exp_series(T x);
	/**
	* @brief The Destructor.
	* @authors Bolshakov M.P.
	*/
	~exp_series();
	/**
	* @brief Computes the nth term of the Maclaurin series of the exponent
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the exponent
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
exp_series<T, K>::exp_series() : series_base<T, K>()
{

}

template <typename T, typename K>
exp_series<T, K>::exp_series(T x) : series_base<T,K>(x, std::exp(x))
{
}

template <typename T, typename K>
exp_series<T, K>::~exp_series()
{

}

template <typename T, typename K>
constexpr T exp_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return std::pow(this->x, n) / this->fact(n);
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
	/**
	* @brief Base constructor
	* @authors Bolshakov M.P.
	*/
	four_arctan_series();
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	four_arctan_series(T x);
	/**
	* @brief The Destructor.
	* @authors Bolshakov M.P.
	*/
	~four_arctan_series();
	/**
	* @brief Computes the nth term of the Maclaurin series of the arctan multiplied by four
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
four_arctan_series<T, K>::four_arctan_series() : series_base<T, K>()
{

}

template <typename T, typename K>
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, std::exp(x))
{
	if (std::abs(x) > 1)
		throw std::domain_error("the arctan series diverge at x = " + std::to_string(x));
}

template <typename T, typename K>
four_arctan_series<T, K>::~four_arctan_series()
{

}

template <typename T, typename K>
constexpr T four_arctan_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return 4 * (n % 2 ? -1 : 1) * pow(this->x, 2 * n + 1) / (2 * n + 1);
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
	/**
	* @brief Base constructor
	* @authors Pashkov B.B.
	*/
	cosh_series();

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.	
	* @param x The argument for function series
	*/
	cosh_series(T x);

	/**
	* @brief The Destructor.
	* @authors Pashkov B.B.
	*/
	~cosh_series();

	/**
	* @brief Computes the nth term of the Maclaurin series of hyperbolic cosine
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
cosh_series<T, K>::cosh_series() : series_base<T, K>()
{

}

template <typename T, typename K>
cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x))
{

}

template <typename T, typename K>
cosh_series<T, K>::~cosh_series()
{

}

template <typename T, typename K>
constexpr T cosh_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return pow(this->x, 2 * n) / this->fact(2 * n);
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
	/**
	* @brief Base constructor
	* @authors Pashkov B.B.
	*/
	ln1mx_series();

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	ln1mx_series(T x);

	/**
	* @brief The Destructor.
	* @authors Pashkov B.B.
	*/
	~ln1mx_series();

	/**
	* @brief Computes the nth term of the Maclaurin series of -ln(1 - x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
ln1mx_series<T, K>::ln1mx_series() : series_base<T, K>()
{

}

template <typename T, typename K>
ln1mx_series<T, K>::ln1mx_series(T x) : series_base<T, K>(x, -std::log(1 - x))
{
	if (std::abs(this->x) > 1 || this->x == 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
ln1mx_series<T, K>::~ln1mx_series()
{

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
	/**
	* @brief Base constructor
	* @authors Pashkov B.B.
	*/
	mean_sinh_sin_series();

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	mean_sinh_sin_series(T x);

	/**
	* @brief The Destructor.
	* @authors Pashkov B.B.
	*/
	~mean_sinh_sin_series();

	/**
	* @brief Computes the nth term of the Maclaurin series of (sinh(x) + sin(x)) / 2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
mean_sinh_sin_series<T, K>::mean_sinh_sin_series() : series_base<T, K>()
{

}

template <typename T, typename K>
mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, 0.5 * (std::sinh(x) + std::sin(x)))
{

}

template <typename T, typename K>
mean_sinh_sin_series<T, K>::~mean_sinh_sin_series()
{

}

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
	/**
	* @brief Base constructor
	* @authors Pashkov B.B.
	*/
	exp_squared_erf_series();

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	exp_squared_erf_series(T x);

	/**
	* @brief The Destructor.
	* @authors Pashkov B.B.
	*/
	~exp_squared_erf_series();

	/**
	* @brief Computes the nth term of the Maclaurin series of exp(x^2)*erf(x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
exp_squared_erf_series<T, K>::exp_squared_erf_series() : series_base<T, K>()
{

}

template <typename T, typename K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, std::exp(x * x)* std::erf(x))
{

}

template <typename T, typename K>
exp_squared_erf_series<T, K>::~exp_squared_erf_series()
{

}

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
	/**
	* @brief Base constructor
	* @authors Pashkov B.B.
	*/
	xmb_Jb_two_series();

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series, b The integer constant 
	*/
	xmb_Jb_two_series(T x, K b);

	/**
	* @brief The Destructor.
	* @authors Pashkov B.B.
	*/
	~xmb_Jb_two_series();

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
xmb_Jb_two_series<T, K>::xmb_Jb_two_series() : series_base<T, K>()
{

}

template <typename T, typename K>
xmb_Jb_two_series<T, K>::xmb_Jb_two_series(T x, K b) : series_base<T, K>(x, std::pow(x, -b)* std::cyl_bessel_j(b, 2 * x)), mu(b)
{

}

template <typename T, typename K>
xmb_Jb_two_series<T, K>::~xmb_Jb_two_series()
{

}

template <typename T, typename K>
constexpr T xmb_Jb_two_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return (1 - ((n & 1) << 1)) * pow(this->x, 2 * n) / this->fact(n) / this->fact(n + this->mu);
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
	/**
	* @brief Base constructor
	* @authors Pashkov B.B.
	*/
	half_asin_two_x_series();

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	half_asin_two_x_series(T x);

	/**
	* @brief The Destructor.
	* @authors Pashkov B.B.
	*/
	~half_asin_two_x_series();

	/**
	* @brief Computes the nth term of the Maclaurin series of 0.5 * asin(2x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	constexpr virtual T a_n(const K n) const;
};

template <typename T, typename K>
half_asin_two_x_series<T, K>::half_asin_two_x_series() : series_base<T, K>()
{

}

template <typename T, typename K>
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, 0.5 * std::asin(2 * x))
{
	if (std::abs(this->x) > 0.5)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
half_asin_two_x_series<T, K>::~half_asin_two_x_series()
{

}

template <typename T, typename K>
constexpr T half_asin_two_x_series<T, K>::a_n(const K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this->fact(2 * n) * pow(this->x, 2 * n + 1) / this->fact(n) / this->fact(n) / (2 * n + 1);
}