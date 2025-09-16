
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

    if (std::abs(x) > static_cast <T>(1) || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sqrt_1plusx_min_1_min_x_div_2_series<T, K>::operator()(K n) const
{
    return static_cast<T>(this->binomial_coefficient(static_cast<T>(0.5), n + 2) * std::pow(this->x, n + 2)); // (93.2) [Rows.pdf]
}
