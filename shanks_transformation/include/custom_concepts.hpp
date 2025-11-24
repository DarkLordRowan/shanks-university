#ifndef CUSTOM_CONCEPTS_HPP
#define CUSTOM_CONCEPTS_HPP
#pragma once

#ifndef INC_FPRECISION
    #include "../libs/arbitrary_arithmetics/fprecision.h"
#endif

#ifndef INC_COMPLEXPRECISION
    #include "../libs/arbitrary_arithmetics/complexprecision.h"
#endif

#include <type_traits>

template<typename T>
concept FloatLike =
    #ifdef INC_FPRECISION
        std::is_same<T, float_precision>::value ||
    #endif
    std::is_floating_point<T>::value 
    ;

template<typename T>
struct is_standart_types : std::integral_constant<bool,
    std::is_floating_point<T>::value 
    #ifdef INC_COMPLEXPRECISION
    || 
    std::is_same<T, complex_precision<float>>::value  ||
    std::is_same<T, complex_precision<double>>::value ||
    std::is_same<T, complex_precision<long double>>::value
    #endif
>{};

template<typename T>
struct is_precisable : std::integral_constant<bool,
    false
    #ifdef INC_FPRECISION
    || std::is_same<T, float_precision>::value
    #ifdef INC_COMPLEXPRECISION
    || std::is_same<T, complex_precision<float_precision>>::value
    #endif
    #endif
>{};


#ifdef INC_COMPLEXPRECISION
template<typename T>
struct isComplexLike : std::integral_constant<bool,
        std::is_same<T, complex_precision<float>>::value  ||
        std::is_same<T, complex_precision<double>>::value ||
        std::is_same<T, complex_precision<long double>>::value ||
        #ifdef INC_FPRECISION
            std::is_same<T, complex_precision<float_precision>>::value>
        #endif
        {};

template<typename T>
concept ComplexLike =
    #ifdef INC_FPRECISION
        std::is_same<T, complex_precision<float_precision>>::value ||
    #endif
    std::is_same<T, complex_precision<float>>::value  ||
    std::is_same<T, complex_precision<double>>::value ||
    std::is_same<T, complex_precision<long double>>::value;
#endif


template<typename T>
concept AcceptedLike = requires{ 
    requires FloatLike<T> || ComplexLike<T>; 
};

template<typename T>
struct isFloatLike : std::integral_constant<bool, std::is_floating_point<T>::value || std::is_same<T, float_precision>::value >{};

template<typename K>
concept UnsignedIntLike = requires {
    std::is_integral<K>::value && !std::is_signed<K>::value;
};

template<typename K>
struct isUnsignedIntLike : std::integral_constant<bool, std::is_integral<K>::value && !std::is_signed<K>::value>{};

#endif