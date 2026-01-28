#ifndef UTILS_MATH_FWD_HPP
#define UTILS_MATH_FWD_HPP
#pragma once

template <typename T>
struct utils::math {
    // MATH FUNCTIONS
    /**
     * @brief Calculates Euler's totient function
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Euler%27s_totient_function
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex
     * types)
     * @tparam T UnsginedIntLike (std::integral or types similar to it)
     * @param n
     * @return return result of phi(n) of type T
     */
    template <UnsignedIntLike K>
    static K phi(K n);

    /**
     * @brief Calculates factorial of a natural number n!
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @tparam T UnsginedIntLike (std::integral or types similar to it)
     * @param n
     * @return return result of n! of type T
     */
    template <UnsignedIntLike K>
    static K fact(const K n);

    /**
     * @brief Calculates double factorial of a natural number n!!
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @tparam K UnsginedIntLike (std::integral or types similar to it)
     * @param n
     * @return return result of n!! type K
     */
    template <UnsignedIntLike K>
    static K double_fact(const K n);

    /**
     * @brief Calculates binomial coefficient (n, k)
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Binomial_coefficient
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @tparam K  UnsginedIntLike (std::integral or types similar to it)
     * @param n
     * @param k
     * @return return binomial coefficient of type K
     */
    template <UnsignedIntLike K>
    static K binomial_coefficient(const K n, const K k);

    /**
     * @brief Returns (-1)^j
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex
     * types)
     * @tparam K UnsginedIntLike (std::integral or types similar to it)
     * @param j
     * @return return (-1)^j of type T
     */
    template <UnsignedIntLike K>
    static T minus_one_raised_to_power_n(const K j);

    /**
     * @brief Calculates x^y
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex
     * types) or std::integral
     * @param x (T), y (T)
     * @return T (result of power)
     */
    static T pow(const T& x, const T& y);

    /**
     * @brief Calculates atan2(y, x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    static T atan2(const T& y, const T& x);

    /**
     * @brief Calculates a * b + c
     *
     * Uses fma intructions for standard types, or if it is implemented for type T
     * @tparam T AcceptedLike Type (std::floating_point, custom floating_point types, std::complex and custom complex
     * types) or std::integral
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param a (T), b (T), c (T)
     * @return T a * b + c
     */
    static T fma(const T& a, const T& b, const T& c);

    /**
     * @brief Calculates sqrt(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (square root)
     */
    static T sqrt(const T& x);

    /**
     * @brief Calcualtes exp(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (exponent)
     */
    static T exp(const T& x);

    /**
     * @brief Calculates log(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (natural logarithm)
     */
    static T log(const T& x);
    static T log10(const T& x);

    /**
     * @brief Calcualtes sqrt(a^2 + b^2)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param a (T), b (T)
     * @return T (hypotenuse)
     */
    static T hypot(const T& a, const T& b);

    /**
     * @brief Calculates error function erf(x)
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Error_function
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (erf result)
     */
    static T erf(const T& x);

    /**
     * @brief Calculates Riemann zeta function
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Riemann_zeta_function
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (zeta result)
     */
    static T zeta(const T& x);

    /**
     * @brief Calculates trigonometric integral of cos
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Trigonometric_integral
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (Ci(x) result)
     */
    static T ci_x(const T& x);

    /**
     * @brief Calculates trigonometric integral of sin
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Trigonometric_integral
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (Si(x) result)
     */
    static T si_x(const T& x);

    /**
     * @brief Calculates complete elliptical integral of the second kind
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Elliptic_integral
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (E(x) result)
     */
    static T e_x(const T& x);

    /**
     * @brief Calculates complete elliptical integral of the first kind
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Elliptic_integral
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (K(x) result)
     */
    static T k_x(const T& x);

    /**
     * @brief Calculates lower incomplete gamma function
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Incomplete_gamma_function
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T), alpha (T)
     * @return T (gamma result)
     */
    static T inc_gamma(const T& x, const T& alpha);

    /**
     * @brief Calculates Lambert's function pricipal branch (W_{0})
     *
     * For more info see wiki page https://en.wikipedia.org/wiki/Lambert_W_function
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (W0 result)
     */
    static T lambertW0(const T& x);

    /**
     * @brief Calcaulates sin(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (sine result)
     */
    static T sin(const T& x);

    /**
     * @brief Calculates asin(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (arcsine result)
     */
    static T asin(const T& x);

    /**
     * @brief Calculates cos(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (cosine result)
     */
    static T cos(const T& x);

    /**
     * @brief Calculates acos(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (arccosine result)
     */
    static T acos(const T& x);

    /**
     * @brief Calculates tan(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (tangent result)
     */
    static T tan(const T& x);

    /**
     * @brief Calculates atan(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (arctangent result)
     */
    static T atan(const T& x);

    /**
     * @brief Calculates sinh(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (hyperbolic sine result)
     */
    static T sinh(const T& x);

    /**
     * @brief Calculates asinh(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (hyperbolic arcsine result)
     */
    static T asinh(const T& x);

    /**
     * @brief Calculates cosh(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (hyperbolic cosine result)
     */
    static T cosh(const T& x);

    /**
     * @brief Calculates acosh(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (hyperbolic arccosine result)
     */
    static T acosh(const T& x);

    /**
     * @brief Calculates tanh(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (hyperbolic tangent result)
     */
    static T tanh(const T& x);

    /**
     * @brief Calculates atanh(x)
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (hyperbolic arctangent result)
     */
    static T atanh(const T& x);

    /**
     * @brief Calculates absolute value of a variable
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return Underlying type value (absolute value)
     */
    static typename real_of<T>::value abs(const T& x);
}

#endif