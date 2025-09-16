
/**
* @brief Numerical series representation of (-3 + (3/2)*ln3 + 2*ln2) * x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_three_plus_ln3_three_devided_two_plus_two_ln2_series : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    minus_three_plus_ln3_three_devided_two_plus_two_ln2_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the cosine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the cosine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series(T x) : series_base<T, K>(x, x * (static_cast<T>(-3)  + (static_cast<T>(3) / static_cast<T>(2)) * static_cast<T>(std::log(3)) +  static_cast<T>(std::log(4))))
{
    this->series_name = "(-3 + 3/2*ln(3) + 2*ln(2))*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::operator()(K n) const
{
    T n_temp = static_cast<T>(n + 1);
    return this->x / (n_temp * static_cast<T>(std::fma(36, n_temp * n_temp, -1)));  // (43.2) [Rows.pdf]
}
