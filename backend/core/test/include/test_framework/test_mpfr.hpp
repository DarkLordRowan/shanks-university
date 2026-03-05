#ifndef TEST_MPFR_HPP
#define TEST_MPFR_HPP
#pragma once

#include <fstream>
#include <thread>

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

void build_one_var_functions() {
    std::ofstream table1("table1.csv");
    using T = mpfr::mpreal;
    // build tables for single variable functions
    std::vector<std::tuple<const char*, std::function<T(const T&)>>> x_functions{
        std::make_tuple("EXP", utils::math<T>::exp),   std::make_tuple("SIN", utils::math<T>::sin),
        std::make_tuple("COS", utils::math<T>::cos),   std::make_tuple("SQRT", utils::math<T>::sqrt),
        std::make_tuple("ZETA", utils::math<T>::zeta), std::make_tuple("ERF", utils::math<T>::erf),
        std::make_tuple("LOG", utils::math<T>::log),   std::make_tuple("LOG10", utils::math<T>::log10)};

    // array of x vector, from 0.1, 0.5, 1.0, 15.0, 1000.0
    std::vector<const char*> arr_x_values{"0.13", "0.5732", "1.0452", "15.05235", "1000.0236"};

    // function, x, precision givven, bits_allocated, precision_got, f(x), f(x)_len, abs(f(x)_1000-f(x))
    table1 << "function,x,precision_givven,bits_allocated,f_x,f_x_len,abs_f_x_1000-f_x, abs_f_x_{n}-f_x_{n-1}\n";
    for (std::size_t precision = 15; precision <= 1000; ++precision) {
        for (const auto& x : arr_x_values) {
            mpfr::mpreal the_x(x, mp_prec_t(1000));
            for (const auto& f : x_functions) {
                mpfr::mpreal _x(x, mp_prec_t(precision));
                mpfr::mpreal _y(x, mp_prec_t(precision - 1ull));
                table1 << std::get<0>(f) << ","                                           // function
                       << utils::helpers<T>::to_string(_x) << ","                         // x
                       << precision << ","                                                // precision givven
                       << mpfr::digits2bits(precision) << ","                             // bits_allocated
                       << utils::helpers<T>::to_string(std::get<1>(f)(_x)) << ","         // f(x)
                       << utils::helpers<T>::to_string(std::get<1>(f)(_x)).size() << ","  // f(x)_len
                       << utils::math<T>::abs(std::get<1>(f)(the_x) - std::get<1>(f)(_x))
                       << ","  // abs(f(x,1000) - f(x))
                       << utils::math<T>::abs(std::get<1>(f)(_x) - std::get<1>(f)(_y))
                       << "\n";  // abs(f(x,n-1) - f(x,n))
            }
        }
    }

    table1.close();
}

void build_two_var_functions() {
    std::ofstream table2("table2.csv");
    using T = mpfr::mpreal;
    // build tables for single variable functions
    std::vector<std::tuple<const char*, std::function<T(const T&, const T&)>>> x_functions{
        std::make_tuple("POW(X,Y)", utils::math<T>::pow), std::make_tuple("ATAN(X,Y)", utils::math<T>::atan2),
        // std::make_tuple("INC_GAMMA_MPFR", utils::math<T>::inc_gamma ),
        std::make_tuple("OPERATOR+", [](const T& rhs, const T& lhs) { return rhs + lhs; }),
        std::make_tuple("OPERATOR-", [](const T& rhs, const T& lhs) { return rhs - lhs; }),
        std::make_tuple("OPERATOR*", [](const T& rhs, const T& lhs) { return rhs * lhs; }),
        std::make_tuple("OPERATOR/", [](const T& rhs, const T& lhs) { return rhs / lhs; })};

    std::vector<std::pair<const char*, const char*>> arr_x_values{{"0.1412", "0.5324"},
                                                                  {"0.5341", "20.13123"},
                                                                  {"1.012352", "0.0132425"},
                                                                  {"2.02345", "3.3333333"},
                                                                  {"100.02345", "424.13424"},
                                                                  {"532.1246234", "0.324263246"},
                                                                  {"0.00000145345", "1000000000000000001"},
                                                                  {"1000000000000000001", "0.000000000000000013"}};

    // function, x, x_precision_givven, x_bits_allocated, x_precision_got, y, y_precision_givven, y_bits_allocated,
    // y_precision_got, f(x,y), f(x,y)_len abs(f(x,y)_1000-f(x,y)),"
    table2 << "function,x,x_precision_givven,x_bits_allocated,x_precision_got,y,y_precision_givven,y_bits_allocated,y_"
              "precision_got,f_x_Y,f_x_y_len,abs_f_x_y_1000-f_x_y\n";
    for (std::size_t x_precision = 15; x_precision <= 1000; ++x_precision) {
        for (std::size_t y_precision = 15; y_precision <= 1000; ++y_precision) {
            for (const auto& [x, y] : arr_x_values) {
                mpfr::mpreal the_x(x, mpfr::digits2bits(1000));
                mpfr::mpreal the_y(y, mpfr::digits2bits(1000));
                for (const auto& f : x_functions) {
                    mpfr::mpreal _x(x, mpfr::digits2bits(x_precision));
                    mpfr::mpreal _y(y, mpfr::digits2bits(y_precision));
                    table2 << std::get<0>(f) << ","                                               // function
                           << utils::helpers<T>::to_string(_x) << ","                             // x
                           << x_precision << ","                                                  // precision givven
                           << mpfr::digits2bits(x_precision) << ","                               // bits_allocated
                           << mpfr::bits2digits(_x.get_prec()) << ","                             // precision got
                           << utils::helpers<T>::to_string(_y) << ","                             // y
                           << y_precision << ","                                                  // precision givven
                           << mpfr::digits2bits(y_precision) << ","                               // bits_allocated
                           << mpfr::bits2digits(_y.get_prec()) << ","                             // precision got
                           << utils::helpers<T>::to_string(std::get<1>(f)(_x, _y)) << ","         // f(x,y)
                           << utils::helpers<T>::to_string(std::get<1>(f)(_x, _y)).size() << ","  // f(x,y)_len
                           << utils::math<T>::abs(std::get<1>(f)(the_x, the_y) - std::get<1>(f)(_x, _y))
                           << "\n";  // abs(f(x,1000) - f(x))
                }
            }
        }
    }

    table2.close();
}

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

    build_one_var_functions();
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
