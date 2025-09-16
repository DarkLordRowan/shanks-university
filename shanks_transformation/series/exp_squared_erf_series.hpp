
/**
* @brief Maclaurin series of function exp(x^2)*erf(x), where erf(x) is error function of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class exp_squared_erf_series : public series_base<T, K>
{
public:
    exp_squared_erf_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    exp_squared_erf_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of exp(x^2)*erf(x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, static_cast<T>(std::exp(x * x)) * static_cast<T>(std::erf(x)))
{
    this->series_name = "exp(x²)*erf(x)";
    // Сходится при ∀x ∈ ℝ (ряд для erf(x) сходится на всей числовой прямой)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_squared_erf_series<T, K>::operator()(K n) const
{
    const T result = static_cast<T>(std::pow(this->x, std::fma(2, n, 1))) / static_cast<T>(std::tgamma(n + 1.5)); // (10.3) [Rows.pdf]
    if (!isfinite(result))
        throw std::overflow_error("operator() is too big");
    return result;
}
