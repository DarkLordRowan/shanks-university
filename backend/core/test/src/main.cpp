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
#define DEBUG
#define SHANKS_ENABLE_PROFILING
// clang-format off
// Critical Section
#include "../../include/lib.hpp"
#include "../include/test_framework/test_framework.hpp"
// clang-format on

int main() {
    test_intervals();
    test_eigen();

    //while (true) {
    //    try {
    //        main_testing_function<mpfr::mpreal, unsigned short int>();
    //        main_testing_function<intprec::interval<shanks::profiling::OperationCounting<double>>,
    //                              unsigned short int>();
    //        main_testing_function<double, unsigned short int>();
    //        main_testing_function<std::complex<long double>, unsigned long long int>();
    //        main_testing_function<std::complex<shanks::profiling::OperationCounting<double>>, unsigned long long int>();
    //        main_testing_function<intprec::interval<mpfr::mpreal>, unsigned short int>();
    //    } catch (std::domain_error& e) {
    //        std::cout << e.what() << "\n";
    //    } catch (std::overflow_error& e) {
    //        std::cout << e.what() << "\n";
    //    }
    //}

    return 0;
}