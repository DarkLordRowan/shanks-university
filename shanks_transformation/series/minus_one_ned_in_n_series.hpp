
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
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_ned_in_n_series<T, K>::operator()(K n) const
{
    return static_cast<T>(n ? this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / static_cast<T>(std::pow(n, n)) : 0); // (37.2) [Rows.pdf]
}
