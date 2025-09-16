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
 * @brief This file contains series base class, recurrent_series_base class and derived classes of various serieses (e.g. exp(x), ch(x))
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
	virtual ~series_base() = default;

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
* @brief Abstract class for recurrent series
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class recurrent_series_base
{
public:
	/**
	* @brief Parameterized constructor to initialize the recurrent series with function argument
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	recurrent_series_base(T x);

	/**
	* @brief Parameterized constructor to initialize the recurrent series with vector, containing elements of series
	* @authors Kreynin R.G.
	* @param row The first elements of the series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	recurrent_series_base(std::vector<T> row);

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
recurrent_series_base<T, K>::recurrent_series_base(T x)
{
	this->series_vector.push_back(x);
};


template <std::floating_point T, std::unsigned_integral K>
recurrent_series_base<T, K>::recurrent_series_base(std::vector<T> row) : series_vector(std::move(row))
{
	if (this->series_vector.empty()) {
		this->throw_domain_error("empty initial row");
	}
};
