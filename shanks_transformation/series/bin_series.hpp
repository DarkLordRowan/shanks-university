
/**
* @brief Binomial series (Maclaurin series for (1+x)^a function)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class bin_series : public series_base<T, K>
{
    using series_base<T, K>::binomial_coefficient;

public:
    bin_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Bolshakov M.P.
    * @param x The argument for function series, alpha The power constant
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    bin_series(T x, T alpha);

    /**
    * @brief Computes the nth term of the Binomial series
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
private:

    /**
    * @brief The power
    * @authors Bolshakov M.P.
    */
    const T alpha;
};

template <std::floating_point T, std::unsigned_integral K>
bin_series<T, K>::bin_series(T x, T alpha) : series_base<T, K>(x, static_cast<T>(std::pow(static_cast<T>(1) + x, alpha))), alpha(alpha)
{
    this->series_name = "(1+x)^α";
    // Сходится при |x| < 1 (абсолютно), при x = -1 зависит от α
    // При α > 0 сходится при x = -1, при α ≤ -1 расходится

    if (std::abs(x) >= 1 || !std::isfinite(x) || !std::isfinite(alpha)) {
        this->throw_domain_error("|x| must be < 1 and parameters must be finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T bin_series<T, K>::operator()(K n) const
{
    return binomial_coefficient(alpha, n) * static_cast<T>(std::pow(this->x, n)); // (6.1) [Rows.pdf]
}
