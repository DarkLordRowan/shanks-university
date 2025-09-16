
/**
* @brief Maclaurin series of function arctan(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arctan_x2_series : public series_base<T, K>
{
public:
    arctan_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arctan_x2_series(T x);

    /**
    * @brief Computes the nth term of the arctan_x2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the arctan_x2 series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
arctan_x2_series<T, K>::arctan_x2_series(T x) : series_base<T, K>(x, std::atan(x* x))
{
    this->series_name = "arctan(x²)";
    // Сходится при |x| ≤ 1 (ряд для arctan(z) сходится при |z| ≤ 1, где z = x²)
    // Расходится при |x| > 1

    if (std::abs(x) > 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arctan_x2_series<T, K>::operator()(K n) const
{
    const K a = 2 * n + 1;
    return series_base<T, K>::minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(this->x, static_cast <K>(2) * a)) / static_cast<T>(a); // (80.2) [Rows.pdf]
}
