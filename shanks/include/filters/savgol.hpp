#ifndef SAVGOL_HPP
#define SAVGOL_HPP
#pragma once

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <stdexcept>
#include "../custom_concepts.hpp"
#include "../utils.hpp"


namespace shanks{
namespace filters{

/**
// * @file savgol.hpp
// * @brief Savitzky-Golay filter implementation.
// * Code for calculating coefficients is taken from https://izadori.net/en/math-savitzky-golay-en/
// * the code implementation gives similar values as scipy.savgol_filter with mode = constant and deriv = 0.
// * main theory is given on wiki page https://en.wikipedia.org/wiki/Savitzky–Golay_filter
*/
template<AcceptedLike Scalar>
std::vector<Scalar> savgol_filter(
    const std::vector<Scalar>& data, 
    size_t window_length,
    size_t polyorder, //order of approximation polynomial
    size_t derive, // order of differentiation
    const Scalar& delta //spacing between points
){

    if (window_length > data.size()) throw std::invalid_argument("window's length is bigger than data's size");
    if (polyorder >= window_length) throw std::invalid_argument("polyorder is bigger or equal than window's length");

    //taken code
    const int N = static_cast<int>(window_length) / 2;
    Eigen::Vector<Scalar, Eigen::Dynamic> v = Eigen::Vector<Scalar, Eigen::Dynamic>::LinSpaced(window_length, -N, N);
    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> x = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Ones(window_length, polyorder + 1);

    for (size_t i{1}; i <= polyorder; ++i)
        x.col(i) = (x.col(i-1).array() * v.array()).matrix();

    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> coeff_mat = (x.transpose() * x).inverse() * x.transpose();

    Eigen::RowVector<Scalar, Eigen::Dynamic> coeffs = utils::cast<Scalar>(utils::fact(derive)) * coeff_mat.row(derive) / utils::pow(delta, utils::cast<Scalar>(derive));

    //convolution with padding: adding 0 at the start, rest 0 on the end
    std::vector<Scalar> padded_vector(data.size() + (window_length - 1) * 2, Scalar(0.0));
    std::vector<Scalar> result(data.size(), Scalar(0.0));
    std::copy(data.begin(), data.end(), padded_vector.begin()+1);

    for (size_t i{0}; i < result.size(); ++i)
        for(size_t j{0}; j < window_length; ++j)
            result[i] += coeffs[j] * padded_vector[i + j];

    return result;
}

} //namespace shanks::filters
} //namespace shanks

#endif