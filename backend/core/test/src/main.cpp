/**
 * @file main.cpp
 * @brief Entry point for the shanks-university sequence transformation application.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
// #include "../include/test_framework/test_framework.hpp"

/**
 * @brief Main entry point of the application.
 *
 * This function initiates the test framework, running a series of evaluations
 * on different sequence acceleration algorithms using various mathematical series.
 * It enters an infinite loop to allow continuous testing or monitoring.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @return int The exit status of the program.
 * @throws std::domain_error if a mathematical domain violation occurs during series evaluation.
 * @throws std::overflow_error if a numerical overflow or division by zero occurs.
 */
#include <complex>

// clang-format off
// Critical Section
#include "../../include/custom_concepts.hpp"
#include "gsl/gsl_sf_expint.h"
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_lambert.h>
#include "../../include/custom_types/intervalprecision.fwd.hpp"
#include "../../include/custom_types/mpreal.h"
#include "../../include/utils/utils.fwd.hpp"
#include "../../include/custom_types/intervalprecision.hpp"

#include "../include/test_framework/test_framework.hpp"

// clang-format on

int main() {
    std::cout << utils::math<mpfr::mpreal>::ci_x(mpfr::mpreal(3.0)) << "\n";
    // while (true) {
    //     try {
    //         main_testing_function<long double, unsigned short int>();
    //         main_testing_function<std::complex<long double>, unsigned long long int>();
    //         main_testing_function<intprec::interval<mpfr::mpreal>, unsigned short int>();
    //         main_testing_function<mpfr::mpreal, unsigned short int>();
    //     } catch (std::domain_error& e) {
    //         std::cout << e.what() << "\n";
    //     } catch (std::overflow_error& e) {
    //         std::cout << e.what() << "\n";
    //     }
    // }

    return 0;
}