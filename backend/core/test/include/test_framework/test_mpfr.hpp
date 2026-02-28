#ifndef TEST_MPFR_HPP
#define TEST_MPFR_HPP
#pragma once

#include <fstream>

template <typename T, typename U, typename V>
const std::vector<
    std::tuple<const char*, std::function<T(const T&)>, std::function<U(const U&)>, std::function<V(const V&)>>>
    math_funcs_to_test{
        std::make_tuple("EXP", utils::math<T>::exp, utils::math<U>::exp, utils::math<V>::exp),
        std::make_tuple("SIN", utils::math<T>::sin, utils::math<U>::sin, utils::math<V>::sin),
        std::make_tuple("COS", utils::math<T>::cos, utils::math<U>::cos, utils::math<V>::cos),
        std::make_tuple("ATANH", utils::math<T>::atanh, utils::math<U>::atanh, utils::math<V>::atanh),
        std::make_tuple("SQRT", utils::math<T>::sqrt, utils::math<U>::sqrt, utils::math<V>::sqrt),
        std::make_tuple("LOG", utils::math<T>::log, utils::math<U>::log, utils::math<V>::log),
    };

void test_mpreal() {
    float control_x_f = 0.501f;
    double control_x_d = 0.501;
    auto test_x1 = mpfr::mpreal(control_x_f, mp_prec_t{24});
    auto test_x2 = mpfr::mpreal(control_x_f, mp_prec_t{25});
    auto test_x3 = mpfr::mpreal(control_x_f, mp_prec_t{26});
    auto test_x4 = mpfr::mpreal(control_x_f, mp_prec_t{27});
    auto test_x5 = mpfr::mpreal(control_x_d, mp_prec_t{53});
    auto test_x6 = mpfr::mpreal(control_x_d, mp_prec_t{54});
    auto test_x7 = mpfr::mpreal(control_x_d, mp_prec_t{55});
    auto test_x8 = mpfr::mpreal(control_x_d, mp_prec_t{56});

    std::cout << "MPFR WITH 24 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x1) << "\n";
    std::cout << "MPFR WITH 25 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x2) << "\n";
    std::cout << "MPFR WITH 26 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x3) << "\n";
    std::cout << "MPFR WITH 27 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x4) << "\n";
    std::cout << "FLOAT X ( 24 MANTISSA BITS) " << utils::helpers<float>::to_string(control_x_f) << "\n";
    std::cout << "MPFR WITH 53 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x5) << "\n";
    std::cout << "MPFR WITH 54 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x6) << "\n";
    std::cout << "MPFR WITH 55 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x7) << "\n";
    std::cout << "MPFR WITH 56 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(test_x8) << "\n";
    std::cout << "DOUBLE X                    " << utils::helpers<double>::to_string(control_x_d) << "\n\n";

    for (const auto& x : math_funcs_to_test<mpfr::mpreal, float, double>) {
        std::cout << "RESULT OF " << std::get<0>(x) << "\n";
        std::cout << "SCALAR COMPARISON\n";
        std::cout << "MPFR WITH 24 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x1))
                  << "\n";
        std::cout << "MPFR WITH 25 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x2))
                  << "\n";
        std::cout << "MPFR WITH 26 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x3))
                  << "\n";
        std::cout << "MPFR WITH 27 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x4))
                  << "\n";
        std::cout << "FLOAT X  (24 MANTISSA BITS) " << utils::helpers<float>::to_string(std::get<2>(x)(control_x_f))
                  << "\n";
        std::cout << "MPFR WITH 53 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x5))
                  << "\n";
        std::cout << "MPFR WITH 54 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x6))
                  << "\n";
        std::cout << "MPFR WITH 55 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x7))
                  << "\n";
        std::cout << "MPFR WITH 56 MANTISSA BITS  " << utils::helpers<mpfr::mpreal>::to_string(std::get<1>(x)(test_x8))
                  << "\n";
        std::cout << "DOUBLE X (53 MANTISSA BITS) " << utils::helpers<double>::to_string(std::get<3>(x)(control_x_d))
                  << "\n";
        std::cout << "\n\n";
    }

    std::ofstream fout("mpfr_sin.csv");
    fout << "precision" << "," << "sin(0.25)_precision" << "," << "sin(0.25)_1000-sin(0.25)_precision" << ","    << "sin(0.25)_precision+1-sin(0.25)_precision" << ","
                               << "exp(0.25)_precision" << "," << "exp(0.25)_1000-exp(0.25)_precision" << ","    << "exp(0.25)_precision+1-exp(0.25)_precision" << ","
                               << "sqrt(9.13)_precision" << "," << "sqrt(9.13)_1000-sqrt(9.13)_precision" << "," << "sqrt(9.13)_precision+1-sqrt(9.13)_precision" << ","
                               << "zeta(2.0)_precision" << "," << "zeta(2.0)_1000-zeta(2.0)_precision" << ","    << "zeta(2.0)_precision+1-zeta(2.0)_precision" << "\n";
    for(int i{1000}; i > 15; --i){
        fout << i << ","
        //sin
        << utils::helpers<mpfr::mpreal>::to_string(mpfr::sin(mpfr::mpreal("0.25", mp_prec_t(i)))) << "," 
        << mpfr::sin(mpfr::mpreal("0.25", mp_prec_t(1000))) - mpfr::sin(mpfr::mpreal("0.25", mp_prec_t(i))) << ","
        << mpfr::sin(mpfr::mpreal("0.25", mp_prec_t(i+1))) -  mpfr::sin(mpfr::mpreal("0.25", mp_prec_t(i))) << ","
        //exp
        << utils::helpers<mpfr::mpreal>::to_string(mpfr::exp(mpfr::mpreal("0.25", mp_prec_t(i)))) << "," 
        << mpfr::exp(mpfr::mpreal("0.25", mp_prec_t(1000)))   - mpfr::exp(mpfr::mpreal("0.25", mp_prec_t(i))) << ","
        << mpfr::exp(mpfr::mpreal("0.25", mp_prec_t(i+1)))    - mpfr::exp(mpfr::mpreal("0.25", mp_prec_t(i))) << ","
        //sqrt
        << utils::helpers<mpfr::mpreal>::to_string(mpfr::sqrt(mpfr::mpreal("9.13", mp_prec_t(i)))) << "," 
        << mpfr::sqrt(mpfr::mpreal("9.13", mp_prec_t(1000)))   - mpfr::sqrt(mpfr::mpreal("9.13", mp_prec_t(i))) << ","
        << mpfr::sqrt(mpfr::mpreal("9.13", mp_prec_t(i+1)))    - mpfr::sqrt(mpfr::mpreal("9.13", mp_prec_t(i))) << ","
        //zeta
        << utils::helpers<mpfr::mpreal>::to_string(mpfr::zeta(mpfr::mpreal("2", mp_prec_t(i)))) << "," 
        << mpfr::zeta(mpfr::mpreal("2", mp_prec_t(1000)))   - mpfr::zeta(mpfr::mpreal("2", mp_prec_t(i))) << ","
        << mpfr::zeta(mpfr::mpreal("2", mp_prec_t(i+1)))    - mpfr::zeta(mpfr::mpreal("2", mp_prec_t(i)))
        << "\n";
    }

    fout.close();


}

void test_intervals() {
    using arb = mpfr::mpreal;
    using interval_f = intprec::interval<float>;
    using interval_d = intprec::interval<double>;
    using interval = intprec::interval<arb>;

    const interval_f control_x_f(0.5f, 1.002f);
    const interval_d control_x_d(0.5, 1.002);

    const interval test_x1(arb(0.5, mp_prec_t{24}), arb(1.002, mp_prec_t(24)));
    const interval test_x2(arb(0.5, mp_prec_t{27}), arb(1.002, mp_prec_t(27)));
    const interval test_x3(arb(0.5, mp_prec_t{53}), arb(1.002, mp_prec_t(53)));
    const interval test_x4(arb(0.5, mp_prec_t{56}), arb(1.002, mp_prec_t(56)));

    for (const auto& x : math_funcs_to_test<interval, interval_f, interval_d>) {
        std::cout << "RESULT OF " << std::get<0>(x) << "\n";
        std::cout << "24 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(test_x1)) << "\n";
        std::cout << "27 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(test_x2)) << "\n";
        std::cout << "FLOAT    " << utils::helpers<interval_f>::to_string(std::get<2>(x)(control_x_f)) << "\n";
        std::cout << "53 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(test_x3)) << "\n";
        std::cout << "56 digits" << utils::helpers<interval>::to_string(std::get<1>(x)(test_x4)) << "\n";
        std::cout << "DOUBLE   " << utils::helpers<interval_d>::to_string(std::get<3>(x)(control_x_d)) << "\n";
        std::cout << "\n\n";
    }
}

void test_eigen() {
    using arb = mpfr::mpreal;

    const mp_prec_t prec1{24};
    ::Eigen::Matrix<arb, 3, 3> control_matrix{{arb(1, prec1), arb(3.05, prec1), arb(6, prec1)},
                                              {arb(5.9, prec1), arb(2, prec1), arb(17, prec1)},
                                              {arb(71.1, prec1), arb(7.11, prec1), arb(5, prec1)}};

    const mp_prec_t prec2{27};
    ::Eigen::Matrix<arb, 3, 3> test_matrix{{arb(1, prec2), arb(3.05, prec2), arb(6, prec2)},
                                           {arb(5.9, prec2), arb(2, prec2), arb(17, prec2)},
                                           {arb(71.1, prec2), arb(7.11, prec2), arb(5, prec2)}};

    std::cout << "RESULT OF DET(X)\n";
    std::cout << prec1 << " digits " << utils::helpers<arb>::to_string(control_matrix.determinant()) << "\n";
    std::cout << prec2 << " digits " << utils::helpers<arb>::to_string(test_matrix.determinant()) << "\n";
    std::cout << "\n\n";
}

#endif