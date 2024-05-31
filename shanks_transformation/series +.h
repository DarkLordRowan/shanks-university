/** updated
 * @file series +.h
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
 * 13 - inverse_1mx_series
 * 14 - x_1mx_squared_series
 * 15 - erf_series
 * 16 - m_fact_1mx_mp1_inverse_series
 * 17 - inverse_sqrt_1m4x_series
 * 18 - one_twelfth_3x2_pi2_series
 * 19 - x_twelfth_x2_pi2_series
 * 20 - ln2_series
 * 21 - one_series
 * 22 - minus_one_quarter_series
 * 23 - pi_3_series
 * 24 - pi_4_series
 * 25 - pi_squared_6_minus_one_series
 * 26 - three_minus_pi_sderies
 * 27 - one_twelfth_series
 * 28 - eighth_pi_m_one_third_series
 * 29 - one_third_pi_squared_m_nine_series
 * 30 - four_ln2_m_3_series
 * 31 - exp_m_cos_x_sinsin_x_series
 * 32 - pi_four_minus_ln2_halfed_series
 * 33 - five_pi_twelve_series
 * 34 - x_two_series
 * 35 - pi_six_min_half_series
 * 36 - x_two_throught_squares
 * 37 - minus_one_ned_in_n_series
 * 38 - minus_one_n_fact_n_in_n_series
 * 39 - ln_x_plus_one_x_minus_one_halfed_series
 * 40 - two_arcsin_square_x_halfed_series
 * @brief This file contains series base class, requrrent_series_base class and derived classes of various serieses (e.g. exp(x), ch(x))
 */

#pragma once
#define NO_X_GIVEN 0
#define NO_SERIES_EXPRESSION_GIVEN 0
#include <numbers>
#include <limits>
#include <vector>

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
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	series_base(T x = 0);

	/**
	* @brief Computes partial sum of the first n terms
	* @authors Bolshakov M.P.
	* @param n The amount of terms in the partial sum
	* @return Partial sum of the first n terms
	*/
	[[nodiscard]] constexpr T S_n(K n) const;

	/**
	* @brief Computes nth term of the series
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const = 0;

	/**
	* @brief x getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_x() const;

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_sum() const;

	/**
	* @brief factorial n!
	* @authors Bolshakov M.P.
	* @return n!
	*/
	[[nodiscard]] constexpr static const K fact(K n);

	/**
	* @brief binomial coefficient C^n_k
	* @authors Bolshakov M.P.
	* @return combinations(n,k)
	*/
	[[nodiscard]] constexpr static const T binomial_coefficient(const T n, const K k);


	/**
	* @brief evaluates (-1)^n
	* @authors Bolshakov M.P.
	* @return (-1)^n
	*/
	[[nodiscard]] constexpr static const T minus_one_raised_to_power_n(K n);

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
};

template <typename T, typename K>
series_base<T, K>::series_base(T x) : x(x), sum(0)
{
	static_assert(std::is_floating_point_v<T>);					
	static_assert(std::numeric_limits<K>::is_integer);		
}

template <typename T, typename K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum)
{
	static_assert(std::is_floating_point_v<T>);					
	static_assert(std::numeric_limits<K>::is_integer);
}

template <typename T, typename K>
constexpr T series_base<T, K>::S_n(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	T sum = operator()(n);
	for (int i = 0; i < n; ++i)
		sum += operator()(i);
	return sum;
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
constexpr const K series_base<T, K>::fact(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	int f = 1;
	for (int i = 2; i <= n; ++i)
		f *= i;
	return f;
}

template <typename T, typename K>
constexpr const T series_base<T, K>::binomial_coefficient(const T n, const K k)
{
	T b_c = 1;
	for (int i = 0; i < k; ++i)
		b_c = b_c * (n - static_cast<T>(i)) / (i + 1);
	return b_c;
}

template <typename T, typename K>
constexpr const T series_base<T, K>::minus_one_raised_to_power_n(K n)
{
	return n % 2 ? -1 : 1;
}


/**
* @brief Abstract class for requrrent series
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class requrrent_series_base
{
public:
	/**
	* @brief Parameterized constructor to initialize the requrrent series with function argument
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	requrrent_series_base(T x);

	/**
	* @brief Parameterized constructor to initialize the requrrent series with vector, containing elements of series
	* @authors Kreynin R.G.
	* @param row The first elements of the series
	*/
	requrrent_series_base(std::vector<T> row);

	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the series
	*/
	virtual T acsess_row(K n) = 0;

	/**
	* @brief Vector, containing elements of the series 
	* It's empty by default
	* @authors Kreynin R.G.
	*/
	std::vector<T> series_vector;
};

template <typename T, typename K>
requrrent_series_base<T, K>::requrrent_series_base(T x)
{
	this->series_vector.push_back(x);
};

template <typename T, typename K>
requrrent_series_base<T, K>::requrrent_series_base(std::vector<T> row)
{
	if (row.size() < 1)
		throw std::domain_error("empty row imput");

	this->series_vector = row;
};


template <typename T, typename K>
class exp_series : public series_base<T, K>, public requrrent_series_base<T, K>
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
exp_series<T, K>::exp_series(T x) : series_base<T, K>(x, std::exp(x)), requrrent_series_base<T, K>(T(1)) {  }

template <typename T, typename K>
T exp_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * this->x / i);
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T exp_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<exp_series<T, K>*>(this)->acsess_row(n);
	return a;
}

/**
* @brief Maclaurin series of cosine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class cos_series : public series_base<T, K>, public requrrent_series_base<T, K>
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
cos_series<T, K>::cos_series(T x) : series_base<T, K>(x, std::cos(x)), requrrent_series_base<T, K>(std::vector<T>{1, T((-1)* std::pow(x, 2) / 2)}) {}

template <typename T, typename K>
T cos_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * T(-1) * std::pow(this->x, 2) / (2 * i * (2 * i - 1)));
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T cos_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<cos_series<T, K>*>(this)->acsess_row(n);
	return a;
}

/**
* @brief Maclaurin series of sine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class sin_series : public series_base<T, K>, public requrrent_series_base<T, K>
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
sin_series<T, K>::sin_series(T x) : series_base<T, K>(x, std::sin(x)), requrrent_series_base<T, K>(x) {}

template <typename T, typename K>
T sin_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * T(-1) * std::pow(this->x, 2) / (2 * i * (2 * i + 1)));
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T sin_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<sin_series<T, K>*>(this)->acsess_row(n);
	return a;
}

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class cosh_series : public series_base<T, K>, public requrrent_series_base<T, K>
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
cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x)), requrrent_series_base<T, K>(std::vector<T>{1, T(std::pow(this->x, 2) / 2) }) {}

template <typename T, typename K>
T cosh_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * std::pow(this->x, 2) / (2 * i * (2 * i - 1)));
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T cosh_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<cosh_series<T, K>*>(this)->acsess_row(n);
	return a;
}

/**
* @brief Maclaurin series of sinh function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class sinh_series : public series_base<T, K>, public requrrent_series_base<T, K>
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
sinh_series<T, K>::sinh_series(T x) : series_base<T, K>(x, std::sinh(x)), requrrent_series_base<T, K>(x) {}

template <typename T, typename K>
T sinh_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * std::pow(this->x, 2) / (2 * i * (2 * i + 1)));
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T sinh_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<sinh_series<T, K>*>(this)->acsess_row(n);
	return a;
}

/**
* @brief Binomial series ( (1+x)^a maclaurin series)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class bin_series : public series_base<T, K>
{
	using series_base<T, K>::binomial_coefficient;

public:
	bin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series, alpha The integer constant
	*/
	bin_series(T x, T alpha);

	/**
	* @brief Computes the nth term of the Binomial series
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
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
	if (std::abs(x) > 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T bin_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return binomial_coefficient(alpha, n) * std::pow(this->x, n);
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
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, 4 * std::atan(x))
{
	if (std::abs(x) > 1)
		throw std::domain_error("the arctan series diverge at x = " + std::to_string(x));
}

template <typename T, typename K>
constexpr T four_arctan_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return 4 * series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, 2 * n + 1) / (2 * n + 1);
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
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln1mx_series<T, K>::ln1mx_series(T x) : series_base<T, K>(x, -std::log(1 - x))
{
	if (std::abs(this->x) > 1 || this->x == 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T ln1mx_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return std::pow(this->x, n + 1) / (n + 1);
}

/**
* @brief Maclaurin series of (sinh(x) + sin(x)) / 2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class mean_sinh_sin_series : public series_base<T, K>, public requrrent_series_base<T, K>
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
mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, 0.5 * (std::sinh(x) + std::sin(x))), requrrent_series_base<T, K>(x) {}

template <typename T, typename K>
T mean_sinh_sin_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * std::pow(this->x, 4) / ((4 * i + 1) * (4 * i) * (4 * i - 1) * (4 * i - 2)));
	}

	return this->series_vector[n];
} //std::pow(this->x, 4 * n + 1) / this->fact(4 * n + 1); FIX THIS SHIT!

template <typename T, typename K>
constexpr T mean_sinh_sin_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<mean_sinh_sin_series<T, K>*>(this)->acsess_row(n);
	return a;
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
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, std::exp(x* x)* std::erf(x)) {}

template <typename T, typename K>
constexpr T exp_squared_erf_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const auto result = std::pow(this->x, 2 * n + 1) / std::tgamma(n + 1.5);
	if (!isfinite(result))
		throw std::overflow_error("operator() is too big");
	return result;
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
	[[nodiscard]] constexpr virtual T operator()(K n) const;
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
constexpr T xmb_Jb_two_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, 2 * n) / (this->fact(n) * this->fact(n + this->mu));
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
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, 0.5 * std::asin(2 * x))
{
	if (std::abs(this->x) > 0.5)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T half_asin_two_x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const auto _fact_n = this->fact(n);
	return this->fact(2 * n) * std::pow(this->x, 2 * n) / (_fact_n * _fact_n * (2 * n + 1)); // p. 566 typo
}

/**
* @brief Maclaurin series of 1 / (1 - x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class inverse_1mx_series : public series_base<T, K>
{
public:
	inverse_1mx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	inverse_1mx_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of 1 / (1 - x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
inverse_1mx_series<T, K>::inverse_1mx_series(T x) : series_base<T, K>(x, 1 / (1 - x))
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T inverse_1mx_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return std::pow(this->x, n);
}

/**
* @brief Maclaurin series of x / (1 - x)^2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_1mx_squared_series : public series_base<T, K>
{
public:
	x_1mx_squared_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	x_1mx_squared_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of x / (1 - x)^2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
x_1mx_squared_series<T, K>::x_1mx_squared_series(T x) : series_base<T, K>(x, x / std::fma(x, x - 1, 1 - x))
{
	if (std::abs(this->x) > 1 || this->x == 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T x_1mx_squared_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return std::pow(this->x, n) * n;
}

/**
* @brief Maclaurin series of sqrt(pi) * erf(x) / 2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class erf_series : public series_base<T, K>, public requrrent_series_base<T,K>
{
public:
	erf_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	erf_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of sqrt(pi) * erf(x) / 2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
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
erf_series<T, K>::erf_series(T x) : series_base<T, K>(x, std::sqrt(std::numbers::pi)* std::erf(x) * 0.5), requrrent_series_base<T, K>(x)
{

}

template <typename T, typename K>
T erf_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * (-1) * std::pow(this->x, 2) / i  * (2 * (i - 1) + 1) / (2 * i + 1));
	}

	return this->series_vector[n];
}


template <typename T, typename K>
constexpr T erf_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<erf_series<T, K>*>(this)->acsess_row(n);
	return a;
}

/**
* @brief Maclaurin series of m! / (1 - x) ^ (m + 1)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class m_fact_1mx_mp1_inverse_series : public series_base<T, K>, public requrrent_series_base<T,K>
{
public:
	m_fact_1mx_mp1_inverse_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series, m The integer constant
	*/
	m_fact_1mx_mp1_inverse_series(T x, K m);

	/**
	* @brief Computes the nth term of the Maclaurin series of  m! / (1 - x) ^ (m + 1)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
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
	T acsess_row(K n);
};

template <typename T, typename K>
m_fact_1mx_mp1_inverse_series<T, K>::m_fact_1mx_mp1_inverse_series(T x, K m) : series_base<T, K>(x, this->fact(m) / pow(1 - x, m + 1)), m(m), requrrent_series_base<T, K>(this->fact(m))
{
	if (!isfinite(series_base<T, K>::sum)) // sum = this->fact(m) / pow(1 - x, m + 1))
		throw std::overflow_error("sum is too big");
	if (std::abs(this->x) >= 1) // p. 564 typo
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
T m_fact_1mx_mp1_inverse_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * this->x * (this->m + i) / i);
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T m_fact_1mx_mp1_inverse_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<m_fact_1mx_mp1_inverse_series<T, K>*>(this)->acsess_row(n);
	return a;
}

/**
* @brief Maclaurin series of (1 - 4x) ^ (-1/2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class inverse_sqrt_1m4x_series : public series_base<T, K>
{
public:
	inverse_sqrt_1m4x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	inverse_sqrt_1m4x_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of (1 - 4x) ^ (-1/2)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
inverse_sqrt_1m4x_series<T, K>::inverse_sqrt_1m4x_series(T x) : series_base<T, K>(x, std::pow(std::fma(-4, x, 1), -0.5))
{
	if (std::abs(this->x) > 0.25 || this->x == 0.25)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T inverse_sqrt_1m4x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const auto _fact_n = this->fact(n);
	return this->fact(2 * n) * pow(this->x, n) / (_fact_n * _fact_n);
}

/**
* @brief Trigonometric series of 1/12 * (3x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class one_twelfth_3x2_pi2_series : public series_base<T, K>
{
public:
	one_twelfth_3x2_pi2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	one_twelfth_3x2_pi2_series(T x);

	/**
	* @brief Computes the nth term of the Trigonometric series of 1/12 * (3x^2 - pi^2)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
one_twelfth_3x2_pi2_series<T, K>::one_twelfth_3x2_pi2_series(T x) : series_base<T, K>(x, std::fma(0.25 * x, x, -std::numbers::pi * std::numbers::pi / 12))
{
	if (std::abs(this->x) > std::numbers::pi)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T one_twelfth_3x2_pi2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? series_base<T, K>::minus_one_raised_to_power_n(n) * std::cos(n * this->x) / (n * n) : 0;
}

/**
* @brief Trigonometric series of x/12 * (x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_twelfth_x2_pi2_series : public series_base<T, K>
{
public:
	x_twelfth_x2_pi2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	x_twelfth_x2_pi2_series(T x);

	/**
	* @brief Computes the nth term of the Trigonometric series of x/12 * (x^2 - pi^2)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
x_twelfth_x2_pi2_series<T, K>::x_twelfth_x2_pi2_series(T x) : series_base<T, K>(x, std::fma(x / 12, (x + std::numbers::pi)* (x - std::numbers::pi), -std::fma(x + std::numbers::pi, x - std::numbers::pi, (x + std::numbers::pi) * (x - std::numbers::pi))))
{
	if (std::abs(this->x) > std::numbers::pi)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T x_twelfth_x2_pi2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? series_base<T, K>::minus_one_raised_to_power_n(n) * std::sin(n * this->x) / (n * n * n) : 0;
}

/**
* @brief Numerical series representation of ln(2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln2_series : public series_base<T, K>
{
public:
	ln2_series();

	/**
	* @brief Computes the nth term of the Numerical series of ln(2)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln2_series<T, K>::ln2_series() : series_base<T, K>(0, std::log(2)) {}

template <typename T, typename K>
constexpr T ln2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? static_cast<T>(-series_base<T, K>::minus_one_raised_to_power_n(n)) / n : 0;
}

/**
* @brief Numerical series representation of 1
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class one_series : public series_base<T, K>
{
public:
	/**
	* @brief series constructor
	* @authors Pashkov B.B.
	*/
	one_series();

	/**
	* @brief Computes the nth term of the Numerical series of 1
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
one_series<T, K>::one_series() : series_base<T, K>(0, 1) {}

template <typename T, typename K>
constexpr T one_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? 1.0 / fma(n, n, n) : 0;
}

/**
* @brief Numerical series representation of -1/4
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class minus_one_quarter_series : public series_base<T, K>
{
public:
	minus_one_quarter_series();

	/**
	* @brief Computes the nth term of the Numerical series of -1/4
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
minus_one_quarter_series<T, K>::minus_one_quarter_series() : series_base<T, K>(0, -0.25) {}

template <typename T, typename K>
constexpr T minus_one_quarter_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? series_base<T, K>::minus_one_raised_to_power_n(n) / (n * n + 2 * n) : 0;
}

/**
* @brief Numerical series representation of pi/3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class pi_3_series : public series_base<T, K>
{
public:
	pi_3_series();

	/**
	* @brief Computes the nth term of the Numerical series of pi/3
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
pi_3_series<T, K>::pi_3_series() : series_base<T, K>(0, std::numbers::pi / 3) {}

template <typename T, typename K>
constexpr T pi_3_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return 1.0 / ((n + 1) * (2 * n + 1) * (4 * n + 1));
}

/**
* @brief Numerical series representation of pi/4
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class pi_4_series : public series_base<T, K>
{
public:
	pi_4_series();

	/**
	* @brief Computes the nth term of the Numerical series of pi/4
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
pi_4_series<T, K>::pi_4_series() : series_base<T, K>(0, 0.25 * std::numbers::pi) {}

template <typename T, typename K>
constexpr T pi_4_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return series_base<T, K>::minus_one_raised_to_power_n(n) / (2 * n + 1);
}

/**
* @brief Numerical series representation of pi^2 / 6 - 1
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class pi_squared_6_minus_one_series : public series_base<T, K>
{
public:
	pi_squared_6_minus_one_series();

	/**
	* @brief Computes the nth term of the Numerical series of pi^2 / 6 - 1
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
pi_squared_6_minus_one_series<T, K>::pi_squared_6_minus_one_series() : series_base<T, K>(0, std::fma(std::numbers::pi / 6, std::numbers::pi, -1)) {}

template <typename T, typename K>
constexpr T pi_squared_6_minus_one_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? 1.0 / (n * n * (n + 1)) : 0;
}

/**
* @brief Numerical series representation of 3 - pi
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class three_minus_pi_series : public series_base<T, K>
{
public:
	three_minus_pi_series();

	/**
	* @brief Computes the nth term of the Numerical series of 3 - pi
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
three_minus_pi_series<T, K>::three_minus_pi_series() : series_base<T, K>(0, 3 - std::numbers::pi) {}

template <typename T, typename K>
constexpr T three_minus_pi_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? series_base<T, K>::minus_one_raised_to_power_n(n) / (n * (n + 1) * (2 * n + 1)) : 0;
}

/**
* @brief Numerical series representation of 1/12
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class one_twelfth_series : public series_base<T, K>
{
public:
	one_twelfth_series();

	/**
	* @brief Computes the nth term of the Numerical series of 1 / 12
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
one_twelfth_series<T, K>::one_twelfth_series() : series_base<T, K>(0, 1 / 12) {}

template <typename T, typename K>
constexpr T one_twelfth_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return 1.0 / ((2 * n + 1) * (2 * n + 3) * (2 * n + 5));
}

/**
* @brief Numerical series representation of pi/8 - 1/3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class eighth_pi_m_one_third_series : public series_base<T, K>
{
public:
	eighth_pi_m_one_third_series();

	/**
	* @brief Computes the nth term of the Numerical series of pi/8 - 1/3
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
eighth_pi_m_one_third_series<T, K>::eighth_pi_m_one_third_series() : series_base<T, K>(0, std::numbers::pi / 8 - 1 / 3) {}

template <typename T, typename K>
constexpr T eighth_pi_m_one_third_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return series_base<T, K>::minus_one_raised_to_power_n(n) / ((2 * n + 1) * (2 * n + 3) * (2 * n + 5));
}

/**
* @brief Numerical series representation of (pi^2 - 9) / 3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class one_third_pi_squared_m_nine_series : public series_base<T, K>
{
public:
	one_third_pi_squared_m_nine_series();

	/**
	* @brief Computes the nth term of the Numerical series of (pi^2 - 9) / 3
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
one_third_pi_squared_m_nine_series<T, K>::one_third_pi_squared_m_nine_series() : series_base<T, K>(0, std::fma(std::numbers::pi, std::numbers::pi, -9) / 3) {}

template <typename T, typename K>
constexpr T one_third_pi_squared_m_nine_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? 1.0 / (n * n * (n + 1) * (n + 1)) : 0;
}

/**
* @brief Numerical series representation of 4 * ln2 - 3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class four_ln2_m_3_series : public series_base<T, K>
{
public:
	four_ln2_m_3_series();

	/**
	* @brief Computes the nth term of the Numerical series of 4 * ln2 - 3
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
four_ln2_m_3_series<T, K>::four_ln2_m_3_series() : series_base<T, K>(0, std::fma(4, std::log(2), -3)) {}

template <typename T, typename K>
constexpr T four_ln2_m_3_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return n ? series_base<T, K>::minus_one_raised_to_power_n(n) / (n * n * (n + 1) * (n + 1)) : 0;
}

/**
* @brief Maclaurin series of exp(-cos(x)) * sin(sin(x))
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class exp_m_cos_x_sinsin_x_series : public series_base<T, K>
{
public:
	exp_m_cos_x_sinsin_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	exp_m_cos_x_sinsin_x_series(T x);

	/**
	* @brief Computes the nth term of the exp(-cos(x)) * sin(sin(x)) series
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
exp_m_cos_x_sinsin_x_series<T, K>::exp_m_cos_x_sinsin_x_series(T x) : series_base<T, K>(x, std::exp(-std::cos(x))* std::sin(std::sin(x))) {}

template <typename T, typename K>
constexpr T exp_m_cos_x_sinsin_x_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this->minus_one_raised_to_power_n(n) * std::sin(n * this->x) / this->fact(n);
}

/**
* @brief pi/4 - ln2/2 series page 526 part 1 This series = -0.5151, but in reality 0,438825
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class pi_four_minus_ln2_halfed_series : public series_base<T, K>
{
public:
	pi_four_minus_ln2_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	pi_four_minus_ln2_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
pi_four_minus_ln2_halfed_series<T, K>::pi_four_minus_ln2_halfed_series(T x) : series_base<T, K>(x, x * (std::numbers::pi / 4 - std::log(2) / 2)) {}

template <typename T, typename K>
constexpr T pi_four_minus_ln2_halfed_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	if (n == 0)
		return 0;
	return this->x * series_base<T, K>::minus_one_raised_to_power_n(std::trunc(n / 2)) / n;
}

/**
* @brief 5pi/12 p 528 B1
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class five_pi_twelve_series : public series_base<T, K>
{
public:
	five_pi_twelve_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	five_pi_twelve_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
five_pi_twelve_series<T, K>::five_pi_twelve_series(T x) : series_base<T, K>(x, x * 5 * std::numbers::pi / 12) {}

template <typename T, typename K>
constexpr T five_pi_twelve_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this->x * series_base<T, K>::minus_one_raised_to_power_n(std::trunc(n / 3)) / (2*n + 1);
}

/**
* @brief x / 2 series p 531
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_two_series : public series_base<T, K>
{
public:
	x_two_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	x_two_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
x_two_series<T, K>::x_two_series(T x) : series_base<T, K>(x, x / 2) {}

template <typename T, typename K>
constexpr T x_two_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this->x / ((2 * n + 1) * (2 * n + 3));
}

/**
* @brief pi/5 - 1/2 series p 534
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class pi_six_min_half_series : public series_base<T, K>
{
public:
	pi_six_min_half_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	pi_six_min_half_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
pi_six_min_half_series<T, K>::pi_six_min_half_series(T x) : series_base<T, K>(x, x * (std::numbers::pi / 6  - 1 / 2)) {}

template <typename T, typename K>
constexpr T pi_six_min_half_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return this-> x * series_base<T, K>::minus_one_raised_to_power_n(n) / ((6 * n + 5) * (6 * n + 7));
}


/**
* @brief x/2 p 552
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_two_throught_squares_series : public series_base<T, K>
{
public:
	x_two_throught_squares_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	x_two_throught_squares_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
x_two_throught_squares_series<T, K>::x_two_throught_squares_series(T x) : series_base<T, K>(x, x / 2) {}

template <typename T, typename K>
constexpr T x_two_throught_squares_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	if (n == 0)
		return 0;
	return this->x * (2 * std::pow(n, 2) - 1) / (4 * std::pow(n, 4) + 1);
}

/**
* @brief (-1)^n/n^n series = -0,78343051	p 553
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class minus_one_ned_in_n_series : public series_base<T, K>
{
public:
	minus_one_ned_in_n_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	minus_one_ned_in_n_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
minus_one_ned_in_n_series<T, K>::minus_one_ned_in_n_series(T x) : series_base<T, K>(x, -0.78343051 * x) {}

template <typename T, typename K>
constexpr T minus_one_ned_in_n_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	if (n == 0)
		return 0;
	return this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / std::pow(n,n);
}

/**
* @brief (-1)^n n! / n^n  p 554
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class minus_one_n_fact_n_in_n_series : public series_base<T, K>
{
public:
	minus_one_n_fact_n_in_n_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	minus_one_n_fact_n_in_n_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
minus_one_n_fact_n_in_n_series<T, K>::minus_one_n_fact_n_in_n_series(T x) : series_base<T, K>(x, -0.65583160 * x) {}

template <typename T, typename K>
constexpr T minus_one_n_fact_n_in_n_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	if (n == 0)
		return 0;
	return this->x * this->fact(n) * series_base<T, K>::minus_one_raised_to_power_n(n) / std::pow(n, n);
}

/**
* @brief ln(x+1)/(1-x) / 2 series p557
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class ln_x_plus_one_x_minus_one_halfed_series : public series_base<T, K>
{
public:
	ln_x_plus_one_x_minus_one_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	ln_x_plus_one_x_minus_one_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
ln_x_plus_one_x_minus_one_halfed_series<T, K>::ln_x_plus_one_x_minus_one_halfed_series(T x) : series_base<T, K>(x, std::log((1 + x) / (1 - x)) / 2)
{
	if (std::abs(x) > 1)
		throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T ln_x_plus_one_x_minus_one_halfed_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return std::pow(this->x, 2 * n + 1) / (2 * n + 1);
}

/**
* @brief 2 arcsin(x/2)^2  p 567
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class two_arcsin_square_x_halfed_series : public series_base<T, K>
{
public:
	two_arcsin_square_x_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	two_arcsin_square_x_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(T x) : series_base<T, K>(x, 2 * std::pow(std::asin(x / 2), 2)) {}

template <typename T, typename K>
constexpr T two_arcsin_square_x_halfed_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	return  pow(this->x, 2 * n + 2) * pow(this->fact(n), 2) / (this->fact(2 * n + 2));
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
class requrrent_testing_series : public series_base<T, K>, public requrrent_series_base<T, K>
{
public:
	requrrent_testing_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	requrrent_testing_series(T x);

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
requrrent_testing_series<T, K>::requrrent_testing_series(T x) : series_base<T, K>(x, 0), requrrent_series_base<T, K>(x) {}

template <typename T, typename K>
T requrrent_testing_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K old_size = this->series_vector.size();

	for (int i = old_size; i <= n; ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * std::pow(this->x, 2) / (2 * i * (2 * i + 1)));
	}

	return this->series_vector[n];
}

template <typename T, typename K>
constexpr T requrrent_testing_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<requrrent_testing_series<T, K>*>(this)->acsess_row(n);
	return a;
}