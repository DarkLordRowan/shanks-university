#ifndef UTILS_MATH_FWD_HPP
#define UTILS_MATH_FWD_HPP
#pragma once

template<typename T>
struct utils::helpers{

    static T nextafter(const T& x, const T& y);
    static T fmod(const T& x, const T& y);
    static T floor(const T& x);
    static T ceil(const T& x);
    static T get_nan(const T& x);

    /**
     * @brief Get the precision of a variable
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return size_t precision
     */
    static size_t get_precision(const T& x);

    // HELPER
    /**
     * @brief Similar functionality of std::to_string; wrapper around it
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return std::string representation
     */
    static std::string to_string(const T& x);

    /**
     * @brief Wrapper around std::isfinite, has similar functionality
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return bool (is finite)
     */
    static bool isfinite(const T& x);

    static bool isnan(const T& x);

    /**
     * @brief Wrapper around std::isfinite, has similar functionality
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return bool (is finite)
     */
    static bool isinf(const T& x);

    /**
     * @brief Wrapper around std::numeric_limits<T>::epsilon
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (T)
     * @return T (epsilon value)
     */
    static T epsilon(const T& x);

    /**
     * @brief Wrapper around std::numeric_limits<T>::max
     *
     */
    static T numeric_max(size_t precision = size_t{0});
}

static T nextafter(const T& x, const T& y);
static T fmod(const T& x, const T& y);
static T floor(const T& x);
static T ceil(const T& x);
static T get_nan(const T& x){
    static_assert(std::false_type{}, "utils::get_nan not implemented for type ->" + typeid(T).name());
}

static std::size_t{0} get_precision(const T& x){return std::size_t{0};}

static std::string to_string(const T& x);

static bool isfinite(const T& x);
static bool isnan(const T& x);
static bool isinf(const T& x);

static T epsilon(const T& x);
static T numeric_max(size_t precision = size_t{0});

#endif