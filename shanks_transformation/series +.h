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
	 for (K i = 0; i < n; ++i)
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
	 K f = 1;
	 for (K i = 2; i <= n; ++i)
		 f *= i;
	 return f;
 }
 
 template <typename T, typename K>
 constexpr const T series_base<T, K>::binomial_coefficient(const T n, const K k)
 {
	 T b_c = 1;
	 for (K i = 0; i < k; ++i)
		 b_c = b_c * (n - static_cast<T>(i)) / (i + 1);
	 return b_c;
 }
 
 template <typename T, typename K>
 constexpr const T series_base<T, K>::minus_one_raised_to_power_n(K n)
 {
	 return static_cast<T>(n % 2 ? -1.0 : 1.0);
 }
 
 
 template <typename T, typename K>
 constexpr const T series_base<T, K>::phi(K n)
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 K result = n;
	 for (K i = 2; i * i <= n; ++i)
		 if (n % i == 0) {
			 while (n % i == 0)
				 n /= i;
			 result -= result / i;
		 }
	 if (n > 1)
		 result -= result / n;
	 return static_cast<T>(result);
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
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
	 {
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
 cos_series<T, K>::cos_series(T x) : series_base<T, K>(x, std::cos(x)), requrrent_series_base<T, K>(std::vector<T>{1, T(((-1) * x * x) / 2)}) {}
 
 template <typename T, typename K>
 T cos_series<T, K>::acsess_row(K n)
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		 this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(-1) * static_cast<T>((this->x * this->x) / (i * (4 * i - 2))));
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
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		 this->series_vector.push_back(this->series_vector[i - 1] * T(-1) * static_cast<T>((this->x * this->x) / (i * (4 * i + 2))));
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
 cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x)), requrrent_series_base<T, K>(std::vector<T>{1, T((x * x) / 2) }) {}
 
 template <typename T, typename K>
 T cosh_series<T, K>::acsess_row(K n)
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		 this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>((this->x * this->x) / (i * (4 * i - 2))));
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
 sinh_series<T, K>::sinh_series(T x) : series_base<T, K>(x, sinh(x)), requrrent_series_base<T, K>(x) {}
 
 template <typename T, typename K>
 T sinh_series<T, K>::acsess_row(K n)
 {
	 if (n < 0)
		 throw domain_error("negative integer in the input");
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		 this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>((this->x * this->x) / (i * (4 * i + 2))));
	 }
 
	 return this->series_vector[n];
 }
 
 template <typename T, typename K>
 constexpr T sinh_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw domain_error("negative integer in the input");
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
	 return static_cast<T>(binomial_coefficient(alpha, n) * std::pow(this->x, n));
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
	 // T acsess_row(K n);
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
	 return static_cast<T>(4 * series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, 2 * n + 1) / (2 * n + 1));
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
	 return static_cast<T>(std::pow(this->x, n + 1) / (n + 1));
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
 mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, static_cast<T>(0.5 * (std::sinh(x) + std::sin(x)))), requrrent_series_base<T, K>(x) {}
 
 template <typename T, typename K>
 T mean_sinh_sin_series<T, K>::acsess_row(K n)
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		 this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(std::pow(this->x, 4) / ((4 * i + 1) * (4 * i) * (4 * i - 1) * (4 * i - 2))));
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
 exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, std::exp(x * x) * std::erf(x)) {}
 
 template <typename T, typename K>
 constexpr T exp_squared_erf_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 const auto result = std::pow(this->x, 2 * n + 1) / std::tgamma(n + 1.5);
	 if (!isfinite(result))
		 throw std::overflow_error("operator() is too big");
	 return static_cast<T>(result);
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
 xmb_Jb_two_series<T, K>::xmb_Jb_two_series(T x, K b) : series_base<T, K>(x, static_cast<T>(std::pow(x, -b) * std::cyl_bessel_j(b, 2 * x))), mu(b) {}
 
 template <typename T, typename K>
 constexpr T xmb_Jb_two_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, 2 * n) / (static_cast<T>(this->fact(n)) * static_cast<T>(this->fact(n + this->mu))));
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
 half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, static_cast<T>(0.5 * std::asin(2 * x)))
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
	 return static_cast<T>(static_cast<T>(this->fact(2 * n)) * std::pow(this->x, 2 * n) / (_fact_n * _fact_n * (2 * n + 1))); // p. 566 typo
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
	 return static_cast<T>(std::pow(this->x, n));
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
	 return static_cast<T>(std::pow(this->x, n) * n);
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
 erf_series<T, K>::erf_series(T x) : series_base<T, K>(x, static_cast<T>(std::sqrt(std::numbers::pi)) * static_cast<T>(std::erf(x) / 2)), requrrent_series_base<T, K>(x) { }
 
 template <typename T, typename K>
 T erf_series<T, K>::acsess_row(K n)
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		 this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>((-1)) * static_cast<T>((this->x * this->x) / i  * (2 * i - 1) / (2 * i + 1)));
	 }
 
	 return static_cast<T>(this->series_vector[n]);
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
 m_fact_1mx_mp1_inverse_series<T, K>::m_fact_1mx_mp1_inverse_series(T x, K m) : series_base<T, K>(x, static_cast<T>(static_cast<T>(this->fact(m)) / pow(1 - x, m + 1))), m(m), requrrent_series_base<T, K>(static_cast<T>(this->fact(m)))
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
 
	 auto old_size = this->series_vector.size();
	 this->series_vector.reserve(n);
 
	 for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
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
 inverse_sqrt_1m4x_series<T, K>::inverse_sqrt_1m4x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(std::fma(-4, x, 1), -0.5)))
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
	 return this->fact(2 * n) * static_cast<T>(pow(this->x, n) / (_fact_n * _fact_n));
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
 one_twelfth_3x2_pi2_series<T, K>::one_twelfth_3x2_pi2_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(0.25 * x, x, -std::numbers::pi * std::numbers::pi / 12)))
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
 x_twelfth_x2_pi2_series<T, K>::x_twelfth_x2_pi2_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(x / 12, (x + std::numbers::pi)* (x - std::numbers::pi), -std::fma(x + std::numbers::pi, x - std::numbers::pi, (x + std::numbers::pi) * (x - std::numbers::pi)))))
 {
	 if (std::abs(this->x) > std::numbers::pi)
		 throw std::domain_error("series diverge");
 }
 
 template <typename T, typename K>
 constexpr T x_twelfth_x2_pi2_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return n ? static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * std::sin(n * this->x) / (std::pow(n , 3))) : 0;
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
 ln2_series<T, K>::ln2_series() : series_base<T, K>(0, static_cast<T>(std::log(2))) {}
 
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
	 return n ? static_cast<T>(1.0 / fma(n, n, n)) : 0;
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
	 return n ? series_base<T, K>::minus_one_raised_to_power_n(n) / (n * (n + 2)) : 0;
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
 pi_3_series<T, K>::pi_3_series() : series_base<T, K>(0, static_cast<T>(std::numbers::pi / 3)) {}
 
 template <typename T, typename K>
 constexpr T pi_3_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(1.0 / ((n + 1) * (2 * n + 1) * (4 * n + 1)));
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
 pi_4_series<T, K>::pi_4_series() : series_base<T, K>(0, static_cast<T>(0.25 * std::numbers::pi)) {} 
 
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
 pi_squared_6_minus_one_series<T, K>::pi_squared_6_minus_one_series() : series_base<T, K>(0, static_cast<T>(std::fma(std::numbers::pi / 6, std::numbers::pi, -1))) {}
 
 template <typename T, typename K>
 constexpr T pi_squared_6_minus_one_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(n ? 1.0 / (n * n * (n + 1)) : 0);
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
 three_minus_pi_series<T, K>::three_minus_pi_series() : series_base<T, K>(0, static_cast<T>(3 - std::numbers::pi)) {}
 
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
	 return static_cast<T>(1.0 / ((2 * n + 1) * (2 * n + 3) * (2 * n + 5)));
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
 eighth_pi_m_one_third_series<T, K>::eighth_pi_m_one_third_series() : series_base<T, K>(0, static_cast<T>(std::numbers::pi / 8 - 1 / 3)) {}
 
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
 one_third_pi_squared_m_nine_series<T, K>::one_third_pi_squared_m_nine_series() : series_base<T, K>(0, static_cast<T>(std::fma(std::numbers::pi, std::numbers::pi, -9) / 3)) {}
 
 template <typename T, typename K>
 constexpr T one_third_pi_squared_m_nine_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(n ? 1.0 / (n * n * (n + 1) * (n + 1)) : 0);
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
 four_ln2_m_3_series<T, K>::four_ln2_m_3_series() : series_base<T, K>(0, static_cast<T>(std::fma(4, std::log(2), -3))) {}
 
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
 pi_four_minus_ln2_halfed_series<T, K>::pi_four_minus_ln2_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(x * static_cast<T>((std::numbers::pi / 4 - std::log(2) / 2)))) {}
 
 template <typename T, typename K>
 constexpr T pi_four_minus_ln2_halfed_series<T, K>::operator()(K n) const 
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (n == 0)
		 return 0;
	 return this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 2))) / n;
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
 five_pi_twelve_series<T, K>::five_pi_twelve_series(T x) : series_base<T, K>(x, static_cast<T>(x * 5 * std::numbers::pi / 12)) {}
 
 template <typename T, typename K>
 constexpr T five_pi_twelve_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 3))) / (2*n + 1);
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
 pi_six_min_half_series<T, K>::pi_six_min_half_series(T x) : series_base<T, K>(x, static_cast<T>(x * (std::numbers::pi / 6  - 1 / 2))) {}
 
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
	 return static_cast<T>(this->x * static_cast<T>((2 * (n * n) - 1) / (4 * std::pow(n, 4) + 1)));
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
 minus_one_ned_in_n_series<T, K>::minus_one_ned_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(- 0.78343051 * x)) {}
 
 template <typename T, typename K>
 constexpr T minus_one_ned_in_n_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (n == 0)
		 return 0;
	 return this->x * static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) / std::pow(n,n));
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
 minus_one_n_fact_n_in_n_series<T, K>::minus_one_n_fact_n_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(- 0.65583160 * x)) {}
 
 template <typename T, typename K>
 constexpr T minus_one_n_fact_n_in_n_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (n == 0)
		 return 0;
	 return static_cast<T>(this->x * this->fact(n) * series_base<T, K>::minus_one_raised_to_power_n(n) / std::pow(n, n));
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
	 return static_cast<T>(std::pow(this->x, 2 * n + 1) / (2 * n + 1));
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
 two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(2 * std::pow(std::asin(x / 2), 2))) {}
 
 template <typename T, typename K>
 constexpr T two_arcsin_square_x_halfed_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return  static_cast<T>((pow(this->x, 2 * n + 2) * this->fact(n) * this->fact(n)) / (this->fact(2 * n + 2)));
 }
 
 
 /**
 * @brief Maclaurin series of value pi^2/12
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class pi_squared_twelve_series : public series_base<T, K>
 {
 public:
	 /**
	 * @brief Parameterized constructor
	 * @authors Trudolyubov N.A.
	 */
	 pi_squared_twelve_series();
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the cosine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the cosine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 pi_squared_twelve_series<T, K>::pi_squared_twelve_series() : series_base<T, K>(0, static_cast<T>((std::numbers::pi * std::numbers::pi) / 12)) {}
 
 template <typename T, typename K>
 constexpr T pi_squared_twelve_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return (this->minus_one_raised_to_power_n(n) / static_cast<T>(((n + 1) * (n + 1))));
 }
 
 /**
 * @brief Maclaurin series of value pi^3/32
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class pi_cubed_32_series : public series_base<T, K>
 {
 public:
	 /**
	 * @brief Parameterized constructor
	 * @authors Trudolyubov N.A.
	 */
	 pi_cubed_32_series();
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the cosine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the cosine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 pi_cubed_32_series<T, K>::pi_cubed_32_series() : series_base<T, K>(0, static_cast<T>(std::pow(std::numbers::pi, 3) / 32)) {}
 
 template <typename T, typename K>
 constexpr T pi_cubed_32_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 T n_temp = static_cast<T>(n+1);
	 return this->minus_one_raised_to_power_n(n) / static_cast<T>((std::pow(2 * n_temp - 1, 3)));
 }
 
 /**
 * @brief Maclaurin series of value -3 + (3/2)*ln3 + 2*ln2
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class minus_three_plus_ln3_three_devided_two_plus_two_ln2_series : public series_base<T, K>
 {
 public:
	 /**
	 * @brief Parameterized constructor
	 * @authors Trudolyubov N.A.
	 */
	 minus_three_plus_ln3_three_devided_two_plus_two_ln2_series();
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the cosine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the cosine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series() : series_base<T, K>(0, static_cast<T>(- 3 + (3 / 2) * std::log(3) + 2 * std::log(2))) {}
 
 template <typename T, typename K>
 constexpr T minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 T n_temp = static_cast<T>(n + 1);
 
	 return 1 / (n_temp*(36 * n_temp * n_temp - 1));
 }
 
 
 /**
 * @brief Maclaurin series of value 2*ln2
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class two_ln2_series : public series_base<T, K>
 {
 public:
	 /**
	 * @brief Parameterized constructor
	 * @authors Trudolyubov N.A.
	 */
	 two_ln2_series();
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the cosine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the cosine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 two_ln2_series<T, K>::two_ln2_series() : series_base<T, K>(0, static_cast<T>(2 * std::log(2))) {}
 
 template <typename T, typename K>
 constexpr T two_ln2_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
 
	 T n_temp = static_cast<T>(n + 1);
 
	 return (12 * n_temp * n_temp - 1) / (n_temp * (4 * n_temp * n_temp - 1) * (4 * n_temp * n_temp - 1));
 }
 
 /**
 * @brief Maclaurin series of function pi * x * (e^(pi*x) + e^(-pi*x)) / (e^(pi*x) - e^(-pi*x)) - 1
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series : public series_base<T, K>
 {
 public:
	 pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series(T x) : series_base<T, K>(x, static_cast<T>(std::numbers::pi* x * 2.0 * std::cosh(std::numbers::pi * x) / (2.0 * std::sinh(std::numbers::pi * x)) - 1)) {}
 
 template <typename T, typename K>
 constexpr T pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x == 0)
		 throw std::domain_error("x cannot be zero");
	 return (2*(this->x)*(this->x))/((this->x)*(this->x) + (n + 1)*(n + 1));
 }
 
 /**
 * @brief Maclaurin series of function (pi - x) / 2
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class pi_minus_x_2 : public series_base<T, K>
 {
 public:
	 pi_minus_x_2() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 pi_minus_x_2(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 pi_minus_x_2<T, K>::pi_minus_x_2(T x) : series_base<T, K>(x, static_cast<T>((std::numbers::pi - x) / 2)) {}
 
 template <typename T, typename K>
 constexpr T pi_minus_x_2<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if(this->x <= 0 or this->x >= 2*std::numbers::pi)
		 throw std::domain_error("The x value must be between 0 and 2*pi");
	 return std::sin((n + 1) * (this->x)) / (n + 1);
 }
 
 
 /**
 * @brief Maclaurin series of function 0.5 * ln(1/(2*(1 - cos(x))))
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class half_multi_ln_1div2multi1minuscosx : public series_base<T, K>
 {
 public:
	 half_multi_ln_1div2multi1minuscosx() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 half_multi_ln_1div2multi1minuscosx(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 half_multi_ln_1div2multi1minuscosx<T, K>::half_multi_ln_1div2multi1minuscosx(T x) : series_base<T, K>(x, static_cast<T>(0.5 * std::log(1 / (2 - 2*std::cos(x))))) {}
 
 template <typename T, typename K>
 constexpr T half_multi_ln_1div2multi1minuscosx<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= 0 or this->x >= 2 * std::numbers::pi)
		 throw std::domain_error("The x value must be between 0 and 2*pi");
	 return std::cos((n + 1) * (this->x)) / (n + 1);
 }
 
 
 /**
 * @brief Maclaurin series of function 0.5 - (pi/4)*sin(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class half_minus_sinx_multi_pi_4 : public series_base<T, K>
 {
 public:
	 half_minus_sinx_multi_pi_4() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 half_minus_sinx_multi_pi_4(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 half_minus_sinx_multi_pi_4<T, K>::half_minus_sinx_multi_pi_4(T x) : series_base<T, K>(x, static_cast<T>((static_cast<T>(0.5) - std::numbers::pi * std::sin(x) * static_cast<T>(0.25)))) {}
 
 template <typename T, typename K>
 constexpr T half_minus_sinx_multi_pi_4<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return std::cos(2*(this->x)*(n + 1)) / ((2*n + 1)*(2*n + 3));
 }
 
 /**
 * @brief Maclaurin series of function ln(1 + sqrt(1 + x^2)) - ln(2)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class ln_1plussqrt1plusxsquare_minus_ln_2 : public series_base<T, K>
 {
 public:
	 ln_1plussqrt1plusxsquare_minus_ln_2() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 ln_1plussqrt1plusxsquare_minus_ln_2(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 ln_1plussqrt1plusxsquare_minus_ln_2<T, K>::ln_1plussqrt1plusxsquare_minus_ln_2(T x) : series_base<T, K>(x, static_cast<T>(std::log((1 + std::hypot(1, x)) / 2))) {}
 
 template <typename T, typename K>
 constexpr T ln_1plussqrt1plusxsquare_minus_ln_2<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if((this->x)*(this->x) > 1)
		 throw std::domain_error("x^2 cannot be more than 1");
	 return static_cast<T>((this->minus_one_raised_to_power_n(n) * this->fact(2*n + 1) * std::pow(this->x, 2*n + 2)) / (pow(2, 2*n + 2) * this->fact(n+1) * this->fact(n+1)));
 }
 
 
 /**
 * @brief Maclaurin series of function ln(cos(x))
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class ln_cosx : public series_base<T, K>
 {
 public:
	 ln_cosx() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 ln_cosx(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 ln_cosx<T, K>::ln_cosx(T x) : series_base<T, K>(x, std::log(std::cos(x))) {}
 
 template <typename T, typename K>
 constexpr T ln_cosx<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
 
	 return static_cast<T>(std::log(1 - 4*(this->x)*(this->x) / ((2*n + 1)*(2*n + 1) * std::numbers::pi * std::numbers::pi)));
 }
 
 
 /**
 * @brief Maclaurin series of function ln(sin(x)) - ln(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class ln_sinx_minus_ln_x : public series_base<T, K>
 {
 public:
	 ln_sinx_minus_ln_x() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 ln_sinx_minus_ln_x(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 ln_sinx_minus_ln_x<T, K>::ln_sinx_minus_ln_x(T x) : series_base<T, K>(x, std::log(std::sin(x) / x)) {}
 
 template <typename T, typename K>
 constexpr T ln_sinx_minus_ln_x<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
 
	 return static_cast<T>(std::log(1 - (this->x)*(this->x) / ((n + 1)*(n + 1)*std::numbers::pi*std::numbers::pi)));
 }
 
 
 /**
 * @brief Maclaurin series of function (pi/8)*cos^2(x) - (1/3)*cos(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class pi_8_cosx_square_minus_1_div_3_cosx : public series_base<T, K>
 {
 public:
	 pi_8_cosx_square_minus_1_div_3_cosx() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 pi_8_cosx_square_minus_1_div_3_cosx(T x);
 
	 /**
	 * @brief Computes the nth term of the Maclaurin series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Maclaurin series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 pi_8_cosx_square_minus_1_div_3_cosx<T, K>::pi_8_cosx_square_minus_1_div_3_cosx(T x) : series_base<T, K>(x, static_cast<T>((std::numbers::pi/8)*std::cos(x)*(std::cos(x) - (1/3)))) {}
 
 template <typename T, typename K>
 constexpr T pi_8_cosx_square_minus_1_div_3_cosx<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if(abs(this->x) > std::numbers::pi / 2)
		 throw std::domain_error("The value x must be between -pi/2 and pi/2 inclusive");
	 return static_cast<T>(this->minus_one_raised_to_power_n(n) * std::cos(2*n + 3) * this->x / ((2*n + 1)*(2*n + 3)*(2*n + 5)));
 }
 
 /**
 * @brief Series of function sqrt((1 - sqrt(1 - x)) / x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class sqrt_oneminussqrtoneminusx_div_x : public series_base<T, K>
 {
 public:
	 sqrt_oneminussqrtoneminusx_div_x() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 sqrt_oneminussqrtoneminusx_div_x(T x);
 
	 /**
	 * @brief Computes the nth term of the series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 sqrt_oneminussqrtoneminusx_div_x<T, K>::sqrt_oneminussqrtoneminusx_div_x(T x) : series_base<T, K>(x, std::sqrt((1 - sqrt(1 - x)) / x)) {}
 
 template <typename T, typename K>
 constexpr T sqrt_oneminussqrtoneminusx_div_x<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if(std::abs(this->x) >= 1 or this->x == 0)
		 throw std::domain_error("Modulus of the value x must be less 1 and cannot be equal to 0");
	 return static_cast<T>(this->fact(4*n)*pow(this->x, n) / (pow(2, 4*n)*sqrt(2)*(this->fact(2*n))* (this->fact(2*n + 1))));
 }
 
 
 /**
 * @brief Series of function (1 - sqrt(1 - 4x)) / 2x
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class one_minus_sqrt_1minus4x_div_2x : public series_base<T, K>
 {
 public:
	 one_minus_sqrt_1minus4x_div_2x() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 one_minus_sqrt_1minus4x_div_2x(T x);
 
	 /**
	 * @brief Computes the nth term of the series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 one_minus_sqrt_1minus4x_div_2x<T, K>::one_minus_sqrt_1minus4x_div_2x(T x) : series_base<T, K>(x, (1 - static_cast<T>(sqrt(std::fma(-4, x, 1)))) / (2*x)) {}
 
 template <typename T, typename K>
 constexpr T one_minus_sqrt_1minus4x_div_2x<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (std::abs(this->x) > 0.25 or this->x == 0)
		 throw std::domain_error("Modulus of the value x must be less or equal 1/4 and cannot be equal to 0");
 
	 T xn = static_cast<T>(std::pow(this->x, n));
	 T binom_coef = static_cast<T>(this->binomial_coefficient(static_cast<T>(2*n), n));
	 T n_plus_1 = static_cast<T>(n + 1);
 
	 return (xn * binom_coef) / n_plus_1;
 }
 
 
 /**
 * @brief Taylor series of function arcsin(x) - x (The problematic row, possibly due to the double factorial in the denominator, needs to be fixed.)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class arcsin_x_minus_x_series : public series_base<T, K>
 {
 public:
	 arcsin_x_minus_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 arcsin_x_minus_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 arcsin_x_minus_x_series<T, K>::arcsin_x_minus_x_series(T x) : series_base<T, K>(x, std::asin(x) - x) {}
 
 template <typename T, typename K>
 constexpr T arcsin_x_minus_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (std::abs(this->x) > 1)
		 throw std::domain_error("Modulus of the value x must be less or equal to 1");
	 return static_cast<T>((this->fact(this->fact(2 * n + 1)) * std::pow(this->x, 2 * n + 3)) / (this->fact(this->fact(2 * n + 2)) * (2 * n + 3)));
 }
 
 
 /**
 * @brief Fourier series of system functions pi*x - x^2, 0 < x < pi
 *											x^2 - 3*pi*x + 2*pi^2, pi < x < 2*pi
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series : public series_base<T, K>
 {
 public:
	 pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Fourier series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Fourier series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= 0 or this->x >= 2 * std::numbers::pi)
		 throw std::domain_error("Modulus of the value x must be between 0 and 2*pi not inclusive");
	 return static_cast<T>((8/std::numbers::pi) * (std::sin((2*n + 1) * (this->x)) / ((2*n + 1)*(2*n + 1)*(2*n + 1))));
 }
 
 
 /**
 * @brief Fourier series of system functions sin(x) - 2/pi, 0 <= x <= pi
 *											-sin(x) - 2/pi, pi <= x <= 2*pi
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class abs_sin_x_minus_2_div_pi_series : public series_base<T, K>
 {
 public:
	 abs_sin_x_minus_2_div_pi_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 abs_sin_x_minus_2_div_pi_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Fourier series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Fourier series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 abs_sin_x_minus_2_div_pi_series<T, K>::abs_sin_x_minus_2_div_pi_series(T x) : series_base<T, K>(x, static_cast<T>(std::abs(std::sin(x)) - (2/std::numbers::pi))) {}
 
 template <typename T, typename K>
 constexpr T abs_sin_x_minus_2_div_pi_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(- 4 * (std::cos(2 * (this->x) * (n + 1))) / ((2 * n + 1) * (2 * n + 3) * std::numbers::pi));
 }
 
 
 /**
 * @brief Fourier series of system functions pi - 3*pi/4, -pi < x < 0
 *											pi - x - 3*pi/4, 0 <= x <= pi
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series : public series_base<T, K>
 {
 public:
	 pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Fourier series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Fourier series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= -std::numbers::pi or this->x > std::numbers::pi)
		 throw std::domain_error("The value x must be between -pi not inclusive and pi inclusive");
	 return static_cast<T>((std::cos((n + 1)*(this->x)) * (1 - (1 - 2 * ((n + 1) % 2)))) / ((n + 1)*(n + 1)*std::numbers::pi) + ((1 - 2 * ((n + 1) % 2)) * std::sin((n + 1)*(this->x))) / (n + 1)); // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
 }
 
 
 
 /**
 * @brief Fourier series of system functions f(x) = -3/4, -3 < x <= 0
 *											f(x) = x - 3/4, 0 < x < 3
 *											f(x + 6) = f(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class minus_3_div_4_or_x_minus_3_div_4_series : public series_base<T, K>
 {
 public:
	 minus_3_div_4_or_x_minus_3_div_4_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 minus_3_div_4_or_x_minus_3_div_4_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Fourier series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Fourier series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 minus_3_div_4_or_x_minus_3_div_4_series<T, K>::minus_3_div_4_or_x_minus_3_div_4_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T minus_3_div_4_or_x_minus_3_div_4_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= -3 or this->x >= 3)
		 throw std::domain_error("The value x must be between -3 and 3 not inclusive");
	 return static_cast<T>(- 6 / (std::numbers::pi * std::numbers::pi) * std::cos((2 * n + 1) * std::numbers::pi * (this->x) / 3) / ((2 * n + 1) * (2 * n + 1))
			- 3/(std::numbers::pi) * ((1 - 2 * ((n + 1) % 2)) * std::sin((this->x) * (n + 1) * std::numbers::pi / 3)) / (n + 1));  // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
 }
 
 
 /**
 * @brief Fourier series of function f(x) = 10 - x, 5 < x < 15
 *									f(x + 10) = f(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class ten_minus_x_series : public series_base<T, K>
 {
 public:
	 ten_minus_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 ten_minus_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Fourier series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Fourier series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 ten_minus_x_series<T, K>::ten_minus_x_series(T x) : series_base<T, K>(x, 10 - this->x) {}
 
 template <typename T, typename K>
 constexpr T ten_minus_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= 5 or this->x >= 15)
		 throw std::domain_error("The value x must be between 5 and 15 not inclusive");
	 return static_cast<T>(10 * (1 - 2 * ((n + 1) % 2)) * std:: sin((n + 1) * (this->x) * std::numbers::pi / 5) / ((n + 1) * std::numbers::pi)); // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
 }
 
 
 /**
 * @brief Fourier series of function x, -pi <= x <= pi
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class x_series : public series_base<T, K>
 {
 public:
	 x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Fourier series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Fourier series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 x_series<T, K>::x_series(T x) : series_base<T, K>(x, this->x) {}
 
 template <typename T, typename K>
 constexpr T x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x < -std::numbers::pi or this->x > std::numbers::pi)
		 throw std::domain_error("The value x must be between -pi and pi inclusive");
	 return static_cast<T>(2*std::pow(-1, n) * std::sin(this->x*(n + 1)) / (n + 1));
 }
 
 
 /**
 * @brief Fourier series of system functions -x, -pi < x <= 0
 *											 0,   0 < x < pi
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class minus_x_minus_pi_4_or_minus_pi_4_series : public series_base<T, K>
 {
 public:
	 minus_x_minus_pi_4_or_minus_pi_4_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 minus_x_minus_pi_4_or_minus_pi_4_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Fourier series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Fourier series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::minus_x_minus_pi_4_or_minus_pi_4_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= -std::numbers::pi or this->x >= std::numbers::pi)
		 throw std::domain_error("The value x must be between -pi and pi not inclusive");
	 return static_cast<T>(- 2 * std::cos(2 * n + 1) * (this->x) / (std::numbers::pi * (2 * n + 1) * (2 * n + 1))
			+ (1 - 2 * ((n + 1) % 2)) * std::sin(this->x*(n + 1)) / (n + 1)); // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
 }
 
 
 /**
 * @brief Laurent series of function 1 / ((2 - x)*(3 + x))
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class one_div_two_minus_x_multi_three_plus_x_series : public series_base<T, K>
 {
 public:
	 one_div_two_minus_x_multi_three_plus_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 one_div_two_minus_x_multi_three_plus_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Laurent series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Laurent series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 one_div_two_minus_x_multi_three_plus_x_series<T, K>::one_div_two_minus_x_multi_three_plus_x_series(T x) : series_base<T, K>(x, 1/((2 - this->x)*(3 + this->x))) {}
 
 template <typename T, typename K>
 constexpr T one_div_two_minus_x_multi_three_plus_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>((std::pow(-3, n) - std::pow(2, n)) / (5*std::pow(this->x, n + 1)));
 }
 
 
 /**
 * @brief Taylor series of function Si(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Si_x_series : public series_base<T, K>
 {
 public:
	 Si_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 Si_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 Si_x_series<T, K>::Si_x_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T Si_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 2*n + 1) / ((2*n + 1) * this->fact(2*n + 1)));
 }
 
 
 /**
 * @brief integral cos serieses
 * @authors Pavlova A.R.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Ci_x_series : public series_base<T, K>
 {
 public:
	 Ci_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Pavlova A.R.
	 * @param x The argument for function series
	 */
	 Ci_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the integral cos series
	 * @authors Pavlova A.R.
	 * @param n The number of the term
	 * @param t The parametr for series
	 * @return nth term of the integral cos series
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 
 private:
	 const T gamma = static_cast<T>(0.57721566490153286060); // Константа Эйлера-Маскерони
 };
 
 template <typename T, typename K>
 Ci_x_series<T, K>::Ci_x_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T Ci_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("The term index must be a non-negative integer");
 
	 if (n == 0)
		 return gamma + std::log(this->x);
 
	 return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 2 * n) / (2 * n * this->fact(2 * n)));
 }
 
 
 
 /**
 * @brief Dirichlet series of Riemann zeta function
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Riemann_zeta_func_series : public series_base<T, K>
 {
 public:
	 Riemann_zeta_func_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 Riemann_zeta_func_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Dirichlet series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Dirichlet series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 Riemann_zeta_func_series<T, K>::Riemann_zeta_func_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T Riemann_zeta_func_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= 1)
		 throw std::domain_error("The value x must be greater than 1");
	 return static_cast<T>(1 / std::pow(n + 1, this->x));
 }
 
 
 
 
 /**
 * @brief Dirichlet series of Riemann zeta function of x-1 divided by Riemann zeta function of x (R(x-1) / R(x))
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series : public series_base<T, K>
 {
 public:
	 Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Dirichlet series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Dirichlet series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x <= 2)
		 throw std::domain_error("The value x must be greater than 1");
	 return static_cast<T>(this->phi(n + 1) / std::pow(n + 1, this->x));
 }
 
 
 /**
 * @brief Taylor series of function ((x^2 + 3) / (x^2 + 2*x)) - 1
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class xsquareplus3_div_xsquareplus2multix_minus_1_series : public series_base<T, K>
 {
 public:
	 xsquareplus3_div_xsquareplus2multix_minus_1_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 xsquareplus3_div_xsquareplus2multix_minus_1_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::xsquareplus3_div_xsquareplus2multix_minus_1_series(T x) : series_base<T, K>(x, (x*x + 3) / (x * (x + 2)) - 1) {}
 
 template <typename T, typename K>
 constexpr T xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (std::abs(this->x - 1) <= 3)
		 throw std::domain_error("The absolute value of x - 1 must be greater than 3");
	 return static_cast<T>(std::pow(-1, n)*(1.5 - 3.5*std::pow(3, n)) / std::pow(this->x - 1, n + 1));
 }
 
 
 /**
 * @brief Taylor series of function arcsin(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class arcsin_x_series : public series_base<T, K>
 {
 public:
	 arcsin_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 arcsin_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 arcsin_x_series<T, K>::arcsin_x_series(T x) : series_base<T, K>(x, std::asin(x)) {}
 
 template <typename T, typename K>
 constexpr T arcsin_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x < -1 or this->x > 1)
		 throw std::domain_error("The value x must be between -1 and 1 inclusive");
	 return static_cast<T>(this->fact(2*n)*std::pow(this->x, 2*n + 1) / (std::pow(4, n) * this->fact(n)*this->fact(n) * (2*n + 1)));
 }
 
 
 /**
 * @brief Taylor series of function arctg(x)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class arctg_x_series : public series_base<T, K>
 {
 public:
	 arctg_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 arctg_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 arctg_x_series<T, K>::arctg_x_series(T x) : series_base<T, K>(x, std::atan(x)) {}
 
 template <typename T, typename K>
 constexpr T arctg_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x < -1 or this->x > 1)
		 throw std::domain_error("The value x must be between -1 and 1 inclusive");
	 return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 2*n + 1) / (2*n + 1));
 }
 
 
 /**
 * @brief Taylor series of Complete elliptic integral of first kind K
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class K_x_series : public series_base<T, K>
 {
 public:
	 K_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 K_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 K_x_series<T, K>::K_x_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T K_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(std::numbers::pi * this->fact(2*n)*this->fact(2*n) * std::pow(this->x, 2*n) / (2*std::pow(16, n) * std::pow(this->fact(n), 4)));
 }
 
 
 /**
 * @brief Taylor series of Complete elliptic integral of second kind E
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class E_x_series : public series_base<T, K>
 {
 public:
	 E_x_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 E_x_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 E_x_series<T, K>::E_x_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T E_x_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(std::numbers::pi * this->fact(2 * n) * this->fact(2 * n) * std::pow(this->x, 2 * n) / (2 * (1 - 2*n) * std::pow(16, n) * std::pow(this->fact(n), 4)));
 }
 
 
 /**
 * @brief Taylor series of function sqrt(1 + x) (тут многое вычисляется неверно, вероятно из-за факториалов)
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class sqrt_1plusx_series : public series_base<T, K>
 {
 public:
	 sqrt_1plusx_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 sqrt_1plusx_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 sqrt_1plusx_series<T, K>::sqrt_1plusx_series(T x) : series_base<T, K>(x, std::sqrt(1 + x)) {}
 
 template <typename T, typename K>
 constexpr T sqrt_1plusx_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (this->x < -1)
		 throw std::domain_error("The value x must be more or equel to -1");
	 return static_cast<T>(std::pow(-1, n)*(this->fact(2*n))*std::pow(this->x, n) / ((1 - 2*n)*(this->fact(n))*(this->fact(n))*std::pow(4, n)));
 }
 
 
 
 /**
 * @brief Taylor series of Lambert W function 
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Lambert_W_func_series : public series_base<T, K>
 {
 public:
	 Lambert_W_func_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 Lambert_W_func_series(T x);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 Lambert_W_func_series<T, K>::Lambert_W_func_series(T x) : series_base<T, K>(x) {}
 
 template <typename T, typename K>
 constexpr T Lambert_W_func_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 if (std::abs(this->x) >= 1 / std::numbers::e)
		 throw std::domain_error("The absolute value of x must be less 1/e");
	 return static_cast<T>(std::pow(-n - 1, n)*std::pow(this->x, n + 1) / this->fact(n+1));
 }
 
 
 /**
 * @brief Taylor series of Incomplete Gamma function
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Incomplete_Gamma_func_series : public series_base<T, K>
 {
 public:
	 Incomplete_Gamma_func_series() = delete;
 
	 /**
	 * @brief Parameterized constructor to initialize the series with function argument and sum
	 * @authors Trudolyubov N.A.
	 * @param x The argument for function series
	 */
	 Incomplete_Gamma_func_series(T x, T s);
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
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
 
 template <typename T, typename K>
 Incomplete_Gamma_func_series<T, K>::Incomplete_Gamma_func_series(T x, T s) : series_base<T, K>(x), s(s) {}
 
 template <typename T, typename K>
 constexpr T Incomplete_Gamma_func_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(std::pow(-1, n) * std::pow(this->x, this->s + n) / (this->fact(n) * (this->s + n)));
 }
 
 
 /**
 * @brief Series with ln number1
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Series_with_ln_number1 : public series_base<T, K>
 {
 public:
	 Series_with_ln_number1();
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 Series_with_ln_number1<T, K>::Series_with_ln_number1() : series_base<T, K>(0) {}
 
 template <typename T, typename K>
 constexpr T Series_with_ln_number1<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(std::log(1 + std::pow(n + 1, (n + 1)*(n + 1) + (n + 1)/2) /
		   (std::pow(this->fact(n + 1), n + 1) * std::pow(std::numbers::e, (n + 1)*(n + 1)))));
 }
 
 
 
 
 /**
 * @brief Series with ln number2
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
 template <typename T, typename K>
 class Series_with_ln_number2 : public series_base<T, K>
 {
 public:
	 Series_with_ln_number2();
 
	 /**
	 * @brief Computes the nth term of the Taylor series of the sine function
	 * @authors Trudolyubov N.A.
	 * @param n The number of the term
	 * @return nth term of the Taylor series of the sine functions
	 */
	 [[nodiscard]] constexpr virtual T operator()(K n) const;
 };
 
 template <typename T, typename K>
 Series_with_ln_number2<T, K>::Series_with_ln_number2() : series_base<T, K>(0) {}
 
 template <typename T, typename K>
 constexpr T Series_with_ln_number2<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 auto tmp = std::log(n + 2);
 
	 return 1 / static_cast<T>((std::pow(tmp, tmp)));
 }
 
 
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
	 return static_cast<T>(std::sqrt(12) * std::pow(-3, -n) / (2*n + 1));
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
		 tempsum += static_cast<T>(static_cast<T>(std::pow(this->x, m)) * static_cast<T>(this->binomial_coefficient(static_cast<T>(std::pow(2, n + 1)), 2*m + 1)));
 
	 return static_cast<T>(std::pow(this->x-1, std::pow(2, n)) / tempsum);
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
 
	 return static_cast<T>(- std::pow(this->x, 2 * n + 1) / (n + 1));
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
 cos3xmin1_div_xsqare_series<T, K>::cos3xmin1_div_xsqare_series(T x) : series_base<T, K>(x, (std::cos(3*x) - 1) / (x*x)) {}
 
 template <typename T, typename K>
 constexpr T cos3xmin1_div_xsqare_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("negative integer in the input");
	 return static_cast<T>(this->minus_one_raised_to_power_n(n + 1) * std::pow(3, 2*n + 2) * std::pow(this->x, 2*n) / this->fact(2*n + 2));
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
 sqrt_1plusx_min_1_min_x_div_2_series<T, K>::sqrt_1plusx_min_1_min_x_div_2_series(T x) : series_base<T, K>(x, std::sqrt(1 + x) - 1 - x/2) {}
 
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
 ln13_min_ln7_div_7_series<T, K>::ln13_min_ln7_div_7_series() : series_base<T, K>(0, (static_cast<T>(std::log(13/7) / 7))) {}
 
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
	 return this->minus_one_raised_to_power_n(n) * std::pow(this->x / 2, 2*n + this->a) / (this->fact(n) * std::tgamma(n + this->a + 1));
 }
 
 
 /**
 /**
 * @brief Taylor series of function 1/sqrt(2) * sin(x/sqrt(2)) (вычисляется плохо во всех методах)
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
 
	 return static_cast<T>(this->minus_one_raised_to_power_n(n+2) * std::pow(this->x, n+1) / ((n+1) * std::pow(1 + (this->x * this->x), n + 1)));
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
 
	 // Пример функции для коэффициента a_k
	 T a_k(K n) const;
 };
 
 template <typename T, typename K>
 gamma_series<T, K>::gamma_series(T t, T x) : series_base<T, K>(x), t(t), x(x) {}
 
 template <typename T, typename K>
 T gamma_series<T, K>::a_k(K n) const
 {
	 // Определите последовательность a_k, например:
	 return static_cast<T>(n + 1);  // В качестве примера пусть a_k = n + 1
 }
 
 template <typename T, typename K>
 constexpr T gamma_series<T, K>::operator()(K n) const
 {
	 if (n < 0)
		 throw std::domain_error("The term index must be a non-negative integer");
 
	 T a_k_n = a_k(n);  // Вычисляем a_k
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
	 this->series_vector.reserve(n);
 
	 for (K i = old_size; i <= n; ++i) {
		 this->series_vector.push_back(this->series_vector[i - 1] * (this->x * this->x) / (i * (4 * i + 2)));
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
 