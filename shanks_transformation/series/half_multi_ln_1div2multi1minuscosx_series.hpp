
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
        this->throw_domain_error("x must be in (0, 2π) and cos(x) ≠ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_multi_ln_1div2multi1minuscosx_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(n + 1);
    return static_cast<T>(std::cos(a * (this->x))) / a; // (47.5) [Rows.pdf]
}
