
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
    if (n == 0)
        return static_cast<T>(0);

    const T pi_n = static_cast<T>(std::numbers::pi) * static_cast<T>(n);
    const T a_n = this->minus_one_raised_to_power_n(n) * static_cast<T>(10) / pi_n;
    return a_n * std::sin(pi_n * this->x * static_cast<T>(0.2)); // (60.4) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}
