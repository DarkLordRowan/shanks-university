#pragma once

#ifndef INC_FPRECISION
    #include "libs/arbitrary_arithmetics/fprecision.h"
#endif

#ifndef INC_COMPLEXPRECISION
    #include "libs/arbitrary_arithmetics/complexprecision.h"
#endif

template<typename T>
concept FloatLike = requires{ std::is_floating_point<T>::value || std::is_same<T, float_precision>::value; };

template<typename T>
concept ComplexLike = requires{
    //std::is_same<T, std::complex<float>>::value  ||
    //std::is_same<T, std::complex<double>>::value ||
    //std::is_same<T, std::complex<long double>>::value ||
    std::is_same<T, complex_precision<float>>::value  ||
    std::is_same<T, complex_precision<double>>::value ||
    std::is_same<T, complex_precision<long double>>::value ||
    std::is_same<T, complex_precision<float_precision>>::value;
};

template<typename T>
concept AcceptedLike = requires{ 
    requires FloatLike<T> || ComplexLike<T>; 
};

template<typename K>
concept UnsignedIntLike = requires {
    std::is_integral<K>::value && !std::is_signed<K>::value || std::is_same<K, int_precision>::value;
};

template<AcceptedLike T>
T convertWithPrec(float realPart, size_t precision) {

    if constexpr(std::is_same<T, float_precision>::value){
        return float_precision(realPart, precision);
    } else if constexpr(std::is_same<T, complex_precision<float_precision>>::value){
        
        return complex_precision<float_precision>(
            float_precision(realPart, precision), 
            float_precision(0, precision)
        );
    } else {
        return static_cast<T>(realPart);
    }
}

template<AcceptedLike T, UnsignedIntLike K>
T minus_one_raised_to_power_n(K j){
    if constexpr (std::is_same<K, int_precision>::value){
        return static_cast<T>(j.even() ? -1 : 1);
    } else {
        return static_cast<T>(j & 1 ? -1 : 1);
    }
}