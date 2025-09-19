#pragma once

#ifndef INC_FPRECISION
    #include "libs/arbitrary_arithmetics/fprecision.h"
#endif

#ifndef INC_COMPLEXPRECISION
    #include "libs/arbitrary_arithmetics/complexprecision.h"
#endif

template<typename T>
concept FloatLike = std::is_floating_point<T>::value || std::is_same<T, float_precision>::value;

template<typename T>
concept ComplexLike = 
    std::is_same<T, complex_precision<float>>::value  ||
    std::is_same<T, complex_precision<double>>::value ||
    std::is_same<T, complex_precision<long double>>::value ||
    std::is_same<T, complex_precision<float_precision>>::value;
;

template<typename T>
concept Accepted = FloatLike<T> || ComplexLike<T>;
