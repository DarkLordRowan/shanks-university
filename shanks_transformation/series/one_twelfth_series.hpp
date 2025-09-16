
/**
* @brief Numerical series representation of 1 * x/12
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_twelfth_series : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    one_twelfth_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of 1 * x/ 12
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_twelfth_series<T, K>::one_twelfth_series(T x) : series_base<T, K>(x, x / static_cast<T>(12))
{
    this->series_name = "x/12";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_twelfth_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(std::fma(2, n, 1));
    return this->x / static_cast<T>((a * (a + static_cast<K>(2)) * (a + static_cast<K>(4)))); // (27.2) [Rows.pdf]
}
