#ifndef TEST_MPFR_HPP
#define TEST_MPFR_HPP
#pragma once

#include <unsupported/Eigen/MatrixFunctions>

template<typename T, typename U, typename V> const std::vector<
std::tuple<
const char*,
 std::function<T(const T&)>, 
 std::function<U(const U&)>, 
 std::function<V(const V&)>
 >> math_funcs_to_test{
    std::make_tuple("EXP", utils::math<T>::exp, utils::math<U>::exp, utils::math<V>::exp),
    std::make_tuple("SIN", utils::math<T>::sin, utils::math<U>::sin, utils::math<V>::sin),
    std::make_tuple("COS", utils::math<T>::cos, utils::math<U>::cos, utils::math<V>::cos),
    std::make_tuple("ATANH", utils::math<T>::atanh, utils::math<U>::atanh, utils::math<V>::atanh),
};

void test_mpreal(){

    float control_x_f = 0.501f;
    float control_x_d = 0.501;
    auto test_x = mpfr::mpreal(control_x_f, mp_prec_t{24});
    auto test_control_x = mpfr::mpreal(control_x_f, mp_prec_t{27});
    

    std::cout << "MPFR WITH 24 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x) << "\n";
    std::cout << "MPFR WITH 27 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_control_x) << "\n";
    std::cout << "FLOAT X ( 24 MANTISSA BITS) " << utils::helpers<float>::to_string(control_x_f) << "\n";
    std::cout << "DOUBLE X                    " << utils::helpers<double>::to_string(control_x_d) << "\n";

    for (const auto& x : math_funcs_to_test<mpfr::mpreal, float, double>) {
        std::cout << "RESULT OF " << std::get<0>(x) << "\n";
        std::cout << "SCALAR COMPARISON\n";
        std::cout << "MPFR WITH 24 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x)) << "\n";
        std::cout << "MPFR WITH 27 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_control_x)) << "\n";
        std::cout << "FLOAT X  (24 MANTISSA BITS) " << utils::helpers<float>::to_string(std::get<2>(x)(control_x_f)) << "\n";
        std::cout << "DOUBLE X                    " << utils::helpers<double>::to_string(std::get<3>(x)(control_x_d)) << "\n";
        std::cout << "\n\n";
    }
}

void test_intervals() {

    using arb = mpfr::mpreal;
    using interval_f = intprec::interval<float>;
    using interval_d = intprec::interval<double>;
    using interval = intprec::interval<arb>;

    const interval_f control_x_f(0.5f, 1.002f);
    const interval_d control_x_d(0.5, 1.002);

    // precision as double
    const interval control_group(arb(0.5, mp_prec_t{24}),
                                                  arb(1.002, mp_prec_t(24)));
    // precision a bit more
    const interval test_group(arb(0.5, mp_prec_t{27}),
                                               arb(1.002, mp_prec_t(27)));

    for (const auto& x : math_funcs_to_test<interval, interval_f, interval_d>) {
        std::cout << "RESULT OF " << std::get<0>(x) << "\n";
        std::cout << "LEFT INTERVAL COMPARISON\n";
        std::cout << "24 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(control_group)) << "\n";
        std::cout << "27 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(test_group)) << "\n";
        std::cout << "FLOAT    " << utils::helpers<interval_f>::to_string(std::get<2>(x)(control_x_f)) << "\n";
        std::cout << "DOUBLE    " << utils::helpers<interval_d>::to_string(std::get<3>(x)(control_x_d)) << "\n";
        std::cout << "\n\n";
    }
}

void test_eigen(){

    using arb = mpfr::mpreal;

    const mp_prec_t prec1{24};
    ::Eigen::Matrix<arb,3,3> control_matrix{
        {arb(1, prec1), arb(3.05, prec1), arb(6, prec1)},
        {arb(5.9, prec1), arb(2, prec1), arb(17, prec1)},
        {arb(71.1, prec1), arb(7.11, prec1), arb(5, prec1)}
    };

    const mp_prec_t prec2{27};
    ::Eigen::Matrix<arb,3,3> test_matrix{
        {arb(1, prec2), arb(3.05, prec2), arb(6, prec2)},
        {arb(5.9, prec2), arb(2, prec2), arb(17, prec2)},
        {arb(71.1, prec2), arb(7.11, prec2), arb(5, prec2)}
    };

    std::cout << "RESULT OF DET(X)\n";
    std::cout << prec1 <<" digits " << utils::helpers<arb>::to_string(control_matrix.determinant())<< "\n";
    std::cout << prec2 <<" digits " << utils::helpers<arb>::to_string(test_matrix.determinant()) << "\n";
    std::cout << "\n\n";

}

#endif