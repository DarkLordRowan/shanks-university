
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
