/**
* @file levin_sidi_S_algorithm.hpp
* @brief Contains implementation of Drummond's D-transformation
* @authors Naumov A.
*/

#pragma once

#include "remainders.hpp"            // Include remainders
#include "series_acceleration.hpp"   // Include series_acceleration
#include <memory>                    // Include for unique ptr

/**
 * @brief S_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 * @param beta           - nonzero positive parameter, 1 is the standart value, for more information see p. 39 in [https://arxiv.org/pdf/math/0306302.pdf]
 * @param remainder_func - remainder to use in calculations
 * @param useRecFormulas - use reccurence relation, the calculation will be done using arrays
 * @param variant        - type of remainder to use
*/
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_sidi_S_algorithm : public series_acceleration<T, K, series_templ> {
protected:

    T beta;
    std::unique_ptr<const transform_base<T, K>> remainder_func;
    bool useRecFormulas = false;
    remainder_type variant = remainder_type::u_variant;

    /**
	* @brief Function to calculate S-tranformation directly by formula. For more information see p. 57 8.2-7 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformationA
	* @return The partial sum after the transformation.
	*/
    inline T calc_result(K n, K order) const;

    /**
	* @brief Function to calculate S-tranformation using reccurence relation. For more information see p. 57 8.3-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/
    inline T calc_result_rec(K n, K order) const;

public:

    /**
	* @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
	* @param series The series class object to be accelerated
	* @param variant Type of remainder to use
	* @param useRecFormulas use reccurence or straightforward formula 
    * @param parameter value of beta parameter
	*/

    explicit levin_sidi_S_algorithm(
        const series_templ& series, 
        const remainder_type variant = remainder_type::u_variant, 
        bool useRecFormulas = false,  
        T parameter = T(1)
    );

    /**
    * @brief S-transformation.
    * Computes the partial sum after the S-transformation
    * @param n The partial sum number (S_n) from which the calculations will be done
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator()(const K n, const K order) const override;

};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_S_algorithm<T, K,series_templ>::calc_result(K n, K order) const{

    using std::isfinite;

    T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
    T rest;
    T up_pochamer, down_pochamer; 
    for (K j = 0; j <= order; ++j){
        rest  = this->series->minus_one_raised_to_power_n(j);
        rest *= this->series->binomial_coefficient(order, j);

        up_pochamer = down_pochamer = static_cast<T>(1);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)
        for (K i = 0; i < order - 1; ++i){
            up_pochamer   *= (beta + static_cast<T>(n + j     + i));
            down_pochamer *= (beta + static_cast<T>(n + order + i));
        }

        rest *= (up_pochamer / down_pochamer);
        rest *= remainder_func->operator()(n, 
            j, 
            this->series,
             (variant == remainder_type::u_variant ? beta : T(1))
            );

        numerator   += rest * this->series->S_n(n + j);
        denominator += rest;
    }

    numerator /= denominator;
    
    if (!isfinite(numerator)) throw std::overflow_error("division by zero");
    return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_S_algorithm<T, K,series_templ>::calc_result_rec(K n, K order) const{

    using std::isfinite;

    std::vector<T>   Num(order + 1, static_cast<T>(0));
    std::vector<T> Denom(order + 1, static_cast<T>(0));

    for (K i = 0; i <= order; ++i){

        Denom[i] = remainder_func->operator()(
            n, 
            i, 
            this->series,
            (variant == remainder_type::u_variant ? beta : T(1))
        );

        Num[i] = this->series->S_n(n + i); Num[i] *= Denom[i];
    }
    
    T scale1, scale2;
    for (K i = 1; i <= order; ++i)
        for(K j = 0; j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula
            scale1 = beta + static_cast<T>(i + j);
            scale1*= (scale1 - T(1));
            scale2 = scale1 + static_cast<T>(i);
            scale2*= (scale2 - T(1));

            Denom[j] = fma(-scale1,Denom[j]/scale2,Denom[j+1]);
              Num[j] = fma(-scale1,  Num[j]/scale2,  Num[j+1]);
        }
    
    Num[0] /= Denom[0];
    
    if (!isfinite(Num[0])) throw std::overflow_error("division by zero");
    return Num[0];
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_sidi_S_algorithm<T, K, series_templ>::levin_sidi_S_algorithm(
    const series_templ& series, 
    remainder_type variant, 
    bool useRecFormulas,  
    T parameter
    ) : 
    series_acceleration<T, K, series_templ>(series),
    useRecFormulas(useRecFormulas)
{
    // parameter is "beta" parameter
    // beta must be nonzero positive real number
    // beta = 1 is default
    //check parameter else default
    if (parameter > static_cast<T>(0)) this->beta = parameter;
    else {
        this->beta = static_cast<T>(1);
        //TODO: наверное сообщение по типу usage
    }

    //check variant else default 'u'
    //TODO: тоже самое наверное
    switch(variant){
        case remainder_type::u_variant :
            remainder_func.reset(new u_transform<T, K>());
            break;
        case remainder_type::t_variant :
            remainder_func.reset(new t_transform<T, K>());
            break;
        case remainder_type::v_variant :
            remainder_func.reset(new v_transform<T, K>());
            break;
        case remainder_type::t_wave_variant:
            remainder_func.reset(new t_wave_transform<T, K>());
            break;
        case remainder_type::v_wave_variant:
            remainder_func.reset(new v_wave_transform<T, K>());
            break;
        default:
            remainder_func.reset(new u_transform<T, K>());
    }
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_sidi_S_algorithm<T, K, series_templ>::operator()(const K n, const K order) const{ 

    using std::isfinite;

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}