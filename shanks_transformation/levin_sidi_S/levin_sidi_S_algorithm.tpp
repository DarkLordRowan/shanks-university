#pragma once

#include "levin_sidi_S_algorithm.hpp"

template<typename T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_S_algorithm<T, K,series_templ>::calc_result(K n, K order) const{
    // продолжение
    T result;
    T numerator(0), denominator(0);
    T rest;
    T up_pochamer;  
    T down_pochamer; 
    for (K j = 0; j <= order; ++j){
        rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(T(order),j);
        up_pochamer = down_pochamer = T(1);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)
        for (K i = 0; i < order - 1; ++i){
            up_pochamer   *= (beta + T(n + j + i));
            down_pochamer *= (beta + T(n + order + i));
        }
        rest *= (up_pochamer / down_pochamer) * remainder_func->operator()(n, 
            j, 
            this->series,
             (variant == 'u' ? beta : T(1))
            );
        numerator   += rest * this->series->S_n(n + j);
        denominator += rest;
    }
    result = numerator/denominator;
    
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

template<typename T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_S_algorithm<T, K,series_templ>::calc_result_rec(K n, K order) const{
    //  пример:
    //  S^(20)_3
    //      |    \     
    //  S^(20)_2    S^(21)_2
    //      |    \      |     \  
    //  S^(20)_1    S^(21)_1    S^(22)_1
    //      |    \      |     \    |      \         
    //  S^(20)_0    S^(21)_0    S^(22)_0    S^(23)_0 
    //
    std::vector<T>   Num(order + 1, T(0));
    std::vector<T> Denom(order + 1, T(0));
    for (K i = 0; i <= order; ++i){
        Denom[i] = remainder_func->operator()(
            n, 
            i, 
            this->series,
            (variant == 'u' ? beta : T(1))
        );
          Num[i] = this->series->S_n(n + i) * Denom[i];
    }
    
    T scale1, scale2;
    for (K i = 1; i <= order; ++i)
        for(K j = 0; j <= order - i; ++j){
            // i ~ k from formula
            // j ~ n from formula
            scale1 = beta + T(i + j);
            scale1*= (scale1 - T(1));
            scale2 = scale1 + T(i);
            scale2*= (scale2 - T(1));
            Denom[j] = fma(-scale1,Denom[j]/scale2,Denom[j+1]);
              Num[j] = fma(-scale1,  Num[j]/scale2,  Num[j+1]);
        }
    
    T result = Num[0] / Denom[0];
    
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

template<typename T, std::unsigned_integral K, typename series_templ>
levin_sidi_S_algorithm<T, K, series_templ>::levin_sidi_S_algorithm(const series_templ& series, char variant, bool useRecFormulas,  T parameter ) : 
    series_acceleration<T, K, series_templ>(series),
    useRecFormulas(useRecFormulas)
    {
        // parameter is "beta" parameter
        // beta must be nonzero positive real number
        // beta = 1 is default
        //check parameter else default
        //if (parameter > T(0)) this->beta = parameter;
        //else {
            this->beta = T(1);
            //TODO: наверное сообщение по типу usage
        //}

        //check variant else default 'u'
        //TODO: тоже самое наверное
        switch(variant){
            case 't':
                remainder_func = new t_transform<T, K>();
                break;
            case 'v':
                remainder_func = new v_transform<T, K>();
                break;
            case 'd':
                remainder_func = new d_transform<T, K>();
                break;
            case 'w':
                remainder_func = new v_transform_2<T, K>();
                break;
            default:
                remainder_func = new u_transform<T, K>();
                break;
        }
    }
template<typename T, std::unsigned_integral K, typename series_templ>
T levin_sidi_S_algorithm<T, K, series_templ>::operator()(const K n, const K order) const{ 
    T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}