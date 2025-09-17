#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function exp(x^2)*erf(x), where erf(x) is error function of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class exp_squared_erf_series final : public series_base<T, K>
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

    constexpr inline T calculate_sum(T x) const {
		if(!isfinite(x)){ return static_cast<T>(0);}

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){
			return exp(x * x) * erf(x);
		}

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return static_cast<T>(0);
		}
	}
};

template <Accepted T, std::unsigned_integral K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : 
series_base<T, K>(
    x, 
    calculate_sum(x)
)
{
    this->series_name = "exp(x²)*erf(x)";
    // Сходится при ∀x ∈ ℝ (ряд для erf(x) сходится на всей числовой прямой)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T exp_squared_erf_series<T, K>::operator()(K n) const
{
    const T result = pow(this->x, static_cast<T>(fma(2, n, 1))) / static_cast<T>(tgamma(n + 1.5)); // (10.3) [Rows.pdf]
    if (!isfinite(result))
        throw std::overflow_error("operator() is too big");
    return result;
}
