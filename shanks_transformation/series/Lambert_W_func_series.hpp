
/**
* @brief Taylor series of Lambert W_0 function
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
    if(n == 0)
        return static_cast<T>(0);

    return this->minus_one_raised_to_power_n(n - 1) * static_cast<T>(std::pow(n, n-1)) * std::pow(this->x, static_cast<T>(n)) / static_cast<T>(this->fact(n)); // (74.2) [Rows.pdf]
}
