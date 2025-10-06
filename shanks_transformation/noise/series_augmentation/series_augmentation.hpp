//
// Created by user on 06.10.2025.
//
#pragma once

#include "custom_concepts.hpp"
#include "noise/noise_generator.hpp"


#ifndef SHANKS_TRANSFORMATION_SERIES_AUGMENTATION_HPP
#define SHANKS_TRANSFORMATION_SERIES_AUGMENTATION_HPP

template<AcceptedLike T>
SeriesResult<T> scaling(const SeriesResult<T>& source, T bottom_border = static_cast<T>(-2.0), T top_border = static_cast<T>(2.0), noise_type type = uniform) {

    const size_t n = source.Sn.size();

    SeriesResult<T> result;
    result.Sn = source.Sn;
    result.an = source.an;

    if (n == 0) {
        return result;
    }

    Noise<T> noise(n, bottom_border, top_border, type);

    result.an[0] *= noise.an[0];
    result.Sn[0] *= noise.Sn[0];

    for (size_t j = 1; j < n; ++j) {
        result.an[j] *= noise.an[j];
        result.Sn[j] = result.Sn[j-1] + result.an[j];
    }

    return result;
}


// не уверен вообще как вращать, да еще и не будет ли это бесполезно для ускорений (плюс надо подумать как на комлексных делать)
//
// template<AcceptedLike T>
// SeriesResult<T> rotate(const SeriesResult<T>& source, T angle = static_cast<T>(3.14159265358979323846) / static_cast<T>(4.0)) {
//
//     const size_t n = source.Sn.size();
//
//     SeriesResult<T> result;
//     result.Sn = source.Sn;
//     result.an = source.an;
//
//     if (n == 0) {
//         return result;
//     }
//
//     T cos_angle = cos(angle);
//     T sin_angle = sin(angle);
//
//     result.Sn[0] = source.Sn[0] * cos_angle - source.Sn[0] * sin_angle;
//     result.an[0] = result.Sn[0];
//
//     for (size_t j = 1; j < n; ++j) {
//         result.Sn[j] = source.Sn[j] * cos_angle - source.Sn[j] * sin_angle;
//         result.an[j] = result.Sn[j] - result.Sn[j - 1];
//     }
//
//     return result;
// }



#endif //SHANKS_TRANSFORMATION_SERIES_AUGMENTATION_HPP