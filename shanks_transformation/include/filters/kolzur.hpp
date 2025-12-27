


#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "../custom_concepts.hpp"
#include "../utils.hpp"

//the code is stolen from
//https://github.com/MathieuSchopfer/kolmogorov-zurbenko-filter/blob/master/kolzur_filter.py
//wiki explanaition https://math.stackexchange.com/questions/1349135/kolmogorov-zurbenko-filter-calculation-of-coefficients
//https://math.stackexchange.com/questions/4535682/calculating-coefficients-of-an-n-degree-polynomial-raised-to-an-arbitrary-power
template<typename Scalar>
requires AcceptedLike<Scalar> || std::is_integral<Scalar>::value
std::vector<Scalar> kolzur_filter(
    const std::vector<Scalar>& data,
    size_t window_length /*m*/,
    size_t degree /*k*/
) {
    const size_t m = window_length, k = degree;
    const size_t size = (m-1)*k + 1;
    std::vector<int> coeffs = std::vector<int>(size, 0);
    coeffs[0] = coeffs[size - 1] = 1;
    
    const size_t middle_coeff = (size + size % 2) / size_t{2};
    for(size_t l{1}; l < middle_coeff; ++l){
        for (size_t i{0}; m * i <= l; ++i){
            const size_t j = (l > m * i ? l - m*i : 0);
            coeffs[l] += (1 - 2 * static_cast<int>(i&1)) * utils::binomial_coefficient(k, i) * utils::binomial_coefficient(k + j - 1, j);
            coeffs[size - 1 - l] = coeffs[l];
        }
    }

    std::vector<Scalar> zur_coeffs(size + 1, utils::cast<Scalar>(0));
    std::vector<Scalar> padded_vector(data.size() + size * 2, Scalar(0.0));
    std::vector<Scalar> result(data.size(), Scalar(0.0));
    if (is_precisable<Scalar>::value){
        utils::set_vec_precision(zur_coeffs, utils::get_precision(data.at(0)));
        utils::set_vec_precision(padded_vector, utils::get_precision(data.at(0)));
        utils::set_vec_precision(result, utils::get_precision(data.at(0)));
    }
    std::copy(data.begin(), data.end(), padded_vector.begin() + size);

    for (size_t i{0}; i < coeffs.size(); ++i) {
        zur_coeffs[i] += utils::cast<Scalar>(coeffs[i]);
        zur_coeffs[i] /= utils::cast<Scalar>(utils::pow(m,k));
    }

    for (size_t i{0}; i < result.size(); ++i)
        for(size_t j{0}; j < size; ++j)
            result[i] += zur_coeffs[j] * padded_vector[i + j];

    return result;
}