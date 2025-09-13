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
 * 26 - three_minus_pi_series
 * 27 - one_twelfth_series
 * 28 - eighth_pi_m_one_third_series
 * 29 - one_third_pi_squared_m_nine_series
 * 30 - four_ln2_m_3_series
 * 31 - exp_m_cos_x_sinsin_x_series
 * 32 - pi_four_minus_ln2_halfed_series
 * 33 - five_pi_twelve_series
 * 34 - x_two_series
 * 35 - pi_six_min_half_series
 * 36 - x_two_throught_squares_series
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
 *  76 - Series_with_ln_number1_series
 *  77 - Series_with_ln_number2_series
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
 *  97 - ln_1plusx_div_1plusx2_series
 *  98 - cos_sqrt_x_series
 *  99 - ln_1_plus_x3_series
 *  100 - x_div_1minx_series
 *  101 - x_div_1minx2_series
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
    * @brief Throws domain error with unified message format
    * @authors Maximov A.K.
    * @param condition Description of the divergence condition
    * @throws std::domain_error with formatted message containing series name, x value and condition
    */
	void throw_domain_error(const std::string& condition) const {
		throw std::domain_error(series_name + " series diverges at x = " + std::to_string(x) + " (" + condition + ")");
	}

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

	/**
    * @brief Name of the series for unified error messages
    * Used to generate consistent error output format
    * @authors Maximov A.K.
    */
	std::string series_name;
};

template <std::floating_point T, std::unsigned_integral K>
series_base<T, K>::series_base(T x) : x(x), sum(0), series_name("unknown") {}

template <std::floating_point T, std::unsigned_integral K>
series_base<T, K>::series_base(T x, T sum) : x(x), sum(sum), series_name("unknown") {}

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

protected:
	/**
	* @brief Throws domain error with unified message format
	* @param condition Description of the divergence condition
	* @throws std::domain_error with formatted message
	* @authors Maximov A.K.
	*/
	void throw_domain_error(const std::string& condition) const {
		throw std::domain_error("recurrent series diverges (" + condition + ")");
	}
};

template <std::floating_point T, std::unsigned_integral K>
reccurent_series_base<T, K>::reccurent_series_base(T x)
{
	this->series_vector.push_back(x);
};


template <std::floating_point T, std::unsigned_integral K>
reccurent_series_base<T, K>::reccurent_series_base(std::vector<T> row) : series_vector(std::move(row))
{
	if (this->series_vector.empty()) {
		this->throw_domain_error("empty initial row");
	}
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
exp_series<T, K>::exp_series(T x) : series_base<T, K>(x, std::exp(x)), reccurent_series_base<T, K>(static_cast<T>(1))
{
	this->series_name = "exp(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T exp_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * this->x / i);

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
cos_series<T, K>::cos_series(T x) : series_base<T, K>(x, std::cos(x)), reccurent_series_base<T, K>(std::vector<T>{static_cast<T>(1), (static_cast<T>(-1) * x * x) / static_cast<T>(2)})
{
	this->series_name = "cos(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T cos_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector; // ссылка на член базового класса
	auto old_size = series_vec.size();
	series_vec.reserve(n);

	T a = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		series_vec.push_back(series_vec[i - 1] * (a / static_cast<T>(i * std::fma(4, i, -2))));

	return series_vec[n];
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
sin_series<T, K>::sin_series(T x) : series_base<T, K>(x, std::sin(x)), reccurent_series_base<T, K>(x) {
	this->series_name = "sin(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T sin_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector; // ссылка на член базового класса
	auto old_size = series_vec.size();
	series_vec.reserve(n);

	T a = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		series_vec.push_back(series_vec[i - 1] * (a / static_cast<T>(i * std::fma(4, i, 2))));

	return series_vec[n];
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
cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x)), reccurent_series_base<T, K>(std::vector<T>{static_cast<T>(1), (x * x) / static_cast<T>(2)})
{
	this->series_name = "cosh(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T cosh_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = static_cast<T>(this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * (a / static_cast<T>(i * std::fma(4, i, -2)))); // (4.2) [Rows.pdf]

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
sinh_series<T, K>::sinh_series(T x) : series_base<T, K>(x, std::sinh(x)), reccurent_series_base<T, K>(x)
{
	this->series_name = "sinh(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T sinh_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = static_cast<T>(this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * (a / static_cast<T>(i * std::fma(4, i, 2))));

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
bin_series<T, K>::bin_series(T x, T alpha) : series_base<T, K>(x, static_cast<T>(std::pow(static_cast<T>(1) + x, alpha))), alpha(alpha)
{
	this->series_name = "(1+x)^α";
	// Сходится при |x| < 1 (абсолютно), при x = -1 зависит от α
	// При α > 0 сходится при x = -1, при α ≤ -1 расходится

	if (std::abs(x) >= 1 || !std::isfinite(x) || !std::isfinite(alpha)) {
		this->throw_domain_error("|x| must be < 1 and parameters must be finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T bin_series<T, K>::operator()(K n) const
{
	return binomial_coefficient(alpha, n) * static_cast<T>(std::pow(this->x, n)); // (6.1) [Rows.pdf]
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
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, static_cast<T>(4) * static_cast<T>(std::atan(x)))
{
	this->series_name = "4*arctan(x)";
	// Сходится при |x| ≤ 1 (условно при |x| = 1)

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T four_arctan_series<T, K>::operator()(K n) const
{
	const K temp = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(4) * series_base<T, K>::minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(this->x, temp)) / static_cast<T>(temp); // (7.1) Rows.pdf
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
ln1mx_series<T, K>::ln1mx_series(T x) : series_base<T, K>(x, - static_cast<T>(std::log(static_cast<T>(1) - x)))
{
	this->series_name = "-ln(1-x)";
	// Сходится при |x| < 1 (абсолютно), условно сходится при x = -1
	// Расходится при x = 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln1mx_series<T, K>::operator()(K n) const
{
	const K temp = n + 1;
	return static_cast<T>(std::pow(this->x, temp)) / static_cast<T>(temp); // (8.2) [Rows.pdf]
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
mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * (static_cast<T>(std::sinh(x)) + static_cast<T>(std::sin(x)))), reccurent_series_base<T, K>(x)
{
	this->series_name = "0.5*(sinh(x) + sin(x))";
	// Сходится при ∀x ∈ ℝ (как сумма двух всюду сходящихся рядов)

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T mean_sinh_sin_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector; // ссылка на член базового класса
	auto old_size = series_vec.size();
	series_vec.reserve(n);
	T a;
	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		a = static_cast<T>(std::fma(4, i, 1));
		series_vec.push_back(series_vec[i - 1] * static_cast<T>(std::pow(this->x, 4)) / static_cast<T>((4 * i * a * (a - 2) * (a - 3))));
	}
	return series_vec[n];
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
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, static_cast<T>(std::exp(x * x)) * static_cast<T>(std::erf(x)))
{
	this->series_name = "exp(x²)*erf(x)";
	// Сходится при ∀x ∈ ℝ (ряд для erf(x) сходится на всей числовой прямой)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_squared_erf_series<T, K>::operator()(K n) const
{
	const T result = static_cast<T>(std::pow(this->x, std::fma(2, n, 1))) / static_cast<T>(std::tgamma(n + 1.5)); // (10.3) [Rows.pdf]
	if (!isfinite(result))
		throw std::overflow_error("operator() is too big");
	return result;
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
xmb_Jb_two_series<T, K>::xmb_Jb_two_series(T x, K b) : series_base<T, K>(x, static_cast<T>(static_cast<T>(1) / static_cast<T>(std::pow(x, b)) * static_cast<T>(std::cyl_bessel_j(b, 2 * x)))), mu(b) 
{
	this->series_name = "x⁻ᵇJᵦ(2x)";
	// Сходится при ∀x ∈ ℝ\{0} (ряд Бесселя сходится на всей числовой прямой)
	// При x = 0 требуется специальное рассмотрение (предел)

	if (!std::isfinite(x) || x == 0) {
		this->throw_domain_error("x must be finite and non-zero");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T xmb_Jb_two_series<T, K>::operator()(K n) const
{
	return series_base<T, K>::minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(this->x, 2 * n))
		/ (static_cast<T>(this->fact(n)) * static_cast<T>(std::tgamma(n + this->mu + 1))); // (11.3) [Rows.pdf]
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
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * static_cast<T>(std::asin(static_cast<T>(2) * x)))
{
	this->series_name = "0.5*asin(2x)";
	// Сходится при |x| ≤ 0.5 (абсолютно при |x| < 0.5, условно при |x| = 0.5)

	if (std::abs(x) > static_cast<T>(0.5) || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 0.5");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_asin_two_x_series<T, K>::operator()(K n) const
{
	const T _fact_n = static_cast<T>(this->fact(n));
	const K a = 2 * n;
	return static_cast<T>(this->fact(a)) * static_cast<T>(std::pow(this->x, a + 1)) / (_fact_n * _fact_n * static_cast<T>((a + 1))); // (12.1) [Rows.pdf]
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
	this->series_name = "1/(1-x)";
	// Сходится при |x| < 1 (геометрический ряд)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
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
x_1mx_squared_series<T, K>::x_1mx_squared_series(T x) : series_base<T, K>(x, x / static_cast<T>(std::fma(x, x - 1, 1 - x)))
{
	this->series_name = "x/(1-x)^2";
	// Сходится при |x| < 1 (производная геометрического ряда)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_1mx_squared_series<T, K>::operator()(K n) const
{
	return static_cast<T>(std::pow(this->x, n)) * static_cast<T>(n); // (14.4) [Rows.pdf]
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
erf_series<T, K>::erf_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * static_cast<T>(std::sqrt(std::numbers::pi)) * static_cast<T>(std::erf(x))), reccurent_series_base<T, K>(x)
{
	this->series_name = "√π/2 * erf(x)";
	// Сходится при ∀x ∈ ℝ (ряд для erf(x) сходится на всей числовой прямой)

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T erf_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector; // ссылка на член базового класса
	auto old_size = series_vec.size();
	series_vec.reserve(n);
	T a;
	T b = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
	{
		a = static_cast<T>(std::fma(2, i, 1));
		series_vec.push_back(series_vec[i - 1] * (b * (a - static_cast<T>(2)) / (static_cast<T>(i) * a)));
	}
	return series_vec[n];
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
m_fact_1mx_mp1_inverse_series<T, K>::m_fact_1mx_mp1_inverse_series(T x, K m) : series_base<T, K>(x, static_cast<T>(this->fact(m)) / static_cast<T>(pow(static_cast<T>(1) - x, m + 1))), m(m), reccurent_series_base<T, K>(static_cast<T>(this->fact(m)))
{
	this->series_name = "m!/(1-x)^(m+1)";
	// Сходится при |x| < 1 (m-ая производная геометрического ряда)
	// Расходится при |x| ≥ 1

	if (!std::isfinite(series_base<T, K>::sum)) {
		throw std::overflow_error("sum is too big");
	}

	if (std::abs(this->x) >= 1 || !std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T m_fact_1mx_mp1_inverse_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector;
	auto old_size = series_vec.size();
	series_vec.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		series_vec.push_back(series_vec[i - 1] * this->x * static_cast<T>((this->m + static_cast<K>(i)) / static_cast<K>(i)));

	return series_vec[n];
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
inverse_sqrt_1m4x_series<T, K>::inverse_sqrt_1m4x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(std::fma(static_cast<T>(-4), x, static_cast<T>(1)), -0.5)))
{
	this->series_name = "1/sqrt(1-4x)";
	// Сходится при |x| < 0.25 (биномиальный ряд)
	// Расходится при |x| ≥ 0.25

	if (std::abs(x) >= static_cast<T>(0.25) || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 0.25");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T inverse_sqrt_1m4x_series<T, K>::operator()(K n) const
{
	const T _fact_n = static_cast<T>(this->fact(n));
	return static_cast<T>(this->fact(2 * n)) * static_cast<T>(pow(this->x, n) / (_fact_n * _fact_n)); // (17.2) [Rows.pdf]
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
one_twelfth_3x2_pi2_series<T, K>::one_twelfth_3x2_pi2_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(static_cast<T>(3) * x, x, -static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi)) / static_cast<T>(12)))
{
	this->series_name = "1/12*(3x^2-pi^2)";
	// Сходится при |x| ≤ π (ряд Фурье для функции f(x) = x²)
	// Расходится при |x| > π

	if (std::abs(x) > std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= π");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_twelfth_3x2_pi2_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return series_base<T, K>::minus_one_raised_to_power_n(a) * static_cast<T>(std::cos(static_cast<T>(a) * this->x)) / static_cast<T>(a * a); // (18.2) [Rows.pdf]
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
x_twelfth_x2_pi2_series<T, K>::x_twelfth_x2_pi2_series(T x) : series_base<T, K>(x, x * static_cast<T>(std::fma(x, x, - static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi))) / static_cast<T>(12))
{
	this->series_name = "x/12*(x^2-pi^2)";
	// Сходится при |x| ≤ π (ряд Фурье для функции f(x) = x³)
	// Расходится при |x| > π

	if (std::abs(x) > std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= π");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_twelfth_x2_pi2_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return series_base<T, K>::minus_one_raised_to_power_n(a) * static_cast<T>(std::sin(static_cast<T>(a) * this->x)) / static_cast<T>(std::pow(a, 3)); // (19.2) [Rows.pdf]
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
ln2_series<T, K>::ln2_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(2)) * x)
{
	this->series_name = "ln(2)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln2_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	return - series_base<T, K>::minus_one_raised_to_power_n(a) * this->x / static_cast<T>(a); // (20.2) [Rows.pdf]
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
one_series<T, K>::one_series(T x) : series_base<T, K>(x, static_cast<T>(1) * x)
{
	this->series_name = "1*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? this->x / static_cast<T>(fma(n, n, n)) : 0); // (21.5) [Rows.pdf]
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
minus_one_quarter_series<T, K>::minus_one_quarter_series(T x) : series_base<T, K>(x, static_cast<T>(-0.25) * x)
{
	this->series_name = "-0.25*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_quarter_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / static_cast<T>(n * (n + static_cast<K>(2))) : 0); // (22.2) [Rows.pdf]
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
pi_3_series<T, K>::pi_3_series(T x) : series_base<T, K>(x, static_cast<T>(std::numbers::pi) * x / static_cast<T>(3))
{
	this->series_name = "π*x/3";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_3_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return this->x / (static_cast<T>(n + 1) * static_cast<T>(a) * static_cast<T>(2 * a - 1)); // (23.2) [Rows.pdf]
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
pi_4_series<T, K>::pi_4_series(T x) : series_base<T, K>(x, static_cast<T>(std::numbers::pi) * x / static_cast<T>(4))
{
	this->series_name = "π*x/4";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_4_series<T, K>::operator()(K n) const
{
	return series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / static_cast<T>(std::fma(2, n, 1)); // (24.3) [Rows.pdf]
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
pi_squared_6_minus_one_series<T, K>::pi_squared_6_minus_one_series(T x) : series_base<T, K>(x, x * static_cast<T>(std::fma(static_cast<T>(std::numbers::pi) / static_cast<T>(6), static_cast<T>(std::numbers::pi), static_cast<T>(-1))))
{
	this->series_name = "(π²/6 - 1)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_squared_6_minus_one_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? this->x / static_cast<T>(n * n * (n + static_cast<K>(1))) : 0); // (25.3) [Rows.pdf]
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
three_minus_pi_series<T, K>::three_minus_pi_series(T x) : series_base<T, K>(x, ((static_cast<T>(3) - static_cast<T>(std::numbers::pi)) * x))
{
	this->series_name = "(3-π)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T three_minus_pi_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? series_base<T, K>::minus_one_raised_to_power_n(n) * this-> x / static_cast<T>(n * (n + static_cast<K>(1)) * std::fma(2, n, 1)) : 0); // (26.2) [Rows.pdf]
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
one_twelfth_series<T, K>::one_twelfth_series(T x) : series_base<T, K>(x, x / static_cast<T>(12))
{
	this->series_name = "x/12";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_twelfth_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return this->x / static_cast<T>((a * (a + static_cast<K>(2)) * (a + static_cast<K>(4)))); // (27.2) [Rows.pdf]
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
eighth_pi_m_one_third_series<T, K>::eighth_pi_m_one_third_series(T x) : series_base<T, K>(x, x * static_cast<T>(std::numbers::pi) / static_cast<T>(8) - static_cast<T>(1) / static_cast<T>(3))
{
	this->series_name = "(π/8 - 1/3)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T eighth_pi_m_one_third_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / static_cast<T>(a * (a + static_cast<K>(2)) * (a + static_cast<K>(4))); // (28.2) [Rows.pdf]
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
one_third_pi_squared_m_nine_series<T, K>::one_third_pi_squared_m_nine_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(static_cast<T>(std::numbers::pi), static_cast<T>(std::numbers::pi), static_cast<T>(-9))) * x / static_cast<T>(3))
{
	this->series_name = "(π² - 9)*x/3";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_third_pi_squared_m_nine_series<T, K>::operator()(K n) const
{
	const K a = n * (n + static_cast<K>(1));
	return static_cast<T>(n ? this->x / static_cast<T>(a * a) : 0); // (29.2) [Rows.pdf]
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
four_ln2_m_3_series<T, K>::four_ln2_m_3_series(T x) : series_base<T, K>(x, x * static_cast<T>(std::fma(static_cast<T>(4), static_cast<T>(std::log(2)), static_cast<T>(-3))))
{
	this->series_name = "(4*ln(2) - 3)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T four_ln2_m_3_series<T, K>::operator()(K n) const
{
	const K a = n * (n + static_cast<K>(1));
	return static_cast<T>(n ? series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / static_cast<T>(a * a) : 0); // (30.2) [Rows.pdf]
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
exp_m_cos_x_sinsin_x_series<T, K>::exp_m_cos_x_sinsin_x_series(T x) : series_base<T, K>(x, std::exp(-std::cos(x))* std::sin(std::sin(x)))
{
	this->series_name = "exp(-cos(x))*sin(sin(x))";
	// Сходится при ∀x ∈ ℝ (композиция всюду сходящихся функций)
	// Однако для численной стабильности ограничиваем |x| ≤ π

	if (std::abs(x) >= std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= π for numerical stability");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_m_cos_x_sinsin_x_series<T, K>::operator()(K n) const
{
	return this->minus_one_raised_to_power_n(n + static_cast<K>(1)) * static_cast<T>(std::sin(static_cast<T>(n) * this->x)) / static_cast<T>(this->fact(n)); // (31.1) [Rows.pdf]
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
pi_four_minus_ln2_halfed_series<T, K>::pi_four_minus_ln2_halfed_series(T x) : series_base<T, K>(x, x * (static_cast<T>(std::numbers::pi) / static_cast<T>(4) - static_cast<T>(std::log(2)) / static_cast<T>(2)))
{
	this->series_name = "(π/4 - ln(2)/2)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_four_minus_ln2_halfed_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 2))) / static_cast<T>(n) : 0); // (32.2) [Rows.pdf]
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
five_pi_twelve_series<T, K>::five_pi_twelve_series(T x) : series_base<T, K>(x, x * static_cast<T>(5) * static_cast<T>(std::numbers::pi) / static_cast<T>(12))
{
	this->series_name = "5πx/12";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T five_pi_twelve_series<T, K>::operator()(K n) const
{
	return this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 3))) / static_cast<T>(std::fma(2, n, 1)); // (33.2) [Rows.pdf]
}


/**
* @brief Numerical series representation of x / 2 (version 1)
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
x_two_series<T, K>::x_two_series(T x) : series_base<T, K>(x, x / static_cast<T>(2))
{
	this->series_name = "x/2";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_two_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return this->x / static_cast<T>(a * (a + static_cast<K>(2))); // (34.2) [Rows.pdf]
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
pi_six_min_half_series<T, K>::pi_six_min_half_series(T x) : series_base<T, K>(x, x * (static_cast<T>(std::numbers::pi) / static_cast<T>(6) - static_cast<T>(0.5)))
{
	this->series_name = "(π/6 - 0.5)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_six_min_half_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(6, n, 5));
	return this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / static_cast<T>(a * (a + static_cast<K>(2))); // (35.2) [Rows.pdf]
}


/**
* @brief Numerical series representation of x/2 (version 2)
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
x_two_throught_squares_series<T, K>::x_two_throught_squares_series(T x) : series_base<T, K>(x, x / static_cast<T>(2))
{
	this->series_name = "x/2";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_two_throught_squares_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? this->x * static_cast<T>(std::fma(2, n * n, -1)) / static_cast<T>(std::fma(4, std::pow(n, 4), 1)) : 0); // (36.2) [Rows.pdf]
}


/**
* @brief Numerical series (-1)^n * x /n^n = -0,78343051 * x
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
minus_one_ned_in_n_series<T, K>::minus_one_ned_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(-0.78343051)* x) // -0.78343051 value only for initialization, we can calculate with more accuracy with operator()(K n)
{
	this->series_name = "∑(-1)ⁿ/n * x";
	// Сходится при |x| ≤ 1, x ≠ -1 (условно при x = 1, расходится при x = -1)
	// При |x| < 1 сходится абсолютно

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_ned_in_n_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / static_cast<T>(std::pow(n, n)) : 0); // (37.2) [Rows.pdf]
}


/**
* @brief  Numerical series (-1)^n n! * x / n^n = -0.65583160
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
minus_one_n_fact_n_in_n_series<T, K>::minus_one_n_fact_n_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(-0.65583160) * x) // -0.65583160 value only for initialization, we can calculate with more accuracy with operator()(K n)
{
	this->series_name = "∑(-1)ⁿ·n!/n * x";
	// Сходится только при x = 0 (факториал растёт быстрее любой геометрической прогрессии)
	// Расходится при ∀x ≠ 0

	if (x != 0 || !std::isfinite(x)) {
		this->throw_domain_error("series converges only at x = 0");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_n_fact_n_in_n_series<T, K>::operator()(K n) const
{
	return static_cast<T>(n ? this->x * static_cast<T>(this->fact(n)) * series_base<T, K>::minus_one_raised_to_power_n(n) / static_cast<T>(std::pow(n, n)) : 0); // (38.2) [Rows.pdf]
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
ln_x_plus_one_x_minus_one_halfed_series<T, K>::ln_x_plus_one_x_minus_one_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(std::log((static_cast<T>(1) + x) / (static_cast<T>(1) - x))) / static_cast<T>(2))
{
	this->series_name = "0.5*ln((1+x)/(1-x))";
	// Сходится при |x| < 1 (абсолютно)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_x_plus_one_x_minus_one_halfed_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return static_cast<T>(std::pow(this->x, a)) / static_cast<T>(a); // (39.3) [Rows.pdf]
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
two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(2) * static_cast<T>(std::asin(x / static_cast<T>(2))) * static_cast<T>(std::asin(x / static_cast<T>(2))))
{
	this->series_name = "2*arcsin(x/2)^2";
	// Сходится при |x| ≤ 2 (ряд для arcsin(z) сходится при |z| ≤ 1, где z = x/2)
	// Расходится при |x| > 2

	if (std::abs(x) > 2 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 2");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_arcsin_square_x_halfed_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	// return (this->binomial_coefficient(2 * n, n) * this->binomial_coefficient(2 * n, n) * std::pow(this->x, 2 * a)) / (a * a * std::pow(2, std::fma(3, a, -2)));
	return static_cast<T>((std::pow(this->x, a - 1)) *  static_cast<T>(this->fact(n)) * static_cast<T>(this->fact(n))) / (static_cast<T>(2 * (n + 1)) * static_cast<T>(this->fact(a)));
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
pi_squared_twelve_series<T, K>::pi_squared_twelve_series(T x) : series_base<T, K>(x, x * static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi) / static_cast<T>(12))
{
	this->series_name = "π²x/12";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_squared_twelve_series<T, K>::operator()(K n) const
{
	const T n_temp = static_cast<T>(n + 1);
	return this->x * this->minus_one_raised_to_power_n(n) / (n_temp * n_temp); // (41.1) [Rows.pdf]
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
pi_cubed_32_series<T, K>::pi_cubed_32_series(T x) : series_base<T, K>(x, x * static_cast<T>(std::pow(std::numbers::pi, 3)) / static_cast<T>(32))
{
	this->series_name = "π³x/32";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_cubed_32_series<T, K>::operator()(K n) const
{
	const T n_temp = static_cast<T>(n + 1);
	return this->x * this->minus_one_raised_to_power_n(n) / static_cast<T>(std::pow(std::fma(2, n_temp, -1), 3)); // (42.2) [Rows.pdf]
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
minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series(T x) : series_base<T, K>(x, x * (static_cast<T>(-3)  + (static_cast<T>(3) / static_cast<T>(2)) * static_cast<T>(std::log(3)) +  static_cast<T>(std::log(4))))
{
	this->series_name = "(-3 + 3/2*ln(3) + 2*ln(2))*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::operator()(K n) const
{
	T n_temp = static_cast<T>(n + 1);
	return this->x / (n_temp * static_cast<T>(std::fma(36, n_temp * n_temp, -1)));  // (43.2) [Rows.pdf]
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
two_ln2_series<T, K>::two_ln2_series(T x) : series_base<T, K>(x, x * static_cast<T>(static_cast<T>(2) * static_cast<T>(std::log(2))))
{
	this->series_name = "2*ln(2)*x";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_ln2_series<T, K>::operator()(K n) const
{
	const T n_temp = static_cast<T>(n + 1);
	const T a = n_temp * n_temp * static_cast<T>(4) - static_cast<T>(1);
	return (static_cast<T>(3) * a + static_cast<T>(2)) / (n_temp * a * a); // (44.2) [Rows.pdf]
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
pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series(T x) : series_base<T, K>(x, ((static_cast<T>(std::numbers::pi) * x) / (static_cast<T>(std::tanh(static_cast<T>(std::numbers::pi) * x)))) - 1)
{
	this->series_name = "π*x*coth(π*x)-1";
	// Сходится при ∀x ∈ ℝ\{0} (ряд для гиперболического котангенса)
	// При x = 0 требуется специальное рассмотрение (предел)

	if (x == 0 || !std::isfinite(x)) {
		this->throw_domain_error("x must be finite and non-zero");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::operator()(K n) const
{
	const K a = n + static_cast<K>(1);
	const T b = this->x * this->x;
	return (static_cast<T>(2) * b) / (b + static_cast<T>(a * a)); // (45.4) [Rows.pdf]
}


/**
* @brief Trigonometric series of function (pi - x) / 2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_minus_x_2_series : public series_base<T, K>
{
public:
	pi_minus_x_2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_minus_x_2_series(T x);

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
pi_minus_x_2_series<T, K>::pi_minus_x_2_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * (static_cast<T>(std::numbers::pi) - x))
{
	this->series_name = "(π-x)/2";
	// Сходится при 0 < x < 2π (ряд Фурье для функции f(x) = x)
	// Расходится при x ≤ 0 или x ≥ 2π

	if (x <= 0 || x >= 2 * std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (0, 2π)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_minus_x_2_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(n + 1);
	return static_cast<T>(std::sin(a * (this->x))) / a;  // (46.5) [Rows.pdf]
}


/**
* @brief Trigonometric series of function 0.5 * ln(1/(2*(1 - cos(x))))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class half_multi_ln_1div2multi1minuscosx_series : public series_base<T, K>
{
public:
	half_multi_ln_1div2multi1minuscosx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	half_multi_ln_1div2multi1minuscosx_series(T x);

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
half_multi_ln_1div2multi1minuscosx_series<T, K>::half_multi_ln_1div2multi1minuscosx_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * static_cast<T>(std::log(static_cast<T>(1) / (static_cast<T>(2) - static_cast<T>(2) * static_cast<T>(std::cos(x))))))
{
	this->series_name = "0.5*ln(1/(2*(1-cos(x))))";
	// Сходится при 0 < x < 2π (ряд для логарифма тригонометрической функции)
	// Расходится при x = 0, x = 2π и других точках, где cos(x) = 1

	if (x <= 0 || x >= 2 * std::numbers::pi || !std::isfinite(x) || std::cos(x) == 1) {
		this->throw_domain_error("x must be in (0, 2π) and cos(x) != 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_multi_ln_1div2multi1minuscosx_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(n + 1);
	return static_cast<T>(std::cos(a * (this->x))) / a; // (47.5) [Rows.pdf]
}


/**
* @brief Trigonometric series of function 0.5 - (pi/4)*sin(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class half_minus_sinx_multi_pi_4_series : public series_base<T, K>
{
public:
	half_minus_sinx_multi_pi_4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	half_minus_sinx_multi_pi_4_series(T x);

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
half_minus_sinx_multi_pi_4_series<T, K>::half_minus_sinx_multi_pi_4_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) - static_cast<T>(std::numbers::pi) * static_cast<T>(std::sin(x)) * static_cast<T>(0.25))
{
	this->series_name = "0.5-(π/4)*sin(x)";
	// Сходится при 0 ≤ x ≤ π/2 (ряд Фурье или тригонометрический ряд)
	// Расходится при x < 0 или x > π/2

	if (x < 0 || x > std::numbers::pi / 2 || !std::isfinite(x)) {
		this->throw_domain_error("x must be in [0, π/2]");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_minus_sinx_multi_pi_4_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return std::cos(static_cast<T>(2) * (this->x) * static_cast<T>(n + 1)) / static_cast<T>(a * (a + 2)); // (48.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function ln(1 + sqrt(1 + x^2)) - ln(2)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_1plussqrt1plusxsquare_minus_ln_2_series : public series_base<T, K>
{
public:
	ln_1plussqrt1plusxsquare_minus_ln_2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_1plussqrt1plusxsquare_minus_ln_2_series(T x);

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
ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>::ln_1plussqrt1plusxsquare_minus_ln_2_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(static_cast<T>(0.5) * (static_cast<T>(1) + static_cast<T>(std::hypot(1, x))))))
{
	this->series_name = "ln(1+sqrt(1+x^2))-ln(2)";
	// Сходится при |x| ≤ 1 (биномиальный ряд для sqrt(1+x²))
	// Расходится при |x| > 1

	if (x * x > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = static_cast<K>(std::fma(2, n, 2));
	return (this->minus_one_raised_to_power_n(n) * static_cast<T>(this->fact(b - 1)) * static_cast<T>(std::pow(this->x, b))) /
		static_cast<T>(static_cast<K>(pow(static_cast<K>(2), b)) * this->fact(a) * this->fact(a)); // (49.3) [Rows.pdf]
}


/**
* @brief Maclaurin series of function ln(cos(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_cosx_series : public series_base<T, K>
{
public:
	ln_cosx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_cosx_series(T x);

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
ln_cosx_series<T, K>::ln_cosx_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(std::cos(x))))
{
	this->series_name = "ln(cos(x))";
	// Сходится при |x| < π/2 (ряд для логарифма косинуса)
	// Расходится при |x| ≥ π/2 (cos(x) ≤ 0, логарифм не определён)

	if (std::abs(x) * 2 >= std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < π/2");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_cosx_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 2));
	return static_cast<T>( - std::pow(std::sin(this->x), a)) / static_cast<T>(a); // (50.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function ln(sin(x)) - ln(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_sinx_minus_ln_x_series : public series_base<T, K>
{
public:
	ln_sinx_minus_ln_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_sinx_minus_ln_x_series(T x);

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
ln_sinx_minus_ln_x_series<T, K>::ln_sinx_minus_ln_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(static_cast<T>(std::sin(x)) / x)))
{
	this->series_name = "ln(sin(x)/x)";
	// Сходится при 0 < x ≤ π (ряд для логарифма синуса)
	// Расходится при x ≤ 0 или x > π (sin(x)/x ≤ 0, логарифм не определён)

	if (x <= 0 || x > std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (0, π]");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_sinx_minus_ln_x_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(this->x / (static_cast<T>(n + 1) * static_cast<T>(std::numbers::pi)));
	return static_cast<T>(std::log(static_cast<T>(1) - a * a)); // (51.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function (pi/8)*cos^2(x) - (1/3)*cos(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_8_cosx_square_minus_1_div_3_cosx_series : public series_base<T, K>
{
public:
	pi_8_cosx_square_minus_1_div_3_cosx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_8_cosx_square_minus_1_div_3_cosx_series(T x);

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
pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>::pi_8_cosx_square_minus_1_div_3_cosx_series(T x) : series_base<T, K>(x, (static_cast<T>(std::numbers::pi) / static_cast<T>(8)) * static_cast<T>(std::cos(x)) * static_cast<T>(std::cos(x)) - ((static_cast<T>(1) / static_cast<T>(3)) * static_cast<T>(std::cos(x))))
{
	this->series_name = "(π/8)*cos²(x)-(1/3)*cos(x)";
	// Сходится при |x| ≤ π/2 (тригонометрический ряд)
	// Расходится при |x| > π/2

	if (std::abs(x) > std::numbers::pi / 2 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= π/2");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>::operator()(K n) const
{
	const K temp = static_cast<K>(std::fma(2, n, 1));
	return this->minus_one_raised_to_power_n(n) * static_cast<T>(std::cos(temp + static_cast<T>(2))) * static_cast<T>(this->x) / 
		static_cast<T>((temp * (temp + 2) * (temp + 4))); // (52.2) [Rows.pdf]
}


/**
* @brief Maclaurin series of function sqrt((1 - sqrt(1 - x)) / x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sqrt_oneminussqrtoneminusx_div_x_series : public series_base<T, K>
{
public:
	sqrt_oneminussqrtoneminusx_div_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sqrt_oneminussqrtoneminusx_div_x_series(T x);

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
sqrt_oneminussqrtoneminusx_div_x_series<T, K>::sqrt_oneminussqrtoneminusx_div_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::sqrt((static_cast<T>(1) - static_cast<T>(std::sqrt(static_cast<T>(1) - x))) / x)))
{
	this->series_name = "sqrt((1-sqrt(1-x))/x)";
	// Сходится при 0 < |x| < 1 (биномиальный ряд для sqrt(1-x))
	// Расходится при |x| ≥ 1 или x = 0

	if (std::abs(x) >= 1 || x == 0 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be in (0, 1)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sqrt_oneminussqrtoneminusx_div_x_series<T, K>::operator()(K n) const
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
class one_minus_sqrt_1minus4x_div_2x_series : public series_base<T, K>
{
public:
	one_minus_sqrt_1minus4x_div_2x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_minus_sqrt_1minus4x_div_2x_series(T x);

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
one_minus_sqrt_1minus4x_div_2x_series<T, K>::one_minus_sqrt_1minus4x_div_2x_series(T x) : series_base<T, K>(x, (static_cast<T>(1) - static_cast<T>(sqrt(std::fma(-4, x, 1)))) / (static_cast<T>(2) * x))
{
	this->series_name = "(1-sqrt(1-4x))/(2x)";
	// Сходится при 0 < |x| ≤ 0.25 (производящая функция для чисел Каталана)
	// Расходится при |x| > 0.25 или x = 0

	if (std::abs(x) > static_cast<T>(0.25) || x == 0 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be in (0, 0.25]");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_minus_sqrt_1minus4x_div_2x_series<T, K>::operator()(K n) const
{
	const T xn = static_cast<T>(std::pow(this->x, n));
	const T binom_coef = static_cast<T>(this->binomial_coefficient(static_cast<T>(0.5), n + static_cast<K>(1)));
	const T power_2 = this->minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(2, std::fma(2, n, 1)));

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
arcsin_x_minus_x_series<T, K>::arcsin_x_minus_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::asin(x)) - x)
{
	this->series_name = "arcsin(x)-x";
	// Сходится при |x| ≤ 1 (ряд для arcsin(x) сходится при |x| ≤ 1)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsin_x_minus_x_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	return (static_cast<T>(this->double_fact(a)) * static_cast<T>(std::pow(this->x, a+2))) / (static_cast<T>(this->double_fact(a+1)) * static_cast<T>(a+2)); // (55.3) [Rows.pdf]
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
pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x) : series_base<T, K>(x, 0 < x && x < std::numbers::pi ? static_cast<T>(std::numbers::pi) * x - x * x : (x * x) - (static_cast<T>(3) * static_cast<T>(std::numbers::pi) * x) + (2 * static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi)))
{
	this->series_name = "f(x) = { πx - x², 0 < x < π; x² - 3πx + 2π², π ≤ x < 2π }";
	// Сходится при 0 < x < 2π (ряд Фурье для кусочно-квадратичной функции)
	// Расходится при x ≤ 0 или x ≥ 2π

	if (x <= static_cast<T>(0) || x >= static_cast<T>(2) * static_cast<T>(std::numbers::pi) || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (0, 2π)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(std::fma(2, n, 1));
	return (static_cast<T>(8) / std::numbers::pi_v<T>) * (static_cast<T>(std::sin(a * (this->x))) / static_cast<T>(std::pow(a, 3))); // (56.2) [Rows.pdf]
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
abs_sin_x_minus_2_div_pi_series<T, K>::abs_sin_x_minus_2_div_pi_series(T x) : series_base<T, K>(x, 0 <= x && x <= static_cast<T>(std::numbers::pi) ? static_cast<T>(std::sin(x)) - (static_cast<T>(2) / static_cast<T>(std::numbers::pi)) : -static_cast<T>(std::sin(x)) - (static_cast<T>(2) / static_cast<T>(std::numbers::pi)))
{
	this->series_name = "|sin(x)| - 2/π";
	// Сходится при 0 ≤ x ≤ 2π (ряд Фурье для функции |sin(x)|)
	// Расходится при x < 0 или x > 2π

	if (x < 0 || x > 2 * std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("x must be in [0, 2π]");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T abs_sin_x_minus_2_div_pi_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(std::fma(2, n, 1));
	return static_cast<T>(-4) * static_cast<T>((std::cos((this->x) * (a + static_cast<T>(1))))) / (a * (a + static_cast<T>(2)) * static_cast<T>(std::numbers::pi)); // (57.2) [Rows.pdf]
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
pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(T x) : series_base<T, K>(x, -static_cast<T>(std::numbers::pi) < x && x < 0 ? static_cast<T>(std::numbers::pi) - static_cast<T>(3) * static_cast<T>(std::numbers::pi) / static_cast<T>(4) : static_cast<T>(std::numbers::pi) - x - static_cast<T>(3) * static_cast<T>(std::numbers::pi) / static_cast<T>(4))
{
	this->series_name = "f(x) = { π - 3π/4, -π < x < 0; π - x - 3π/4, 0 ≤ x ≤ π }";
	// Сходится при -π < x ≤ π (ряд Фурье для кусочно-линейной функции)
	// Расходится при x ≤ -π или x > π

	if (x <= -static_cast<T>(std::numbers::pi) || x > static_cast<T>(std::numbers::pi) || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (-π, π]");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const T b = a * this->x;
	return (static_cast<T>(std::cos(b)) * (static_cast<T>(1) - this->minus_one_raised_to_power_n(static_cast<K>(a)))) / (static_cast<T>(a) * static_cast<T>(a) * static_cast<T>(std::numbers::pi)) + this->minus_one_raised_to_power_n(static_cast<K>(a)) * static_cast<T>(std::sin(b)) / static_cast<T>(a); // (58.5) [Rows.pdf]
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
minus_3_div_4_or_x_minus_3_div_4_series<T, K>::minus_3_div_4_or_x_minus_3_div_4_series(T x) : series_base<T, K>(x, -3 < x && x < 0 ? -static_cast<T>(3) / static_cast<T>(4) : x - static_cast<T>(3) / static_cast<T>(4))
{
	this->series_name = "f(x) = { -3/4, -3 < x < 0; x - 3/4, 0 ≤ x < 3 }";
	// Сходится при -3 < x < 3 (ряд Фурье для кусочно-линейной функции на интервале [-3, 3])
	// Расходится при x ≤ -3 или x ≥ 3

	if (x <= -3 || x >= 3 || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (-3, 3)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_3_div_4_or_x_minus_3_div_4_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(n + static_cast<K>(1));
	const T b = a + static_cast<T>(n); // 2 * n + 1
	const T c = this->x * static_cast<T>(std::numbers::pi) / static_cast<T>(3);
	const T d = static_cast<T>(std::numbers::pi) * b;
	return static_cast<T>(-6) * static_cast<T>(std::cos(b * c)) / (d * d) - static_cast<T>(3) * this->minus_one_raised_to_power_n(static_cast<K>(a)) * static_cast<T>(std::sin(a * c)) / (a * static_cast<T>(std::numbers::pi));  // (59.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
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
ten_minus_x_series<T, K>::ten_minus_x_series(T x) : series_base<T, K>(x, static_cast<T>(10) - x)
{
	this->series_name = "10-x";
	// Сходится при 5 < x < 15 (ряд Фурье для линейной функции на интервале [5, 15])
	// Расходится при x ≤ 5 или x ≥ 15

	if (x <= static_cast<T>(5) || x >= static_cast<T>(15) || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (5, 15)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ten_minus_x_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(n + static_cast<K>(1));
	const T b = a * static_cast<T>(std::numbers::pi);
	return static_cast<T>(10) * this->minus_one_raised_to_power_n(static_cast<K>(a)) * static_cast<T>(std::sin(b * this->x / static_cast<T>(5))) / b; // (60.4) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
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
	this->series_name = "x";
	// Сходится при -π < x < π (ряд Фурье для функции f(x) = x)
	// Расходится при x ≤ -π или x ≥ π

	if (x < -std::numbers::pi || x > std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (-π, π)");
	}
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
	this->series_name = "f(x) = { -x - π/4, -π < x < 0; -π/4, 0 ≤ x < π }";
	// Сходится при -π < x < π (ряд Фурье для кусочно-линейной функции)
	// Расходится при x ≤ -π или x ≥ π

	if (x <= -std::numbers::pi || x >= std::numbers::pi || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (-π, π)");
	}
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
	this->series_name = "1/((2-x)(3+x))";
	// Сходится при |x| < 2 (радиус сходимости определяется полюсами в x=2 и x=-3)
	// Расходится при |x| ≥ 2

	if (std::abs(x) >= 2 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 2");
	}
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
Si_x_series<T, K>::Si_x_series(T x) : series_base<T, K>(x)
{
	this->series_name = "Si(x)";
	// Сходится при ∀x ∈ ℝ (интегральный синус, ряд сходится на всей числовой прямой)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
Ci_x_series<T, K>::Ci_x_series(T x) : series_base<T, K>(x)
{
	this->series_name = "Ci(x)";
	// Сходится при ∀x ∈ ℝ\{0} (интегральный косинус, ряд сходится на всей числовой прямой кроме x=0)
	// При x=0 требуется специальное рассмотрение (предел)

	if (x == 0 || !std::isfinite(x)) {
		this->throw_domain_error("x must be finite and non-zero");
	}
}

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
	this->series_name = "ζ(x)";
	// Сходится при x > 1 (дзета-функция Римана)
	// Расходится при x ≤ 1

	if (x <= 1 || !std::isfinite(x)) {
		this->throw_domain_error("x must be > 1");
	}
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
	this->series_name = "ζ(x-1)/ζ(x)";
	// Сходится при x > 2 (отношение дзета-функций, ζ(x) сходится при x > 1, ζ(x-1) при x > 2)
	// Расходится при x ≤ 2

	if (x <= 2 || !std::isfinite(x)) {
		this->throw_domain_error("x must be > 2");
	}
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
xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::xsquareplus3_div_xsquareplus2multix_minus_1_series(T x) : series_base<T, K>(x, ((x* x + 3) / x * (x + 2)) - 1)
{
	this->series_name = "((x²+3)/x)*(x+2)-1";
	// Сходится при |x| > 2 (ряд Лорана, разложение в окрестности бесконечности)
	// Расходится при |x| ≤ 2

	if (std::abs(x) <= 2 || !std::isfinite(x) || x == 0) {
		this->throw_domain_error("|x| must be > 2 and x != 0");
	}
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
	this->series_name = "arcsin(x)";
	// Сходится при |x| ≤ 1 (биномиальный ряд для arcsin(x))
	// Расходится при |x| > 1

	if (x < -1 || x > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
	this->series_name = "arctg(x)";
	// Сходится при |x| ≤ 1 (ряд для arctg(x) сходится абсолютно при |x| < 1, условно при |x| = 1)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
	this->series_name = "K(x)";
	// Сходится при |x| < 1 (полный эллиптический интеграл первого рода)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
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
	this->series_name = "E(x)";
	// Сходится при |x| < 1 (полный эллиптический интеграл второго рода)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
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
	this->series_name = "sqrt(1+x)";
	// Сходится при |x| ≤ 1 (биномиальный ряд, абсолютно при |x| < 1, условно при x = -1)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
	this->series_name = "W(x)";
	// Сходится при |x| < 1/e (ряд для функции Ламберта)
	// Расходится при |x| ≥ 1/e

	if (std::abs(x) >= 1 / std::numbers::e || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1/e");
	}
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
Incomplete_Gamma_func_series<T, K>::Incomplete_Gamma_func_series(T x, T s) : series_base<T, K>(x), s(s)
{
	this->series_name = "γ(s,x)";
	// Сходится при ∀x ∈ ℝ, s > 0 (неполная гамма-функция)
	// Расходится при s ≤ 0

	if (s <= 0 || !std::isfinite(x) || !std::isfinite(s)) {
		this->throw_domain_error("s must be > 0 and parameters must be finite");
	}
}

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
class Series_with_ln_number1_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Series_with_ln_number1_series(T x);

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
Series_with_ln_number1_series<T, K>::Series_with_ln_number1_series(T x) : series_base<T, K>(x)
{
	this->series_name = "series_with_ln";
	// Сходится при |x| < 1 (ряд с логарифмическими членами)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Series_with_ln_number1_series<T, K>::operator()(K n) const
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
class Series_with_ln_number2_series : public series_base<T, K>
{
public:
	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	Series_with_ln_number2_series(T x);

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
Series_with_ln_number2_series<T, K>::Series_with_ln_number2_series(T x) : series_base<T, K>(x)
{
	this->series_name = "series_with_ln2";
	// Сходится при |x| < 1 (ряд с логарифмическими членами)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Series_with_ln_number2_series<T, K>::operator()(K n) const
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
pi_series<T, K>::pi_series(T x) : series_base<T, K>(x)
{
	this->series_name = "π";
	// Сходится при ∀x ∈ ℝ (константа π)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
	this->series_name = "x-√x";
	// Сходится при 0 < x < 1 (разложение в ряд Тейлора)
	// Расходится при x ≤ 0 или x ≥ 1

	if (x <= 0 || x >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (0, 1)");
	}
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
arctan_x2_series<T, K>::arctan_x2_series(T x) : series_base<T, K>(x, std::atan(x* x))
{
	this->series_name = "arctan(x²)";
	// Сходится при |x| ≤ 1 (ряд для arctan(z) сходится при |z| ≤ 1, где z = x²)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
	this->series_name = "ln(1+x⁴)";
	// Сходится при |x| ≤ 1 (ряд для ln(1+z) сходится при |z| ≤ 1, где z = x⁴)
	// Расходится при |x| > 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
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
sin_x2_series<T, K>::sin_x2_series(T x) : series_base<T, K>(x, std::sin(x * x))
{
	this->series_name = "sin(x²)";
	// Сходится при ∀x ∈ ℝ (ряд для sin(z) сходится при ∀z ∈ ℝ, где z = x²)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
	this->series_name = "arctan(x³)";
	// Сходится при |x| ≤ 1 (ряд для arctan(z) сходится при |z| ≤ 1, где z = x³)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
arcsin_x2_series<T, K>::arcsin_x2_series(T x) : series_base<T, K>(x, std::asin(x* x))
{
	this->series_name = "arcsin(x²)";
	// Сходится при |x| ≤ 1 (ряд для arcsin(z) сходится при |z| ≤ 1, где z = x²)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
	this->series_name = "ln(1-x²)";
	// Сходится при |x| < 1 (ряд для ln(1-z) сходится при |z| < 1, где z = x²)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
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
	this->series_name = "artanh(x)";
	// Сходится при |x| < 1 (ряд для гиперболического арктангенса)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
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
	this->series_name = "arcsinh(x)";
	// Сходится при |x| ≤ 1 (ряд для гиперболического арксинуса)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
cos_x2_series<T, K>::cos_x2_series(T x) : series_base<T, K>(x, std::cos(x * x))
{
	this->series_name = "cos(x²)";
	// Сходится при ∀x ∈ ℝ (ряд для cos(z) сходится при ∀z ∈ ℝ, где z = x²)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
sinh_x2_series<T, K>::sinh_x2_series(T x) : series_base<T, K>(x, std::sinh(x * x))
{
	this->series_name = "sinh(x²)";
	// Сходится при ∀x ∈ ℝ (ряд для sinh(z) сходится при ∀z ∈ ℝ, где z = x²)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
	this->series_name = "arctanh(x²)";
	// Сходится при |x| < 1 (ряд для гиперболического арктангенса, где z = x²)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
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
cos3xmin1_div_xsqare_series<T, K>::cos3xmin1_div_xsqare_series(T x) : series_base<T, K>(x, (std::cos(3 * x) - 1) / (x * x))
{
	this->series_name = "(cos(3x)-1)/x²";
	// Сходится при ∀x ∈ ℝ\{0} (ряд для (cos(z)-1)/z² сходится при ∀z ∈ ℝ\{0}, где z = 3x)
	// При x = 0 требуется специальное рассмотрение (предел)

	if (x == 0 || !std::isfinite(x)) {
		this->throw_domain_error("x must be finite and non-zero");
	}
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
two_degree_x_series<T, K>::two_degree_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(2, x)))
{
	this->series_name = "2^x";
	// Сходится при ∀x ∈ ℝ (экспоненциальная функция 2^x = e^(x·ln2))

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
	this->series_name = "√(1+x)-1-x/2";
	// Сходится при |x| ≤ 1 (биномиальный ряд, абсолютно при |x| < 1, условно при x = -1)
	// Расходится при |x| > 1

	if (std::abs(x) > 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be <= 1");
	}
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
ln13_min_ln7_div_7_series<T, K>::ln13_min_ln7_div_7_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(13 / 7) * x / 7))
{
	this->series_name = "(ln(13 / 7))·x/7";
	// Сходится при ∀x ∈ ℝ (линейная функция)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
Ja_x_series<T, K>::Ja_x_series(T x, T a) : series_base<T, K>(x), a(a)
{
	this->series_name = "Jₐ(x)";
	// Сходится при ∀x ∈ ℝ (функция Бесселя первого рода)
	// Сходится при ∀a ∈ ℝ

	if (!std::isfinite(x) || !std::isfinite(a)) {
		this->throw_domain_error("x and a must be finite");
	}
}

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
one_div_sqrt2_sin_xdivsqrt2_series<T, K>::one_div_sqrt2_sin_xdivsqrt2_series(T x) : series_base<T, K>(x, static_cast<T>((1 / std::sqrt(2)) * std::sin(x / std::sqrt(2))))
{
	this->series_name = "(1/√2)*sin(x/√2)";
	// Сходится при ∀x ∈ ℝ (ряд для sin(z) сходится при ∀z ∈ ℝ, где z = x/√2)

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
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
class ln_1plusx_div_1plusx2_series : public series_base<T, K>
{
public:
	ln_1plusx_div_1plusx2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_1plusx_div_1plusx2_series(T x);

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
ln_1plusx_div_1plusx2_series<T, K>::ln_1plusx_div_1plusx2_series(T x) : series_base<T, K>(x, std::log(1 + x) / (1 + x * x))
{
	this->series_name = "ln(1+x)/(1+x²)";
	// Сходится при |x| < 1 (ряд для ln(1+x) сходится при |x| < 1, знаменатель 1+x² > 0 при ∀x ∈ ℝ)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1plusx_div_1plusx2_series<T, K>::operator()(K n) const
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
class cos_sqrt_x_series : public series_base<T, K>
{
public:
	cos_sqrt_x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cos_sqrt_x_series(T x);

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
cos_sqrt_x_series<T, K>::cos_sqrt_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::cos(std::sqrt(x))))
{
	this->series_name = "cos(√x)";
	// Сходится при x ≥ 0 (ряд для cos(z) сходится при ∀z ∈ ℝ, где z = √x)
	// Расходится при x < 0 (комплексная область)

	if (x < 0 || !std::isfinite(x)) {
		this->throw_domain_error("x must be >= 0");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cos_sqrt_x_series<T, K>::operator()(K n) const
{
	return static_cast<T>((1 - 2 * (n & 1)) * std::pow(this->x, n) / this->fact(2 * n)); // (1 - 2 * (n & 1)) = (-1)^{n} (98.2) [Rows.pdf]
}


/**
* @brief Maclaurin series for function ln(1 + x^3)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_1_plus_x3_series : public series_base<T, K>
{
public:
	ln_1_plus_x3_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_1_plus_x3_series(T x);

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
ln_1_plus_x3_series<T, K>::ln_1_plus_x3_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(1 + std::pow(x, 3))))
{
	this->series_name = "ln(1+x³)";
	// Сходится при |x| ≤ 1 (ряд для ln(1+z) сходится при |z| ≤ 1, где z = x³)
	// Расходится при |x| > 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1_plus_x3_series<T, K>::operator()(K n) const
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
class x_div_1minx_series : public series_base<T, K>
{
public:
	x_div_1minx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_div_1minx_series(T x);

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
x_div_1minx_series<T, K>::x_div_1minx_series(T x) : series_base<T, K>(x, static_cast<T>(x / std::sqrt(1 - x)))
{
	this->series_name = "x/√(1-x)";
	// Сходится при |x| < 1 (биномиальный ряд в знаменателе)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_div_1minx_series<T, K>::operator()(K n) const
{
	return static_cast<T>(this->fact(2 * n) * std::pow(this->x, n + 1) / (this->fact(n) * std::pow(4, n))); // (100.1) [Rows.pdf]
}


/**
* @brief Maclaurin series for function x / sqrt(1 - x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_div_1minx2_series : public series_base<T, K>
{
public:
	x_div_1minx2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pavlova A.R.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	x_div_1minx2_series(T x);

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
x_div_1minx2_series<T, K>::x_div_1minx2_series(T x) : series_base<T, K>(x, static_cast<T>(x / std::sqrt(1 - x * x)))
{
	this->series_name = "x/√(1-x²)";
	// Сходится при |x| < 1 (биномиальный ряд в знаменателе)
	// Расходится при |x| ≥ 1

	if (std::abs(x) >= 1 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_div_1minx2_series<T, K>::operator()(K n) const
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
gamma_series<T, K>::gamma_series(T t, T x) : series_base<T, K>(x), t(t), x(x)
{
	this->series_name = "Γ(t,x)";
	// Сходится при ∀x ∈ ℝ, t > 0 (неполная гамма-функция)
	// Расходится при t ≤ 0

	if (t <= 0 || !std::isfinite(x) || !std::isfinite(t)) {
		this->throw_domain_error("t must be > 0 and parameters must be finite");
	}
}

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
testing_series<T, K>::testing_series(T x) : series_base<T, K>(x, 0) 
{
	this->series_name = "testing_series";
	// Область сходимости зависит от конкретной тестовой функции
	// Требуется уточнение для конкретного тестового случая

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

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
reccurent_testing_series<T, K>::reccurent_testing_series(T x) : series_base<T, K>(x, 0), reccurent_series_base<T, K>(x)
{
	this->series_name = "reccurent_testing_series";
	// Область сходимости зависит от конкретной тестовой рекуррентной функции
	// Требуется уточнение для конкретного тестового случая

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T reccurent_testing_series<T, K>::access_row(K n)
{
	auto& series_vec = this->get_series_vector();
	K old_size = series_vec.size();
	series_vec.reserve(n);

	for (K i = old_size; i <= n; ++i)
		series_vec.push_back(series_vec[i - 1] * (this->x * this->x) / (i * std::fma(4, i, 2)));

	return series_vec[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T reccurent_testing_series<T, K>::operator()(K n) const
{
	return const_cast<reccurent_testing_series<T, K>*>(this)->access_row(n);
}