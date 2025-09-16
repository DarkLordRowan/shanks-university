
/**
* @brief Taylor series of function arcsin(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arcsin_x_series : public series_base<T, K>
{
public:
    arcsin_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arcsin_x_series(T x);

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
arcsin_x_series<T, K>::arcsin_x_series(T x) : series_base<T, K>(x, std::asin(x))
{
    this->series_name = "arcsin(x)";
    // Сходится при |x| ≤ 1 (биномиальный ряд для arcsin(x))
    // Расходится при |x| > 1

    if (std::abs(x) > 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsin_x_series<T, K>::operator()(K n) const
{
    const T fact = static_cast<T>(this->fact(n));
    const T coeff = static_cast<T>(this->fact(2*n)) / fact / fact / static_cast<T>(std::pow(2, 2*n));
    return coeff * std::pow(this->x, static_cast<T>(fma(2,n,1))) / static_cast<T>(fma(2,n,1)); // (69.1) [Rows.pdf]
}
