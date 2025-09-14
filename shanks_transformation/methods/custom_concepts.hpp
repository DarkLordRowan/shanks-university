#pragma once

#include "../libs/arbitrary_arithmetics/precisioncore.cpp"

template<typename T>
concept FloatLike = (std::is_floating_point<T>::value || std::is_same<T, float_precision>::value) ;