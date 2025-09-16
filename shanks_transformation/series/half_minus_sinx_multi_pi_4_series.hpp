
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
