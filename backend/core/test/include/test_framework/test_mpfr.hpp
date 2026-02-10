#ifndef TEST_MPFR_HPP
#define TEST_MPFR_HPP
#pragma once

#include <unsupported/Eigen/MatrixFunctions>

void test_intervals() {

    using arb = mpfr::mpreal;
    using interval = intprec::interval<arb>;

    // precision as double
    interval control_group(arb(0.5, mpfr::digits2bits(15)),
                                                  arb(1.002, mpfr::digits2bits(15)));
    // precision a bit more
    interval test_group(arb(0.5, mpfr::digits2bits(17)),
                                               arb(1.002, mpfr::digits2bits(17)));

    const std::vector<std::tuple<const char*, std::function<interval(const interval&)>>> math_funcs_to_test{
        std::make_tuple("EXP", utils::math<interval>::exp),
        std::make_tuple("SIN", utils::math<interval>::sin),
        std::make_tuple("COS", utils::math<interval>::cos),
        std::make_tuple("ATANH", utils::math<interval>::atanh),
    };

    for (const auto& x : math_funcs_to_test) {
        std::cout << "RESULT OF " << std::get<0>(x) << "\n";
        std::cout << "LEFT INTERVAL COMPARISON\n";
        std::cout << "15 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(control_group)) << "\n";
        std::cout << "17 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(test_group)) << "\n";
        std::cout << "\n\n";
    }
}

void test_eigen(){

    using arb = mpfr::mpreal;

    const size_t prec1 = utils::helpers<arb>::get_precision(arb(1.0));
    ::Eigen::Matrix<arb,3,3> control_matrix{
        {arb(1, mpfr::digits2bits(prec1)), arb(3.05, mpfr::digits2bits(prec1)), arb(6, mpfr::digits2bits(prec1))},
        {arb(5.9, mpfr::digits2bits(prec1)), arb(2, mpfr::digits2bits(prec1)), arb(17, mpfr::digits2bits(prec1))},
        {arb(71.1, mpfr::digits2bits(prec1)), arb(7.11, mpfr::digits2bits(prec1)), arb(5, mpfr::digits2bits(prec1))}
    };

    const size_t prec2 = prec1+2;
    ::Eigen::Matrix<arb,3,3> test_matrix{
        {arb(1, mpfr::digits2bits(prec2)), arb(3.05, mpfr::digits2bits(prec2)), arb(6, mpfr::digits2bits(prec2))},
        {arb(5.9, mpfr::digits2bits(prec2)), arb(2, mpfr::digits2bits(prec2)), arb(17, mpfr::digits2bits(prec2))},
        {arb(71.1, mpfr::digits2bits(prec2)), arb(7.11, mpfr::digits2bits(prec2)), arb(5, mpfr::digits2bits(prec2))}
    };

    std::cout << "RESULT OF DET(X)\n";
    std::cout << prec1 <<" digits " << utils::helpers<arb>::to_string(control_matrix.determinant())<< "\n";
    std::cout << prec2 <<" digits " << utils::helpers<arb>::to_string(test_matrix.determinant()) << "\n";
    std::cout << "\n\n";

}

#endif