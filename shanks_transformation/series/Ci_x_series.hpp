
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
    const T gamma = static_cast<T>(std::numbers::egamma); // the Euler–Mascheroni constant
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

    const T two_n = static_cast<T>(2 * n);
    return this->minus_one_raised_to_power_n(n) * std::pow(this->x, two_n) / two_n / static_cast<T>(this->fact(2 * n)); // (65.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}
