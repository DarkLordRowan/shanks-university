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
 * 41 - pi_squared_twelve_series
 *  42 - pi_cubed_32_series
 *  43 - minus_three_plus_ln3_three_devided_two_plus_two_ln2_series
 *  44 - two_ln2_series
 *  45 - pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series
 *  46 - pi_minus_x_2
 *  47 - half_multi_ln_1div2multi1minuscosx
 *  48 - half_minus_sinx_multi_pi_4
 *  49 - ln_1plussqrt1plusxsquare_minus_ln_2
 *  50 - ln_cosx
 *  51 - ln_sinx_minus_ln_x
 *  52 - pi_8_cosx_square_minus_1_div_3_cosx
 *  53 - sqrt_oneminussqrtoneminusx_div_x
 *  54 - one_minus_sqrt_1minus4x_div_2x
 *  55 - arcsin_x_minus_x_series
 *  56 - pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series
 *  57 - abs_sin_x_minus_2_div_pi_series
 *  58 - pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series
 *  59 - minus_3_div_4_or_x_minus_3_div_4_series
 *  60 - ten_minus_x_series
 *  61 - x_series
 *  62 - minus_x_minus_pi_4_or_minus_pi_4_series
 *  63 - one_div_two_minus_x_multi_three_plus_x_series
 *  64 - Si_x_series
 *  65 - Ci_x_series
 *  66 - Riemann_zeta_func_series
 *  67 - Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series
 *  68 - xsquareplus3_div_xsquareplus2multix_minus_1_series
 *  69 - arcsin_x_series
 *  70 - arctg_x_series
 *  71 - K_x_series
 *  72 - E_x_series
 *  73 - sqrt_1plusx_series
 *  74 - Lambert_W_func_series
 *  75 - Incomplete_Gamma_func_series
 *  76 - Series_with_ln_number1
 *  77 - Series_with_ln_number2
 *  78 - pi_series
 *  79 - x_min_sqrt_x_series
 *  80 - arctan_x2_series
 *  81 - ln1px4_series
 *  82 - sin_x2_series
 *  83 - arctan_x3_series
 *  84 - arcsin_x2_series
 *  85 - ln1_m_x2_series
 *  86 - artanh_x_series
 *  87 - arcsinh_x_series
 *  88 - cos_x2_series
 *  89 - sinh_x2_series
 *  90 - arctanh_x2_series
 *  91 - cos3xmin1_div_xsqare_series
 *  92 - two_degree_x_series
 *  93 - sqrt_1plusx_min_1_min_x_div_2_series
 *  94 - ln13_min_ln7_div_7_series
 *  95 - Ja_x_series
 *  96 - one_div_sqrt2_sin_xdivsqrt2_series
 *  97 - ln_1plusx_div_1plusx2
 *  98 - cos_sqrt_x
 *  99 - ln_1_plus_x3
 *  100 - x_div_1minx
 *  101 - x_div_1minx2
 *  102 - gamma_series
 * @brief This file contains series base class, reccurent_series_base class and derived classes of various serieses (e.g. exp(x), ch(x))
 */

#pragma once
#include <cmath>
#include <stdexcept>
#define NO_X_GIVEN 0
#define NO_SERIES_EXPRESSION_GIVEN 0
#include <numbers>
#include <vector>
#include <concepts>
#include <string>

using std::isfinite;

 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <std::floating_point T, std::unsigned_integral K>
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
	* @brief factorial k!
	* @authors Bolshakov M.P.
	* @return k!
	*/
	[[nodiscard]] constexpr static const K fact(K k);

	/**
	* @brief double factorial k!!
	* @authors Bolshakov M.P.
	* @return k!!
	*/
	[[nodiscard]] constexpr static const K double_fact(K k);

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


	/**
	* @brief evaluates Euler function by n
	* @authors Trudolyubov N.A.
	* @return phi(n)
	*/
	[[nodiscard]] constexpr static const T phi(K n);

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

template <std::floating_point T, std::unsigned_integral K>
series_base<T, K>::series_base(T x) : x(x), sum(0) {}

template <std::floating_point T, std::unsigned_integral K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T series_base<T, K>::S_n(K n) const
{
	T sum = operator()(n);
	for (K i = 0; i < n; ++i)
		sum += operator()(i);
	return sum;
}

template <std::floating_point T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_x() const
{
	return x;
}

template <std::floating_point T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_sum() const
{
	return sum;
}

template <std::floating_point T, std::unsigned_integral K>
constexpr const K series_base<T, K>::fact(K n)
{
	K f = 1;
	for (K i = 2; i <= n; ++i)
		f *= i;
	return f;
}

template <std::floating_point T, std::unsigned_integral K>
constexpr const K series_base<T, K>::double_fact(K n)
{
	if (n == 0 || n == 1)
		return 1;
	return n * series_base<T, K>::double_fact(n - 2);
}

template <std::floating_point T, std::unsigned_integral K>
constexpr const T series_base<T, K>::binomial_coefficient(const T n, const K k)
{
	T b_c = 1;
	for (K i = 0; i < k; ++i)
		b_c *= (n - static_cast<T>(i)) / (i + 1);
	return b_c;
}

template <std::floating_point T, std::unsigned_integral K>
constexpr const T series_base<T, K>::minus_one_raised_to_power_n(K n)
{
	return static_cast<T>(n & 1 ? -1.0 : 1.0);
}


template <std::floating_point T, std::unsigned_integral K>
constexpr const T series_base<T, K>::phi(K n)
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


/**
* @brief Abstract class for reccurent series
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class reccurent_series_base
{
public:
	/**
	* @brief Parameterized constructor to initialize the reccurent series with function argument
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	reccurent_series_base(T x);

	/**
	* @brief Parameterized constructor to initialize the reccurent series with vector, containing elements of series
	* @authors Kreynin R.G.
	* @param row The first elements of the series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	reccurent_series_base(std::vector<T> row);

	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	virtual T access_row(K n) = 0;

	/**
	* @brief Vector, containing elements of the series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* It's empty by default
	* @authors Kreynin R.G.
	*/
	std::vector<T> series_vector;
};

template <std::floating_point T, std::unsigned_integral K>
reccurent_series_base<T, K>::reccurent_series_base(T x)
{
	this->series_vector.push_back(x);
};

template <std::floating_point T, std::unsigned_integral K>
reccurent_series_base<T, K>::reccurent_series_base(std::vector<T> row)
{
	if (row.size() < 1)
		throw std::domain_error("empty row imput");

	this->series_vector = row;
};


/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class exp_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	exp_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	exp_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the exponent
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the exponent
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;


private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
exp_series<T, K>::exp_series(T x) : series_base<T, K>(x, std::exp(x)), reccurent_series_base<T, K>(T(1)) {}

template <std::floating_point T, std::unsigned_integral K>
T exp_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * this->x / i); // (1.2) [Rows.pdf]

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_series<T, K>::operator()(K n) const
{
	return const_cast<exp_series<T, K>*>(this)->access_row(n);
}

/**
* @brief Maclaurin series of cosine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cos_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	cos_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cos_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
cos_series<T, K>::cos_series(T x) : series_base<T, K>(x, std::cos(x)), reccurent_series_base<T, K>(std::vector<T>{1, T(((-1) * x * x) / 2)}) {}

template <std::floating_point T, std::unsigned_integral K>
T cos_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(a / (i * std::fma(4, i, -2)))); // (2.2) [Rows.pdf]

	return this->series_vector[n];
}


template <std::floating_point T, std::unsigned_integral K>
constexpr T cos_series<T, K>::operator()(K n) const
{
	return const_cast<cos_series<T, K>*>(this)->access_row(n);
}

/**
* @brief Maclaurin series of sine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sin_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	sin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sin_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
sin_series<T, K>::sin_series(T x) : series_base<T, K>(x, std::sin(x)), reccurent_series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
T sin_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(a / (i * std::fma(4, i, 2)))); // (3.2) [Rows.pdf]

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sin_series<T, K>::operator()(K n) const
{
	return const_cast<sin_series<T, K>*>(this)->access_row(n);
}

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cosh_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	cosh_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cosh_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of hyperbolic cosine
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x)), reccurent_series_base<T, K>(std::vector<T>{1, T((x * x) / 2) }) {}

template <std::floating_point T, std::unsigned_integral K>
T cosh_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = static_cast<T>(this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(a / (i * std::fma(4, i, -2)))); // (4.2) [Rows.pdf] 

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cosh_series<T, K>::operator()(K n) const
{
	return const_cast<cosh_series<T, K>*>(this)->access_row(n);
}

/**
* @brief Maclaurin series of hyperbolic sine
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sinh_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	sinh_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sinh_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sinh function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sinh functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
sinh_series<T, K>::sinh_series(T x) : series_base<T, K>(x, std::sinh(x)), reccurent_series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
T sinh_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = static_cast<T>(this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(a / (i * std::fma(4, i, 2)))); // (5.2) [Rows.pdf]

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sinh_series<T, K>::operator()(K n) const
{
	return const_cast<sinh_series<T, K>*>(this)->access_row(n);
}

/**
* @brief Binomial series (Maclaurin series for (1+x)^a function)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class bin_series : public series_base<T, K>
{
	using series_base<T, K>::binomial_coefficient;

public:
	bin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series, alpha The power constant
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	bin_series(T x, T alpha);

	/**
	* @brief Computes the nth term of the Binomial series
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
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

template <std::floating_point T, std::unsigned_integral K>
bin_series<T, K>::bin_series(T x, T alpha) : series_base<T, K>(x, std::pow(1 + x, alpha)), alpha(alpha)
{
	if (std::abs(x) >= 1)
		throw std::domain_error("the bin series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T bin_series<T, K>::operator()(K n) const
{
	return static_cast<T>(binomial_coefficient(alpha, n) * std::pow(this->x, n)); // (6.1) [Rows.pdf]
}

/**
* @brief Maclaurin series of function 4 * artan(x)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class four_arctan_series : public series_base<T, K>
{
public:
	four_arctan_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	four_arctan_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the arctan multiplied by four
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, 4 * std::atan(x))
{
	if (std::abs(x) > 1)
		throw std::domain_error("the 4 * arctan(x) series diverge at x = " + std::to_string(x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T four_arctan_series<T, K>::operator()(K n) const
{
	const K temp = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(4 * series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, temp) / temp); // (7.1) Rows.pdf
}


/**
* @brief Maclaurin series of function -ln(1 - x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln1mx_series : public series_base<T, K>
{
public:
	ln1mx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln1mx_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of -ln(1 - x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln1mx_series<T, K>::ln1mx_series(T x) : series_base<T, K>(x, -std::log(1 - x))
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("the -ln(1 - x) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln1mx_series<T, K>::operator()(K n) const
{
	const K temp = n + 1;
	return static_cast<T>(std::pow(this->x, temp) / temp); // (8.2) [Rows.pdf]
}

/**
* @brief Maclaurin series of function (sinh(x) + sin(x)) / 2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class mean_sinh_sin_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	mean_sinh_sin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	mean_sinh_sin_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of (sinh(x) + sin(x)) / 2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, static_cast<T>(static_cast<T>(0.5) * (std::sinh(x) + std::sin(x)))), reccurent_series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
T mean_sinh_sin_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);
	T a;
	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		a = static_cast<T>(std::fma(4, i, 1));
		this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(std::pow(this->x, 4) / (4 * i * a * (a - 2) * (a - 3)))); // (9.1) [Rows.pdf]
	}
	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T mean_sinh_sin_series<T, K>::operator()(K n) const
{
	return const_cast<mean_sinh_sin_series<T, K>*>(this)->access_row(n);
}


/**
* @brief Maclaurin series of function exp(x^2)*erf(x), where erf(x) is error function of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class exp_squared_erf_series : public series_base<T, K>
{
public:
	exp_squared_erf_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	exp_squared_erf_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of exp(x^2)*erf(x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, std::exp(x * x) * std::erf(x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_squared_erf_series<T, K>::operator()(K n) const
{
	const auto result = std::pow(this->x, std::fma(2, n, 1)) / std::tgamma(n + 1.5); // (10.3) [Rows.pdf]
	if (!isfinite(result))
		throw std::overflow_error("operator() is too big");
	return static_cast<T>(result);
}

/**
* @brief Maclaurin series of function x^(-b) * J_b(2x) where J_b(x) is Bessel function of the first kind of order b
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class xmb_Jb_two_series : public series_base<T, K>
{
public:
	xmb_Jb_two_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series, b The order constant
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	xmb_Jb_two_series(T x, K b);

	/**
	* @brief Computes the nth term of the Maclaurin series of x^(-b) * J_b(2x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
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

template <std::floating_point T, std::unsigned_integral K>
xmb_Jb_two_series<T, K>::xmb_Jb_two_series(T x, K b) : series_base<T, K>(x, static_cast<T>(1 / std::pow(x, b) * std::cyl_bessel_j(b, 2 * x))), mu(b) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T xmb_Jb_two_series<T, K>::operator()(K n) const
{
	return static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, 2 * n) * static_cast<T>(std::tgamma(n + this->mu + 1))
		/ (static_cast<T>(this->fact(n)))); // (11.3) [Rows.pdf]
}

/**
* @brief Maclaurin series of function 0.5 * asin(2x), where asin(x) is inverse sine of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class half_asin_two_x_series : public series_base<T, K>
{
public:
	half_asin_two_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	half_asin_two_x_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of 0.5 * asin(2x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, static_cast<T>(0.5 * std::asin(2 * x)))
{
	if (std::abs(this->x) > static_cast<T>(0.5))
		throw std::domain_error("the 0.5 * asin(2x) series diverge at x = " + std::to_string(x) + "; series converge if x only in [-0.5, 0.5]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_asin_two_x_series<T, K>::operator()(K n) const
{
	const auto _fact_n = this->fact(n);
	const K a = 2 * n;
	return static_cast<T>(static_cast<T>(this->fact(a)) * std::pow(this->x, a) / (_fact_n * _fact_n * (a + 1))); // (12.1) [Rows.pdf]
}


/**
* @brief Maclaurin series of function 1 / (1 - x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class inverse_1mx_series : public series_base<T, K>
{
public:
	inverse_1mx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	inverse_1mx_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of 1 / (1 - x)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
inverse_1mx_series<T, K>::inverse_1mx_series(T x) : series_base<T, K>(x, 1 / (1 - x))
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("the 1 / (1 - x) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T inverse_1mx_series<T, K>::operator()(K n) const
{
	return static_cast<T>(std::pow(this->x, n)); // (13.1) [Rows.pdf]
}

/**
* @brief Maclaurin series of function x / (1 - x)^2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_1mx_squared_series : public series_base<T, K>
{
public:
	x_1mx_squared_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_1mx_squared_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of x / (1 - x)^2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_1mx_squared_series<T, K>::x_1mx_squared_series(T x) : series_base<T, K>(x, x / std::fma(x, x - 1, 1 - x))
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("the 1 / (1 - x)^2 series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_1mx_squared_series<T, K>::operator()(K n) const
{
	return static_cast<T>(std::pow(this->x, n) * n); // (14.4) [Rows.pdf]
}

/**
* @brief Maclaurin series of function 0.5 * sqrt(pi) * erf(x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class erf_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	erf_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	erf_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of sqrt(pi) * erf(x) / 2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
erf_series<T, K>::erf_series(T x) : series_base<T, K>(x, static_cast<T>(std::sqrt(std::numbers::pi)) * static_cast<T>(std::erf(x) / 2)), reccurent_series_base<T, K>(x) { }

template <std::floating_point T, std::unsigned_integral K>
T erf_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);
	T a;

	T b = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) 
	{
		a = static_cast<T>(std::fma(2, i, 1));
		this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(b * (a - 2) / (i * a))); // (15.3) [Rows.pdf]
	}
	return static_cast<T>(this->series_vector[n]);
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T erf_series<T, K>::operator()(K n) const
{
	return const_cast<erf_series<T, K>*>(this)->access_row(n);
}

/**
* @brief Maclaurin series of function m! / (1 - x) ^ (m + 1), where m - integer parameter
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class m_fact_1mx_mp1_inverse_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	m_fact_1mx_mp1_inverse_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series, m The integer constant parameter
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	m_fact_1mx_mp1_inverse_series(T x, K m);

	/**
	* @brief Computes the nth term of the Maclaurin series of  m! / (1 - x) ^ (m + 1)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
private:

	/**
	* @brief The const parameter m of the series
	* @authors Pashkov B.B.
	*/
	const K m;

	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
m_fact_1mx_mp1_inverse_series<T, K>::m_fact_1mx_mp1_inverse_series(T x, K m) : series_base<T, K>(x, static_cast<T>(static_cast<T>(this->fact(m)) / pow(1 - x, m + 1))), m(m), reccurent_series_base<T, K>(static_cast<T>(this->fact(m)))
{
	if (!isfinite(series_base<T, K>::sum)) // sum = this->fact(m) / pow(1 - x, m + 1))
		throw std::overflow_error("sum is too big");

	if (std::abs(this->x) >= 1) 
		throw std::domain_error("the m! / (1 - x) ^ (m + 1) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
T m_fact_1mx_mp1_inverse_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * this->x * (this->m + i) / i); // (16.2) [Rows.pdf]

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T m_fact_1mx_mp1_inverse_series<T, K>::operator()(K n) const
{
	return const_cast<m_fact_1mx_mp1_inverse_series<T, K>*>(this)->access_row(n);
}

/**
* @brief Maclaurin series of function (1 - 4x) ^ (-1/2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class inverse_sqrt_1m4x_series : public series_base<T, K>
{
public:
	inverse_sqrt_1m4x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	inverse_sqrt_1m4x_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of (1 - 4x) ^ (-1/2)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
inverse_sqrt_1m4x_series<T, K>::inverse_sqrt_1m4x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(std::fma(-4, x, 1), -0.5)))
{
	if (std::abs(this->x) >= static_cast<T>(0.25))
		throw std::domain_error("the (1 - 4x) ^ (-1/2) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-0.25, 0.25)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T inverse_sqrt_1m4x_series<T, K>::operator()(K n) const
{
	const auto _fact_n = this->fact(n);
	return this->fact(2 * n) * static_cast<T>(pow(this->x, n) / (_fact_n * _fact_n)); // (17.2) [Rows.pdf]
}

/**
* @brief Trigonometric series of function 1/12 * (3x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_twelfth_3x2_pi2_series : public series_base<T, K>
{
public:
	one_twelfth_3x2_pi2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_twelfth_3x2_pi2_series(T x);

	/**
	* @brief Computes the nth term of the Trigonometric series of 1/12 * (3x^2 - pi^2)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_twelfth_3x2_pi2_series<T, K>::one_twelfth_3x2_pi2_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(0.25 * x, x, -std::numbers::pi * std::numbers::pi / 12)))
{
	if (std::abs(this->x) > std::numbers::pi)
		throw std::domain_error("the 1/12 * (3x^2 - pi^2) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-pi, pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_twelfth_3x2_pi2_series<T, K>::operator()(K n) const
{
	return n ? series_base<T, K>::minus_one_raised_to_power_n(n) * std::cos(n * this->x) / (n * n) : 0; // (18.2) [Rows.pdf]
}

/**
* @brief Trigonometric series of function x/12 * (x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_twelfth_x2_pi2_series : public series_base<T, K>
{
public:
	x_twelfth_x2_pi2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_twelfth_x2_pi2_series(T x);

	/**
	* @brief Computes the nth term of the Trigonometric series of x/12 * (x^2 - pi^2)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_twelfth_x2_pi2_series<T, K>::x_twelfth_x2_pi2_series(T x) : series_base<T, K>(x, static_cast<T>(x / 12 * (x * x - std::numbers::pi * std::numbers::pi)))
{
	if (std::abs(this->x) > std::numbers::pi)
		throw std::domain_error("the x/12 * (x^2 - pi^2) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-pi, pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_twelfth_x2_pi2_series<T, K>::operator()(K n) const
{
	return n ? static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * std::sin(n * this->x) / (std::pow(n, 3))) : 0; // (19.2) [Rows.pdf]
}


/**
* @brief Numerical series representation of ln(2) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln2_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln2_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of ln(2) * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln2_series<T, K>::ln2_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(2) * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln2_series<T, K>::operator()(K n) const
{
	return n ? static_cast<T>((- series_base<T, K>::minus_one_raised_to_power_n(n) * this->x) / n) : 0; // (20.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of 1 * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of 1 * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_series<T, K>::one_series(T x) : series_base<T, K>(x, 1 * x) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_series<T, K>::operator()(K n) const
{
	return n ? static_cast<T>(1.0 * this->x / fma(n, n, n)) : 0; // (21.5) [Rows.pdf]
}

/**
* @brief Numerical series representation of -1/4 * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_one_quarter_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_one_quarter_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of -1/4 * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_one_quarter_series<T, K>::minus_one_quarter_series(T x) : series_base<T, K>(x, static_cast<T>(-0.25 * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_quarter_series<T, K>::operator()(K n) const
{
	return n ? static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / (n * (n + 2))) : 0; // (22.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of pi/3 * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_3_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_3_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of pi/3 * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_3_series<T, K>::pi_3_series(T x) : series_base<T, K>(x, static_cast<T>(std::numbers::pi * x / 3)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_3_series<T, K>::operator()(K n) const
{
	const K a = 2 * n + 1;
	return static_cast<T>(1.0 * this->x / ((n + 1) * a * (2 * a - 1))); // (23.2) [Rows.pdf]	
}

/**
* @brief Numerical series representation of pi/4 * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_4_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_4_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of pi/4 * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_4_series<T, K>::pi_4_series(T x) : series_base<T, K>(x, static_cast<T>(std::numbers::pi * x / 4)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_4_series<T, K>::operator()(K n) const
{
	return (series_base<T, K>::minus_one_raised_to_power_n(n) * this->x) / static_cast<T>(std::fma(2, n, 1)); // (24.3) [Rows.pdf]
}

/**
* @brief Numerical series representation of (pi^2 / 6 - 1) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_squared_6_minus_one_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_squared_6_minus_one_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of (pi^2 / 6 - 1) * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_squared_6_minus_one_series<T, K>::pi_squared_6_minus_one_series(T x) : series_base<T, K>(x, static_cast<T>((std::fma(std::numbers::pi / 6, std::numbers::pi, -1)) * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_squared_6_minus_one_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? (1.0 * this->x) / (n * n * (n + 1)) : 0); // (25.3) [Rows.pdf]
}

/**
* @brief Numerical series representation of (3 - pi) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class three_minus_pi_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	three_minus_pi_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of (3 - pi) * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
three_minus_pi_series<T, K>::three_minus_pi_series(T x) : series_base<T, K>(x, static_cast<T>((3 - std::numbers::pi) * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T three_minus_pi_series<T, K>::operator()(K n) const
{
	return n ? static_cast<T>((series_base<T, K>::minus_one_raised_to_power_n(n) * this-> x) / (n * (n + 1) * (2 * n + 1))) : 0; // (26.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of 1 * x/12
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_twelfth_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_twelfth_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of 1 * x/ 12
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_twelfth_series<T, K>::one_twelfth_series(T x) : series_base<T, K>(x, x / 12) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_twelfth_series<T, K>::operator()(K n) const
{
	const K a = 2 * n + 1;
	return static_cast<T>((1.0 * this->x) / (a * (a + 2) * (a + 4))); // (27.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of (pi/8 - 1/3) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class eighth_pi_m_one_third_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	eighth_pi_m_one_third_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of (pi/8 - 1/3) * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
eighth_pi_m_one_third_series<T, K>::eighth_pi_m_one_third_series(T x) : series_base<T, K>(x, static_cast<T>((std::numbers::pi / 8 - 1 / 3) * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T eighth_pi_m_one_third_series<T, K>::operator()(K n) const
{
	const K a = 2 * n + 1;
	return static_cast<T>((series_base<T, K>::minus_one_raised_to_power_n(n) * this->x) / (a * (a + 2) * (a + 4))); // (28.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of (pi^2 - 9) * x / 3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_third_pi_squared_m_nine_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_third_pi_squared_m_nine_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of (pi^2 - 9) * x / 3
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_third_pi_squared_m_nine_series<T, K>::one_third_pi_squared_m_nine_series(T x) : series_base<T, K>(x, static_cast<T>((std::fma(std::numbers::pi, std::numbers::pi, -9) * x) / 3)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_third_pi_squared_m_nine_series<T, K>::operator()(K n) const
{
	const K a = n * (n + 1);
	return static_cast<T>(n ? (1.0 * this->x) / (a * a) : 0); // (29.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of (4 * ln2 - 3) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class four_ln2_m_3_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	four_ln2_m_3_series(T x);

	/**
	* @brief Computes the nth term of the Numerical series of (4 * ln2 - 3) * x
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
four_ln2_m_3_series<T, K>::four_ln2_m_3_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(4, std::log(2), -3) * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T four_ln2_m_3_series<T, K>::operator()(K n) const
{
	const K a = n * (n + 1);
	return static_cast<T>(n ? (series_base<T, K>::minus_one_raised_to_power_n(n) * this->x) / (a * a) : 0); // (30.2) [Rows.pdf]
}

/**
* @brief Maclaurin series of exp(-cos(x)) * sin(sin(x))
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class exp_m_cos_x_sinsin_x_series : public series_base<T, K>
{
public:
	exp_m_cos_x_sinsin_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	exp_m_cos_x_sinsin_x_series(T x);

	/**
	* @brief Computes the nth term of the exp(-cos(x)) * sin(sin(x)) series
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
exp_m_cos_x_sinsin_x_series<T, K>::exp_m_cos_x_sinsin_x_series(T x) : series_base<T, K>(x, std::exp(-std::cos(x)) * std::sin(std::sin(x))) 
{
	if (std::abs(this->x) >= std::numbers::pi)
		throw std::domain_error("the exp(-cos(x)) * sin(sin(x)) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-pi, pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_m_cos_x_sinsin_x_series<T, K>::operator()(K n) const
{
	return static_cast<T>(this->minus_one_raised_to_power_n(n) * std::sin(n * this->x) / this->fact(n)); // (31.1) [Rows.pdf]
}

/**
* @brief Numerical series representation of (pi/4 - ln2/2) * x 
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_four_minus_ln2_halfed_series : public series_base<T, K>
{
public:
	pi_four_minus_ln2_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_four_minus_ln2_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_four_minus_ln2_halfed_series<T, K>::pi_four_minus_ln2_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(x * static_cast<T>((std::numbers::pi / 4 - std::log(2) / 2)))) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_four_minus_ln2_halfed_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 2))) / n : 0); // (32.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of (5pi/12) * x
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class five_pi_twelve_series : public series_base<T, K>
{
public:
	five_pi_twelve_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	five_pi_twelve_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
five_pi_twelve_series<T, K>::five_pi_twelve_series(T x) : series_base<T, K>(x, static_cast<T>(x * 5 * std::numbers::pi / 12)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T five_pi_twelve_series<T, K>::operator()(K n) const
{
	return static_cast<T>(this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 3))) / std::fma(2, n, 1)); // (33.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of x / 2 
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_two_series : public series_base<T, K>
{
public:
	x_two_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_two_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_two_series<T, K>::x_two_series(T x) : series_base<T, K>(x, x / 2) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_two_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(this->x / (a * (a + 2))); // (34.2) [Rows.pdf]
}

/**
* @brief Numerical series representation of (pi/5 - 1/2) * x 
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_six_min_half_series : public series_base<T, K>
{
public:
	pi_six_min_half_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_six_min_half_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_six_min_half_series<T, K>::pi_six_min_half_series(T x) : series_base<T, K>(x, static_cast<T>(x * (std::numbers::pi / 6 - 0.5))) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_six_min_half_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(6, n, 5));
	return static_cast<T>(this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / (a * (a + 2))); // (35.2) [Rows.pdf]
}


/**
* @brief Numerical series representation of x/2
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_two_throught_squares_series : public series_base<T, K>
{
public:
	x_two_throught_squares_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_two_throught_squares_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_two_throught_squares_series<T, K>::x_two_throught_squares_series(T x) : series_base<T, K>(x, x / 2) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_two_throught_squares_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? (this->x * std::fma(2, n * n, -1)) / (std::fma(4, std::pow(n, 4), 1)) : 0); // (36.2) [Rows.pdf]
}


/**
* @brief Numerical series (-1)^n/n^n = -0,78343051
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_one_ned_in_n_series : public series_base<T, K>
{
public:
	minus_one_ned_in_n_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_one_ned_in_n_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_one_ned_in_n_series<T, K>::minus_one_ned_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(-0.78343051 * x)) {} // -0.78343051 value only for initialization, we can calculate with more accuracy with operator()(K n)

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_ned_in_n_series<T, K>::operator()(K n) const
{
	return n ? this->x * static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) / std::pow(n, n)) : 0; // (37.2) [Rows.pdf]
}

/**
* @brief  Numerical series (-1)^n n! / n^n = -0.65583160
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_one_n_fact_n_in_n_series : public series_base<T, K>
{
public:
	minus_one_n_fact_n_in_n_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_one_n_fact_n_in_n_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_one_n_fact_n_in_n_series<T, K>::minus_one_n_fact_n_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(-0.65583160 * x)) {} // -0.65583160 value only for initialization, we can calculate with more accuracy with operator()(K n)

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_n_fact_n_in_n_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? (this->x * this->fact(n) * series_base<T, K>::minus_one_raised_to_power_n(n)) / std::pow(n, n) : 0); // (38.2) [Rows.pdf]
}

/**
* @brief Maclaurin series of function ln((x+1)/(1-x))
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_x_plus_one_x_minus_one_halfed_series : public series_base<T, K>
{
public:
	ln_x_plus_one_x_minus_one_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_x_plus_one_x_minus_one_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_x_plus_one_x_minus_one_halfed_series<T, K>::ln_x_plus_one_x_minus_one_halfed_series(T x) : series_base<T, K>(x, std::log((1 + x) / (1 - x)) / 2)
{
	if (std::abs(x) >= 1)
		throw std::domain_error("the ln((x+1)/(1-x)) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_x_plus_one_x_minus_one_halfed_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(std::pow(this->x, a) / a); // (39.3) [Rows.pdf] 
}

/**
* @brief Maclaurin series of function 2 arcsin(x/2)^2
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class two_arcsin_square_x_halfed_series : public series_base<T, K>
{
public:
	two_arcsin_square_x_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	two_arcsin_square_x_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(2 * std::pow(std::asin(x / 2), 2))) 
{
	if (std::abs(this->x) > 2)
		throw std::domain_error("the 2arcsin(x/2)^2 series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-2, 2]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_arcsin_square_x_halfed_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 2));
	return  static_cast<T>((pow(this->x, a) * this->fact(n) * this->fact(n)) / this->fact(a)); // (40.3) [Rows.pdf]
}


/**
* @brief Numerical series representation of (pi^2/12) * x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_squared_twelve_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_squared_twelve_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_squared_twelve_series<T, K>::pi_squared_twelve_series(T x) : series_base<T, K>(x, static_cast<T>((x * std::numbers::pi * std::numbers::pi) / 12)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_squared_twelve_series<T, K>::operator()(K n) const
{
	T n_temp = static_cast<T>(n + 1);
	return (this->x * this->minus_one_raised_to_power_n(n)) / (n_temp * n_temp); // (41.1) [Rows.pdf]
}

/**
* @brief Numerical series representation of (pi^3/32) * x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_cubed_32_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_cubed_32_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_cubed_32_series<T, K>::pi_cubed_32_series(T x) : series_base<T, K>(0, static_cast<T>(x * std::pow(std::numbers::pi, 3) / 32)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_cubed_32_series<T, K>::operator()(K n) const
{
	T n_temp = static_cast<T>(n + 1);
	return this->x * this->minus_one_raised_to_power_n(n) / static_cast<T>((std::pow(std::fma(2, n_temp, 1), 3))); // (42.2) [Rows.pdf]
}


/**
* @brief Numerical series representation of (-3 + (3/2)*ln3 + 2*ln2) * x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_three_plus_ln3_three_devided_two_plus_two_ln2_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_three_plus_ln3_three_devided_two_plus_two_ln2_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series(T x) : series_base<T, K>(x, x * static_cast<T>(-3 + (3 / 2) * std::log(3) + 2 * std::log(2))) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::operator()(K n) const
{
	T n_temp = static_cast<T>(n + 1);
	return static_cast<T>(this->x / (n_temp * (std::fma(36, n_temp * n_temp, -1))));  // (43.2) [Rows.pdf]
}


/**
* @brief Numerical series representation of 2*ln2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class two_ln2_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	two_ln2_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
two_ln2_series<T, K>::two_ln2_series(T x) : series_base<T, K>(x, x * static_cast<T>(2 * std::log(2))) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_ln2_series<T, K>::operator()(K n) const
{
	T n_temp = static_cast<T>(n + 1);
	T a = n_temp * n_temp * 4 - 1;
	return (3 * a + 2) / (n_temp * a * a); // (44.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function pi * x * (e^(pi*x) + e^(-pi*x)) / (e^(pi*x) - e^(-pi*x)) - 1
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series : public series_base<T, K>
{
public:
	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series(T x) : series_base<T, K>(x, static_cast<T>(std::numbers::pi* x * 2.0 * std::cosh(std::numbers::pi * x) / (2.0 * std::sinh(std::numbers::pi * x)) - 1)) 
{
	if (this->x == 0) // if x = 0 then series turns in 0
		throw std::domain_error("x cannot be zero");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const T b = (this->x) * (this->x);
	return static_cast<T>(2 * b / (b + a * a)); // (45.4) [Rows.pdf]
}


/**
* @brief Trigonometric series of function (pi - x) / 2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_minus_x_2 : public series_base<T, K>
{
public:
	pi_minus_x_2() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_minus_x_2(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_minus_x_2<T, K>::pi_minus_x_2(T x) : series_base<T, K>(x, static_cast<T>((std::numbers::pi - x) / 2)) 
{
	if (this->x <= 0 or this->x >= 2 * std::numbers::pi)
		throw std::domain_error("the (pi - x) / 2 series diverge at x = " + std::to_string(this->x) + "; series converge if x only in (0, 2pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_minus_x_2<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return std::sin(a * (this->x)) / a;  // (46.5) [Rows.pdf]
}


/**
* @brief Trigonometric series of function 0.5 * ln(1/(2*(1 - cos(x))))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class half_multi_ln_1div2multi1minuscosx : public series_base<T, K>
{
public:
	half_multi_ln_1div2multi1minuscosx() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	half_multi_ln_1div2multi1minuscosx(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
half_multi_ln_1div2multi1minuscosx<T, K>::half_multi_ln_1div2multi1minuscosx(T x) : series_base<T, K>(x, static_cast<T>(0.5 * std::log(1 / (2 - 2 * std::cos(x))))) 
{
	if (this->x <= 0 || this->x >= 2 * std::numbers::pi)
		throw std::domain_error("the 0.5 * ln(1/(2*(1 - cos(x)))) series diverge at x = " + std::to_string(x) + "; series converge if x only in (0, 2pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_multi_ln_1div2multi1minuscosx<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return std::cos(a * (this->x)) / a; // (47.5) [Rows.pdf]
}


/**
* @brief Trigonometric series of function 0.5 - (pi/4)*sin(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class half_minus_sinx_multi_pi_4 : public series_base<T, K>
{
public:
	half_minus_sinx_multi_pi_4() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	half_minus_sinx_multi_pi_4(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
half_minus_sinx_multi_pi_4<T, K>::half_minus_sinx_multi_pi_4(T x) : series_base<T, K>(x, static_cast<T>((static_cast<T>(0.5) - std::numbers::pi * std::sin(x) * static_cast<T>(0.25)))) 
{
	if (this->x < 0 || this->x > std::numbers::pi / 2)
		throw std::domain_error("the 0.5 - (pi/4)*sin(x) series diverge at x = " + std::to_string(x) + "; series converge if x only in [0, pi/2]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_minus_sinx_multi_pi_4<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return std::cos(2 * (this->x) * (n + 1)) / (a * (a + 2)); // (48.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function ln(1 + sqrt(1 + x^2)) - ln(2)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_1plussqrt1plusxsquare_minus_ln_2 : public series_base<T, K>
{
public:
	ln_1plussqrt1plusxsquare_minus_ln_2() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_1plussqrt1plusxsquare_minus_ln_2(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_1plussqrt1plusxsquare_minus_ln_2<T, K>::ln_1plussqrt1plusxsquare_minus_ln_2(T x) : series_base<T, K>(x, static_cast<T>(std::log((1 + std::hypot(1, x)) / 2))) 
{
	if ((this->x) * (this->x) > 1)
		throw std::domain_error("x^2 cannot be more than 1");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1plussqrt1plusxsquare_minus_ln_2<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = static_cast<K>(std::fma(2, n, 2));
	return static_cast<T>((this->minus_one_raised_to_power_n(n) * this->fact(b - 1) * std::pow(this->x, b)) / 
		(pow(2, b) * this->fact(a) * this->fact(a))); // (49.3) [Rows.pdf]
}


/**
* @brief Maclaurin series of function ln(cos(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_cosx : public series_base<T, K>
{
public:
	ln_cosx() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_cosx(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_cosx<T, K>::ln_cosx(T x) : series_base<T, K>(x, std::log(std::cos(x))) 
{
	if (std::abs(this->x) * 2 >= std::numbers::pi)
		throw std::domain_error("the ln(cos(x)) series diverge at x = " + std::to_string(x) + "; series converge if x only in [-pi/2, pi/2]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_cosx<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 2));
	return static_cast<T>(-(std::pow(std::sin(this->x), a)) / a); // (50.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function ln(sin(x)) - ln(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_sinx_minus_ln_x : public series_base<T, K>
{
public:
	ln_sinx_minus_ln_x() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_sinx_minus_ln_x(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_sinx_minus_ln_x<T, K>::ln_sinx_minus_ln_x(T x) : series_base<T, K>(x, std::log(std::sin(x) / x)) 
{
	if (this->x <= 0 || this->x > std::numbers::pi)
		throw std::domain_error("the ln(sin(x)) - ln(x) series diverge at x = " + std::to_string(x) + "; series converge if x only in (0, pi]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_sinx_minus_ln_x<T, K>::operator()(K n) const
{
	const T a = static_cast<T>((this->x) / ((n + 1) * std::numbers::pi));
	return static_cast<T>(std::log(1 - a * a)); // (51.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function (pi/8)*cos^2(x) - (1/3)*cos(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_8_cosx_square_minus_1_div_3_cosx : public series_base<T, K>
{
public:
	pi_8_cosx_square_minus_1_div_3_cosx() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_8_cosx_square_minus_1_div_3_cosx(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_8_cosx_square_minus_1_div_3_cosx<T, K>::pi_8_cosx_square_minus_1_div_3_cosx(T x) : series_base<T, K>(x, static_cast<T>((std::numbers::pi / 8) * std::cos(x) * std::cos(x) - ((1 / 3) * std::cos(x)))) 
{
	if (abs(this->x) > std::numbers::pi / 2)
		throw std::domain_error("the (pi/8)*cos^2(x) - (1/3)*cos(x) series diverge at x = " + std::to_string(x) + "; series converge if x only in [-pi/2, pi/2]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_8_cosx_square_minus_1_div_3_cosx<T, K>::operator()(K n) const
{
	const K temp = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(this->minus_one_raised_to_power_n(n) * std::cos(temp + 2) * this->x / (temp * (temp + 2) * (temp + 4))); // (52.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function sqrt((1 - sqrt(1 - x)) / x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sqrt_oneminussqrtoneminusx_div_x : public series_base<T, K>
{
public:
	sqrt_oneminussqrtoneminusx_div_x() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sqrt_oneminussqrtoneminusx_div_x(T x);

	/**
	* @brief Computes the nth term of the series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
sqrt_oneminussqrtoneminusx_div_x<T, K>::sqrt_oneminussqrtoneminusx_div_x(T x) : series_base<T, K>(x, std::sqrt((1 - sqrt(1 - x)) / x)) 
{
	if (std::abs(this->x) >= 1 or this->x == 0)
		throw std::domain_error("the sqrt((1 - sqrt(1 - x)) / x) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 0) and (0, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sqrt_oneminussqrtoneminusx_div_x<T, K>::operator()(K n) const
{
	const K a = 2 * n;
	const K b = 2 * a;
	return static_cast<T>(this->fact(b) * pow(this->x, n) / (pow(2, b) * sqrt(2) * (this->fact(a)) * (this->fact(a + 1)))); // (53.1) [Rows.pdf]
}


/**
* @brief Series of function (1 - sqrt(1 - 4x)) / 2x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_minus_sqrt_1minus4x_div_2x : public series_base<T, K>
{
public:
	one_minus_sqrt_1minus4x_div_2x() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_minus_sqrt_1minus4x_div_2x(T x);

	/**
	* @brief Computes the nth term of the series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_minus_sqrt_1minus4x_div_2x<T, K>::one_minus_sqrt_1minus4x_div_2x(T x) : series_base<T, K>(x, (1 - static_cast<T>(sqrt(std::fma(-4, x, 1)))) / (2 * x)) 
{
	if (std::abs(this->x) > static_cast<T>(0.25) or this->x == 0)
		throw std::domain_error("the  (1 - sqrt(1 - 4x)) / 2x series diverge at x = " + std::to_string(x) + "; series converge if x only in [-1/4, 0) and (0, 1/4]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_minus_sqrt_1minus4x_div_2x<T, K>::operator()(K n) const
{
	const T xn = static_cast<T>(std::pow(this->x, n));
	const T binom_coef = static_cast<T>(this->binomial_coefficient(static_cast<T>(0.5), n + 1));
	const T power_2 = static_cast<T>(std::pow(2, std::fma(2, n, 1)));

	return power_2 * binom_coef * xn; // (54.1) [Rows.pdf]
}


/**
* @brief Maclaurin series of function arcsin(x) - x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arcsin_x_minus_x_series : public series_base<T, K>
{
public:
	arcsin_x_minus_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	arcsin_x_minus_x_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arcsin_x_minus_x_series<T, K>::arcsin_x_minus_x_series(T x) : series_base<T, K>(x, std::asin(x) - x) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the arcsin(x) - x series diverge at x = " + std::to_string(x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsin_x_minus_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>((this->double_fact(a) * std::pow(this->x, a+2)) / (this->double_fact(a+1) * (a+2))); // (55.3) [Rows.pdf]
}


/**
* @brief Fourier series of system functions pi*x - x^2, 0 < x < pi
*											x^2 - 3*pi*x + 2*pi^2, pi < x < 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series : public series_base<T, K>
{
public:
	pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x) : series_base<T, K>(x) 
{
	if (this->x <= 0 || this->x >= 2 * std::numbers::pi)
		throw std::domain_error("the series diverge at x = " + std::to_string(x) + "; series converge if x only in (0, 2pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>((8 / std::numbers::pi) * (std::sin(a * (this->x)) / std::pow(a, 3))); // (56.2) [Rows.pdf]
}


/**
* @brief Fourier series of system functions sin(x) - 2/pi, 0 <= x <= pi
*											-sin(x) - 2/pi, pi <= x <= 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class abs_sin_x_minus_2_div_pi_series : public series_base<T, K>
{
public:
	abs_sin_x_minus_2_div_pi_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	abs_sin_x_minus_2_div_pi_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
abs_sin_x_minus_2_div_pi_series<T, K>::abs_sin_x_minus_2_div_pi_series(T x) : series_base<T, K>(x, static_cast<T>(std::abs(std::sin(x)) - (2 / std::numbers::pi))) 
{
	if (this->x <= 0 || this->x >= 2 * std::numbers::pi)
		throw std::domain_error("the series diverge at x = " + std::to_string(x) + "; series converge if x only in (0, 2pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T abs_sin_x_minus_2_div_pi_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(-4 * (std::cos((this->x) * (a + 1))) / (a * (a + 2) * std::numbers::pi)); // (57.2) [Rows.pdf]
}


/**
* @brief Fourier series of system functions pi - 3*pi/4, -pi < x < 0
*											pi - x - 3*pi/4, 0 <= x <= pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series : public series_base<T, K>
{
public:
	pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(T x) : series_base<T, K>(x) 
{
	if (this->x <= -std::numbers::pi || this->x > std::numbers::pi)
		throw std::domain_error("the series diverge at x = " + std::to_string(x) + "; series converge if x only in (-pi, pi]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = 1 - 2 * (a & 1); // (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
	const T c = static_cast<T>(a * (this->x));
	return static_cast<T>((std::cos(c) * (1 - b)) / (a * a * std::numbers::pi) + b * std::sin(c) / a); // (58.5) [Rows.pdf]
}


/**
* @brief Fourier series of system functions f(x) = -3/4, -3 < x <= 0
*											f(x) = x - 3/4, 0 < x < 3
*											f(x + 6) = f(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_3_div_4_or_x_minus_3_div_4_series : public series_base<T, K>
{
public:
	minus_3_div_4_or_x_minus_3_div_4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_3_div_4_or_x_minus_3_div_4_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_3_div_4_or_x_minus_3_div_4_series<T, K>::minus_3_div_4_or_x_minus_3_div_4_series(T x) : series_base<T, K>(x) 
{
	if (this->x <= -3 || this->x >= 3)
		throw std::domain_error("the series diverge at x = " + std::to_string(x) + "; series converge if x only in (-3, 3)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_3_div_4_or_x_minus_3_div_4_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = a + n; // 2 * n + 1
	const T c = static_cast<T>((this->x) * std::numbers::pi / 3);
	const T d = static_cast<T>(std::numbers::pi * b);
	return static_cast<T>(-6 * std::cos(b * c) / (d * d) - 3 * (1 - 2 * (a & 1)) * std::sin(a * c) / (a * std::numbers::pi));  // (59.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}


/**
* @brief Fourier series of function f(x) = 10 - x, 5 < x < 15
*									f(x + 10) = f(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ten_minus_x_series : public series_base<T, K>
{
public:
	ten_minus_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ten_minus_x_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ten_minus_x_series<T, K>::ten_minus_x_series(T x) : series_base<T, K>(x, 10 - this->x) 
{
	if (this->x <= 5 || this->x >= 15)
		throw std::domain_error("the series diverge at x = " + std::to_string(x) + "; series converge if x only in (5, 15)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ten_minus_x_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const T b = static_cast<T>(a * std::numbers::pi);
	return static_cast<T>((10 - 20 * (a & 1)) * std::sin(b * (this->x) / 5) / b); // (60.4) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}


/**
* @brief Fourier series of function x, -pi <= x <= pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_series : public series_base<T, K>
{
public:
	x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_series<T, K>::x_series(T x) : series_base<T, K>(x, this->x) 
{
	if (this->x < -std::numbers::pi || this->x > std::numbers::pi)
		throw std::domain_error("the series diverge at x = " + std::to_string(x) + "; series converge if x only in [-pi, pi]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return static_cast<T>((2 - 4 * (a & 1)) * std::sin(this->x * a) / a); // (61.4) [Rows.pdf], (1 - 2 * ((n + 1) & 2)) = (-1)^{n+1}
}


/**
* @brief Fourier series of system functions -x, -pi < x <= 0
*											 0,   0 < x < pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_x_minus_pi_4_or_minus_pi_4_series : public series_base<T, K>
{
public:
	minus_x_minus_pi_4_or_minus_pi_4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_x_minus_pi_4_or_minus_pi_4_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::minus_x_minus_pi_4_or_minus_pi_4_series(T x) : series_base<T, K>(x) 
{
	if (this->x <= -std::numbers::pi or this->x >= std::numbers::pi)
		throw std::domain_error("the series diverge at x = " + std::to_string(x) + "; series converge if x only in (-pi, pi)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(-2 * std::cos(a * this->x) / (std::numbers::pi * b * b)
		+ (1 - 2 * (a & 1)) * std::sin(this->x * a) / a); // (62.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}


/**
* @brief Laurent series of function 1 / ((2 - x)*(3 + x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_div_two_minus_x_multi_three_plus_x_series : public series_base<T, K>
{
public:
	one_div_two_minus_x_multi_three_plus_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_div_two_minus_x_multi_three_plus_x_series(T x);

	/**
	* @brief Computes the nth term of the Laurent series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Laurent series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_div_two_minus_x_multi_three_plus_x_series<T, K>::one_div_two_minus_x_multi_three_plus_x_series(T x) : series_base<T, K>(x, 1 / ((2 - this->x) * (3 + this->x))) 
{
	if (std::abs(this->x) >= 2)
		throw std::domain_error("the 1 / ((2 - x)*(3 + x)) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-2, 2)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_div_two_minus_x_multi_three_plus_x_series<T, K>::operator()(K n) const
{
	return static_cast<T>((1 / 5) * std::pow(this->x, n) * (std::pow(6, -1 - n) * (this->minus_one_raised_to_power_n(n) * std::pow(2, n + 1) + std::pow(3, n + 1)))); //(63.1) [Rows.pdf]
}


/**
* @brief Maclaurin series of function Si(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Si_x_series : public series_base<T, K>
{
public:
	Si_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Si_x_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Si_x_series<T, K>::Si_x_series(T x) : series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Si_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, a) / (a * this->fact(a))); // (64.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}


/**
* @brief Maclaurin series of function Ci(x)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Ci_x_series : public series_base<T, K>
{
public:
	Ci_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Ci_x_series(T x);

	/**
	* @brief Computes the nth term of the integral cos series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the integral cos series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	const T gamma = static_cast<T>(0.57721566490153286060); // the Euler–Mascheroni constant
};

template <std::floating_point T, std::unsigned_integral K>
Ci_x_series<T, K>::Ci_x_series(T x) : series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Ci_x_series<T, K>::operator()(K n) const
{
	if (n == 0)
		return gamma + std::log(this->x); // (65.1) [Rows.pdf]

	const K a = 2 * n;
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, a) / (a * this->fact(a))); // (65.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}


/**
* @brief Dirichlet series of Riemann zeta function
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Riemann_zeta_func_series : public series_base<T, K>
{
public:
	Riemann_zeta_func_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Riemann_zeta_func_series(T x);

	/**
	* @brief Computes the nth term of the Dirichlet series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Dirichlet series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Riemann_zeta_func_series<T, K>::Riemann_zeta_func_series(T x) : series_base<T, K>(x) 
{
	if (this->x <= 1)
		throw std::domain_error("The value x must be greater than 1");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Riemann_zeta_func_series<T, K>::operator()(K n) const
{
	return static_cast<T>(1 / std::pow(n + 1, this->x)); // (66.2) [Rows.pdf]
}


/**
* @brief Dirichlet series of Riemann zeta function of x-1 divided by Riemann zeta function of x (R(x-1) / R(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series : public series_base<T, K>
{
public:
	Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x);

	/**
	* @brief Computes the nth term of the Dirichlet series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Dirichlet series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x) : series_base<T, K>(x) 
{
	if (this->x <= 2)
		throw std::domain_error("The value x must be greater than 2");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return static_cast<T>(this->phi(a) / std::pow(a, this->x)); // (67.3) [Rows.pdf]
}


/**
* @brief Taylor series of function ((x^2 + 3) / (x^2 + 2*x)) - 1 at x = 1
* @authors Trudolyubov N.A.
* @tparam T - type of the elements in the series, K - type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class xsquareplus3_div_xsquareplus2multix_minus_1_series : public series_base<T, K>
{
public:
	xsquareplus3_div_xsquareplus2multix_minus_1_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x - argument for function series
	*/
	xsquareplus3_div_xsquareplus2multix_minus_1_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n - number of term
	* @return nth term of Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::xsquareplus3_div_xsquareplus2multix_minus_1_series(T x) : series_base<T, K>(x, ((x * x + 3) / x * (x + 2)) - 1) 
{
	if (std::abs(this->x - 1) >= 2)
		throw std::domain_error("the ((x^2 + 3) / (x^2 + 2*x)) - 1 series diverge at x = " + std::to_string(x) + "; series converge if x only in (-2, 2)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::operator()(K n) const
{
	return static_cast<T>(0.5 * this->minus_one_raised_to_power_n(n) / std::pow(3, n + 1) * (std::pow(3, n + 2) - 7) * std::pow(this->x - 1, n)); // (68.1) [Rows.pdf]
}


/**
* @brief Taylor series of function arcsin(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arcsin_x_series : public series_base<T, K>
{
public:
	arcsin_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	arcsin_x_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arcsin_x_series<T, K>::arcsin_x_series(T x) : series_base<T, K>(x, std::asin(x)) 
{
	if (this->x < -1 || this->x > 1)
		throw std::domain_error("the arcsin(x) series diverge at x = " + std::to_string(x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsin_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(this->fact(a - 1) * std::pow(this->x, a) / (std::pow(4, n) * this->fact(n) * this->fact(n) * a)); // (69.1) [Rows.pdf]
}


/**
* @brief Maclaurin series of function arctg(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arctg_x_series : public series_base<T, K>
{
public:
	arctg_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	arctg_x_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arctg_x_series<T, K>::arctg_x_series(T x) : series_base<T, K>(x, std::atan(x)) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the atan(x) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arctg_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, a) / a); // (70.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}


/**
* @brief Taylor series of Complete elliptic integral of first kind K
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class K_x_series : public series_base<T, K>
{
public:
	K_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	K_x_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
K_x_series<T, K>::K_x_series(T x) : series_base<T, K>(x) 
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("the K(x) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T K_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 2));
	return static_cast<T>((std::numbers::pi * this->double_fact(a - 1) * this->double_fact(a - 1) * std::pow(this->x, a)) / (2 * this->double_fact(a) * this->double_fact(a))); // (71.2) [Rows.pdf]
}


/**
* @brief Taylor series of Complete elliptic integral of second kind E
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class E_x_series : public series_base<T, K>
{
public:
	E_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	E_x_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
E_x_series<T, K>::E_x_series(T x) : series_base<T, K>(x) 
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("the E(x) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T E_x_series<T, K>::operator()(K n) const
{
	const K a = 2 * n;
	return static_cast<T>(std::numbers::pi * this->fact(a) * this->fact(a) * std::pow(this->x, a) / (2 * (1 - a) * std::pow(16, n) * std::pow(this->fact(n), 4))); // (72.1) [Rows.pdf]
}


/**
* @brief Taylor series of function sqrt(1 + x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sqrt_1plusx_series : public series_base<T, K>
{
public:
	sqrt_1plusx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	sqrt_1plusx_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
sqrt_1plusx_series<T, K>::sqrt_1plusx_series(T x) : series_base<T, K>(x, std::sqrt(1 + x)) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the sqrt(1 + x) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sqrt_1plusx_series<T, K>::operator()(K n) const
{
	const K a = 2 * n;
	return static_cast<T>((1 - 2 * (n & 1)) * (this->fact(a)) * std::pow(this->x, n) / ((1 - a) * (this->fact(n)) * (this->fact(n)) * std::pow(4, n))); // (73.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}


/**
* @brief Taylor series of Lambert W function
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Lambert_W_func_series : public series_base<T, K>
{
public:
	Lambert_W_func_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Lambert_W_func_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Lambert_W_func_series<T, K>::Lambert_W_func_series(T x) : series_base<T, K>(x) 
{
	if (std::abs(this->x) >= 1 / std::numbers::e)
		throw std::domain_error("The absolute value of x must be less 1/e");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Lambert_W_func_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(a, n) * std::pow(this->x, a) / this->fact(a)); // (74.2) [Rows.pdf]
}


/**
* @brief Taylor series of Incomplete Gamma function
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Incomplete_Gamma_func_series : public series_base<T, K>
{
public:
	Incomplete_Gamma_func_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Incomplete_Gamma_func_series(T x, T s);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:

	/**
	* @brief The parameter s of the series
	* @authors Trudolyubov N.A.
	*/
	const T s;
};

template <std::floating_point T, std::unsigned_integral K>
Incomplete_Gamma_func_series<T, K>::Incomplete_Gamma_func_series(T x, T s) : series_base<T, K>(x), s(s) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Incomplete_Gamma_func_series<T, K>::operator()(K n) const
{
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, this->s + n) / (this->fact(n) * (this->s + n))); // (75.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}


/**
* @brief Series with ln number1
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Series_with_ln_number1 : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Series_with_ln_number1(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Series_with_ln_number1<T, K>::Series_with_ln_number1(T x) : series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Series_with_ln_number1<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = a * a;
	return this->x * static_cast<T>(std::log(1 + std::pow(a, b + a / 2) / (std::pow(this->fact(a), a) * std::pow(std::numbers::e, b)))); //(76.2) [Rows.pdf]
}


/**
* @brief Series with ln number2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Series_with_ln_number2 : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Series_with_ln_number2(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Series_with_ln_number2<T, K>::Series_with_ln_number2(T x) : series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Series_with_ln_number2<T, K>::operator()(K n) const
{
	auto tmp = std::log(n + 2);
	return this->x / static_cast<T>((std::pow(tmp, tmp))); // (77.2) [Rows.pdf]
}


/**
* @brief Taylor series of number pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_series<T, K>::pi_series(T x) : series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_series<T, K>::operator()(K n) const
{
	return this->x * static_cast<T>(std::sqrt(12) * (1 - 2 * ((n + 1) & 1)) / (std::pow(3, n) * std::fma(2, n, 1))); // (78.1) [Rows.pdf], (n + 1) % 2 is the same (n + 1) & 1
}


/**
* @brief Taylor series of function x - sqrt(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_min_sqrt_x_series : public series_base<T, K>
{
public:
	x_min_sqrt_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_min_sqrt_x_series(T x);

	/**	return this->x * static_cast<T>(std::sqrt(12) * (1 - 2 * ((n + 1) % 2)) / (std::pow(3, n) * (2 * n + 1))); // (78.1) [Rows.pdf]

	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_min_sqrt_x_series<T, K>::x_min_sqrt_x_series(T x) : series_base<T, K>(x, x - std::sqrt(x)) 
{
	if (this->x <= 0 || this->x >= 1)
		throw std::domain_error("the series diverge at x = " + std::to_string(this->x) + "; series converge if x only in (0, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_min_sqrt_x_series<T, K>::operator()(K n) const
{
	if (n == 0 || n == 1)
		return static_cast<T>((1 - this->binomial_coefficient(static_cast<T>(0.5), n)) * std::pow(-1 + this->x, n)); // (79.1) [Rows.pdf]
	return static_cast<T>((-this->binomial_coefficient(static_cast<T>(0.5), n)) * std::pow(-1 + this->x, n)); // (79.1) [Rows.pdf]
} 


/**
* @brief Maclaurin series of function arctan(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arctan_x2_series : public series_base<T, K>
{
public:
	arctan_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	arctan_x2_series(T x);

	/**
	* @brief Computes the nth term of the arctan_x2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the arctan_x2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arctan_x2_series<T, K>::arctan_x2_series(T x) : series_base<T, K>(x, std::atan(x * x)) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the atan(x^2) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arctan_x2_series<T, K>::operator()(K n) const
{
	const K a = 2 * n + 1;
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, 2 * a) / a); // (80.2) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}


/**
* @brief Maclaurin series of function ln(1+x^4)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln1px4_series : public series_base<T, K>
{
public:
	ln1px4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln1px4_series(T x);

	/**
	* @brief Computes the nth term of the ln1px4 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the ln1px4 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln1px4_series<T, K>::ln1px4_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(1 + std::pow(x, 4)))) 
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("the ln(1 + x^4) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln1px4_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return static_cast<T>((1 - 2 * ((a + 1) & 1)) * std::pow(this->x, 4 * a) / a); // (81.3) [Rows.pdf], (1 - 2 * ((a + 1) & 1)) = (-1)^{a + 1}
}


/**
* @brief Maclaurin series of function sin(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sin_x2_series : public series_base<T, K>
{
public:
	sin_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sin_x2_series(T x);

	/**
	* @brief Computes the nth term of the ln1px4 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the ln1px4 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
sin_x2_series<T, K>::sin_x2_series(T x) : series_base<T, K>(x, std::sin(x * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sin_x2_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, 2 * a) / this->fact(a)); // (82.2) [Rows.pdf], // (1 - 2 * (n & 1)) = (-1)^{n}
}

/**
* @brief Maclaurin series of function arctan(x^3)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arctan_x3_series : public series_base<T, K>
{
public:
	arctan_x3_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	arctan_x3_series(T x);

	/**
	* @brief Computes the nth term of the arctanx3 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the arctanx3 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arctan_x3_series<T, K>::arctan_x3_series(T x) : series_base<T, K>(x, static_cast<T>(std::atan(std::pow(x, 3)))) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the atan(x^3) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arctan_x3_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(((1 - 2 * (n & 1)) * std::pow(this->x, 3 * a)) / a); // (83.2) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}

/**
* @brief Maclaurin series of function arcsin(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arcsin_x2_series : public series_base<T, K>
{
public:
	arcsin_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	arcsin_x2_series(T x);

	/**
	* @brief Computes the nth term of the arcsinx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the arcsinx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arcsin_x2_series<T, K>::arcsin_x2_series(T x) : series_base<T, K>(x, std::asin(x * x)) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the asin(x^2) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsin_x2_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>((this->fact(a - 1) * std::pow(this->x, 2 * a)) / (std::pow(4, n) * std::pow(this->fact(n), 2) * a)); // (84.1) [Rows.pdf]
}


/**
* @brief Maclaurin series of function ln(1 - x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln1_m_x2_series : public series_base<T, K>
{
public:
	ln1_m_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln1_m_x2_series(T x);

	/**
	* @brief Computes the nth term of the ln1minx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the ln1minx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln1_m_x2_series<T, K>::ln1_m_x2_series(T x) : series_base<T, K>(x, std::log(1 - x * x)) 
{
	if (std::abs(this->x) >= 1)
		throw std::domain_error("the ln(1 - x^2) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in (-1, 1)");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln1_m_x2_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return static_cast<T>(-std::pow(this->x, 2 * n) / a); // (85.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function artanh(x)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class artanh_x_series : public series_base<T, K>
{
public:
	artanh_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	artanh_x_series(T x);

	/**
	* @brief Computes the nth term of the artanhx series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the artanhx series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
artanh_x_series<T, K>::artanh_x_series(T x) : series_base<T, K>(x, std::atanh(x)) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the arth(x) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T artanh_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(std::pow(this->x, a) / a);  // (86.1) [Rows.pdf]
}


/**
* @brief Maclaurin series of function arsinh(x)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arcsinh_x_series : public series_base<T, K>
{
public:
	arcsinh_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	arcsinh_x_series(T x);

	/**
	* @brief Computes the nth term of the ln1minx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the ln1minx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arcsinh_x_series<T, K>::arcsinh_x_series(T x) : series_base<T, K>(x, std::asinh(x)) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the arsh(x) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsinh_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>((((1 - 2 * (n & 1))) * this->double_fact(a) * std::pow(this->x, a+2)) / (this->double_fact(a + 1) * (a + 2)));  // (87.2) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n} 
}


/**
* @brief Maclaurin series of function cos(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cos_x2_series : public series_base<T, K>
{
public:
	cos_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cos_x2_series(T x);

	/**
	* @brief Computes the nth term of the cos(x^2) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the cos(x^2) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
cos_x2_series<T, K>::cos_x2_series(T x) : series_base<T, K>(x, std::cos(x * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cos_x2_series<T, K>::operator()(K n) const
{
	const K a = 2 * n;
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, 2 * a) / this->fact(a)); // (1 - 2 * (n & 1)) = (-1)^{n} (88.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function sh(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sinh_x2_series : public series_base<T, K>
{
public:
	sinh_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sinh_x2_series(T x);

	/**
	* @brief Computes the nth term of the sinhx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the sinhx2 series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
sinh_x2_series<T, K>::sinh_x2_series(T x) : series_base<T, K>(x, std::sinh(x * x)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sinh_x2_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(std::pow(this->x, 2 * a) / this->fact(a)); // (89.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function arth(x^2) 
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arctanh_x2_series : public series_base<T, K>
{
public:
	arctanh_x2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	arctanh_x2_series(T x);

	/**
	* @brief Computes the nth term of the arctanhx2 series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arctanh_x2_series<T, K>::arctanh_x2_series(T x) : series_base<T, K>(x, std::atanh(x * x)) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the arth(x^2) series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arctanh_x2_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(std::pow(this->x, 2 * a) / a); // (90.2) [Rows.pdf]
}


/**
* @brief Taylor series of function cos(3x - 1) / x^2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cos3xmin1_div_xsqare_series : public series_base<T, K>
{
public:
	cos3xmin1_div_xsqare_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cos3xmin1_div_xsqare_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
cos3xmin1_div_xsqare_series<T, K>::cos3xmin1_div_xsqare_series(T x) : series_base<T, K>(x, (std::cos(3 * x) - 1) / (x * x)) {
	if (this->x == 0)
		throw std::domain_error("x cannot be zero");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cos3xmin1_div_xsqare_series<T, K>::operator()(K n) const
{
	const K a = 2 * n + 2;
	return static_cast<T>(((1 - 2 * (n & 1))) * std::pow(3, a) * std::pow(this->x, a - 2) / this->fact(a));
}


/**
* @brief Maclaurin series of function 2^x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class two_degree_x_series : public series_base<T, K>
{
public:
	two_degree_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	two_degree_x_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
two_degree_x_series<T, K>::two_degree_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(2, x))) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_degree_x_series<T, K>::operator()(K n) const
{
	return static_cast<T>(std::pow(std::log(2), n) * std::pow(this->x, n) / this->fact(n)); // (92.1) [Rows.pdf]
}


/**
* @brief Maclaurin series of function sqrt(1 + x) - 1 - x/2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sqrt_1plusx_min_1_min_x_div_2_series : public series_base<T, K>
{
public:
	sqrt_1plusx_min_1_min_x_div_2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sqrt_1plusx_min_1_min_x_div_2_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
sqrt_1plusx_min_1_min_x_div_2_series<T, K>::sqrt_1plusx_min_1_min_x_div_2_series(T x) : series_base<T, K>(x, std::sqrt(1 + x) - 1 - x / 2) 
{
	if (std::abs(this->x) > 1)
		throw std::domain_error("the sqrt(1 + x) - 1 - x/2 series diverge at x = " + std::to_string(this->x) + "; series converge if x only in [-1, 1]");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sqrt_1plusx_min_1_min_x_div_2_series<T, K>::operator()(K n) const
{
	return static_cast<T>(this->binomial_coefficient(static_cast<T>(0.5), n + 2) * std::pow(this->x, n)); // (93.2) [Rows.pdf]
}


/**
* @brief Numetric series of function (ln13 - ln7) * x / 7
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln13_min_ln7_div_7_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln13_min_ln7_div_7_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln13_min_ln7_div_7_series<T, K>::ln13_min_ln7_div_7_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(13 / 7) * x / 7)) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln13_min_ln7_div_7_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = n + 2;
	return static_cast<T>(this->minus_one_raised_to_power_n(b) * std::pow(6, a) * this->x / (a * std::pow(7, b))); // (94.2) [Rows.pdf]
}

/**
* @brief Taylor series of Bessel function J_b(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Ja_x_series : public series_base<T, K>
{
public:
	Ja_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series, b The integer constant
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Ja_x_series(T x, T a);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
private:
	/**
	* @brief The order of Bessel function
	* @authors Trudolyubov N.A.
	*/
	const T a;
};

template <std::floating_point T, std::unsigned_integral K>
Ja_x_series<T, K>::Ja_x_series(T x, T a) : series_base<T, K>(x), a(a) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Ja_x_series<T, K>::operator()(K n) const
{
	return this->minus_one_raised_to_power_n(n) * std::pow(this->x / 2, 2 * n + this->a) / (this->fact(n) * std::tgamma(n + this->a + 1)); // (95.1) [Rows.pdf]
}


/**
* @brief Taylor series of function 1/sqrt(2) * sin(x/sqrt(2))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_div_sqrt2_sin_xdivsqrt2_series : public series_base<T, K>
{
public:
	one_div_sqrt2_sin_xdivsqrt2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_div_sqrt2_sin_xdivsqrt2_series(T x);

	/**
	* @brief Computes the nth term of the Taylor series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Taylor series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_div_sqrt2_sin_xdivsqrt2_series<T, K>::one_div_sqrt2_sin_xdivsqrt2_series(T x) : series_base<T, K>(x, static_cast<T>((1 / std::sqrt(2)) * std::sin(x / std::sqrt(2)))) {
	if (this->x < -1)
		throw std::domain_error("x cannot be less than -1");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_div_sqrt2_sin_xdivsqrt2_series<T, K>::operator()(K n) const
{
#ifdef _WIN32
	return static_cast<T>(std::pow(-1, n / 2) * _jn(static_cast<int>(2 * n + 1), this->x)); // (96.1) [Rows.pdf]
#else
	return static_cast<T>(std::pow(-1, n / 2) * jn(static_cast<T>(2 * n + 1), this->x));
#endif
}


/**
* @brief Maclaurin series for function ln(1 + x)/(1 + x^2) 
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_1plusx_div_1plusx2 : public series_base<T, K>
{
public:
	ln_1plusx_div_1plusx2() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_1plusx_div_1plusx2(T x);

	/**
	* @brief Computes the nth term of the ln(1 + x)/(1 + x^2) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the ln(1 + x)/(1 + x^2) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_1plusx_div_1plusx2<T, K>::ln_1plusx_div_1plusx2(T x) : series_base<T, K>(std::log(1+x) / (1 + x * x)) 
{
	if (std::abs(this->x) >= 1)
	{
		throw std::domain_error("the ln(1 + x)/(1 + x^2) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1plusx_div_1plusx2<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return static_cast<T>(this->minus_one_raised_to_power_n(n) * std::pow(this->x, a) / (a * std::pow(1 + (this->x * this->x), a))); // (97.1) [Rows.pdf]
}


/**
* @brief Maclaurin series for function cos(sqrt(x))
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cos_sqrt_x : public series_base<T, K>
{
public:
	cos_sqrt_x() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cos_sqrt_x(T x);

	/**
	* @brief Computes the nth term of the cos(sqrt(x)) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the cos(sqrt(x)) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
cos_sqrt_x<T, K>::cos_sqrt_x(T x) : series_base<T, K>(x, static_cast<T>(std::cos(std::sqrt(x)))) 
{
	if (this->x < 0)
		throw std::domain_error("x cannot be negative");
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cos_sqrt_x<T, K>::operator()(K n) const
{
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, n) / this->fact(2 * n)); // (1 - 2 * (n & 1)) = (-1)^{n} (98.2) [Rows.pdf]
}


/**
* @brief Maclaurin series for function ln(1 + x^3)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_1_plus_x3 : public series_base<T, K>
{
public:
	ln_1_plus_x3() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_1_plus_x3(T x);

	/**
	* @brief Computes the nth term of the ln(1 + x^3) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the ln(1 + x^3) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_1_plus_x3<T, K>::ln_1_plus_x3(T x) : series_base<T, K>(x, static_cast<T>(std::log(1 + std::pow(x, 3)))) 
{
	if (std::abs(this->x) >= 1)
	{
		throw std::domain_error("the ln(1 + x^3) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1_plus_x3<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return static_cast<T>(this->minus_one_raised_to_power_n(a) * std::pow(this->x, 3 * a) / a); // (99.3) [Rows.pdf]
}


/**
* @brief Maclaurin series for function x / sqrt(1 - x)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_div_1minx : public series_base<T, K>
{
public:
	x_div_1minx() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_div_1minx(T x);

	/**
	* @brief Computes the nth term of the x / sqrt(1 - x) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the x / sqrt(1 - x) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_div_1minx<T, K>::x_div_1minx(T x) : series_base<T, K>(x, static_cast<T>(x / std::sqrt(1 - x))) 
{
	if (std::abs(this->x) >= 1)
	{
		throw std::domain_error("the x / sqrt(1 - x) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_div_1minx<T, K>::operator()(K n) const
{
	return static_cast<T>(this->fact(2 * n) * std::pow(this->x, n + 1) / (this->fact(n) * std::pow(4, n))); // (100.1) [Rows.pdf]
}


/**
* @brief Maclaurin series for function x / sqrt(1 - x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_div_1minx2 : public series_base<T, K>
{
public:
	x_div_1minx2() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_div_1minx2(T x);

	/**
	* @brief Computes the nth term of the x / sqrt(1 - x) series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the x / sqrt(1 - x) series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_div_1minx2<T, K>::x_div_1minx2(T x) : series_base<T, K>(x, static_cast<T>(x / std::sqrt(1 - x * x)))
{
	if (std::abs(this->x) >= 1)
	{
		throw std::domain_error("the x / sqrt(1 - x^2) series diverge at x = " + std::to_string(x) + "; series converge if x only in (-1, 1)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_div_1minx2<T, K>::operator()(K n) const
{
	return static_cast<T>(std::pow(this->x, std::fma(2, n, 1))); // (101.1) [Rows.pdf]
}


/**
* @brief Maclaurin series for gamma series
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class gamma_series : public series_base<T, K>
{
public:
	gamma_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	gamma_series(T t, T x);

	/**
	* @brief Computes the nth term of the using gamma series
	* @authors Pavlova A.R.
	* @param n The number of the term
	* @param t The parametr for series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the using gamma series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	T t;
	T x;
	T a_k(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
gamma_series<T, K>::gamma_series(T t, T x) : series_base<T, K>(x), t(t), x(x) {}

template <std::floating_point T, std::unsigned_integral K>
T gamma_series<T, K>::a_k(K n) const
{
	return static_cast<T>(n + 1);
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T gamma_series<T, K>::operator()(K n) const
{
	T a_k_n = a_k(n);
	return static_cast<T>(a_k_n * std::pow(this->t, n) * std::tgamma(a_k_n)); // (102.1) [Rows.pdf]
}

/**
* @brief function for testing new serieses or converting basic to
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
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

template <std::floating_point T, std::unsigned_integral K>
testing_series<T, K>::testing_series(T x) : series_base<T, K>(x, 0) {}

template <std::floating_point T, std::unsigned_integral K>
constexpr T testing_series<T, K>::operator()(K n) const
{
	return 0;
}


/**
* @brief function for testing new requrrive serieses or converting basic to
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class reccurent_testing_series : public series_base<T, K>, public reccurent_series_base<T, K>
{
public:
	reccurent_testing_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	reccurent_testing_series(T x);

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
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
reccurent_testing_series<T, K>::reccurent_testing_series(T x) : series_base<T, K>(x, 0), reccurent_series_base<T, K>(x) {}

template <std::floating_point T, std::unsigned_integral K>
T reccurent_testing_series<T, K>::access_row(K n)
{
	K old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (K i = old_size; i <= n; ++i) 
		this->series_vector.push_back(this->series_vector[i - 1] * (this->x * this->x) / (i * std::fma(4, i, 2)));

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T reccurent_testing_series<T, K>::operator()(K n) const
{
	return const_cast<reccurent_testing_series<T, K>*>(this)->access_row(n);
}