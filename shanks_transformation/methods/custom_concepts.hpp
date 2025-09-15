#pragma once

#ifndef INC_FPRECISION
    #include "../libs/arbitrary_arithmetics/fprecision.h"
#endif

template<typename T>
concept FloatLike = (std::is_floating_point<T>::value || std::is_same<T, float_precision>::value) ;