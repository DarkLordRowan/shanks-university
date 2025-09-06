//header with all remainders
#pragma once

#include <concepts>
#include "../series.h"

/**
  * @brief Enum for remainders types to use in Levin-type transformations
  * @authors Bolshakov M.P.
  * @edited by Kreynin R.G.
*/
enum remainder_type{
         u_variant,
         t_variant,
         v_variant,
    t_wave_variant,
    v_wave_variant,
};

/**
* @brief Abstract class for remainders
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class transform_base{
public:

    /**
    * @brief Virtual operator() function for computing remainder
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale The value to multiple (needed for u variant of S algo for now)
    * @return remainder of necessary type
    */
    virtual T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const = 0;
};

/**
* @brief Class for u variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class u_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing u type remainders, such as, w_n = n*a_n (for S algo with beta: w_n = (beta + n) * a_n)
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale The value to multiple (needed for u variant of S algo for now)
    * @return u type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T u_transform<T, K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {

    using std::isfinite;

    const T result = static_cast<T>(1) / (scale * series->operator()(n+order));

    if (!isfinite(result)) throw std::overflow_error("division by zero");

    return result;
 }

/**
* @brief Class for t variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class t_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing t type remainders, such as, w_n = a_n
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return t type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T t_transform<T, K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {

    using std::isfinite;

    const T result = static_cast<T>(1) / series->operator()(n+order);
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

/**
* @brief Class for t-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class t_wave_transform : public transform_base<T, K>  {
    /**
    * @brief Operator() function for computing t-wave type remainders, such as, w_n = a_(n+1)
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return t-wave type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T t_wave_transform<T,K>::operator()(const K n, const K order, const series_base<T, K>* series, T scale ) const {

    using std::isfinite;

	const T result = static_cast<T>(1) / series->operator()(n + order + 1);

	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}

/**
* @brief Class for v variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class v_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing v type remainders, such as, w_n = a_n*a_(n+1)/(a_(n+1)-a_n)
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return v type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = T(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T v_transform<T,K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {

    using std::isfinite;

    const T a1 = series->operator()(n+order), a2  = series->operator()(n+order+1);
    const T result = (a2-a1) / (a1 * a2);
	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}

/**
* @brief Class for v-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class v_wave_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing v-wave type remainders, such as, w_n = a_n*a_(n+1)/(a_n-a_(n+1))
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return v-wave type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T v_wave_transform<T,K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const  {

    using std::isfinite;
    
    const T a1 = series->operator()(n+order), a2 = series->operator()(n+order+1);
    const T result = (a1 - a2) / (a1 * a2);
	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}