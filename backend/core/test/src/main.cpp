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

enum choices { c_test_mpreal, c_test_intervals, c_test_eigen, c_test_series, c_test_algos };

int main() {
    while (true) {
        std::cout << "0 - test mpreal\n";
        std::cout << "1 - test interval\n";
        std::cout << "2 - test eigen\n";
        std::cout << "3 - test series\n";
        std::cout << "4 - test algos\n";
        int choice = console_IO<unsigned long>::input("TODO:");

        switch (choice) {
            case choices::c_test_mpreal: {
                console_effects::clear_console();
                test_mpreal();
                break;
            }
            case choices::c_test_intervals: {
                console_effects::clear_console();
                test_intervals();
                break;
            }
            case choices::c_test_series: {
                console_effects::clear_console();
                std::cout << "0 - float\n";
                std::cout << "1 - double\n";
                std::cout << "2 - long double\n";
                std::cout << "3 - mpreal\n";
                int type_to_use = console_IO<unsigned long>::input("type:");
                console_effects::clear_console();
                switch (type_to_use) {
                    case 0: {
                        test_series<float>();
                        break;
                    }
                    case 1: {
                        test_series<double>();
                        break;
                    }
                    case 2: {
                        test_series<long double>();
                        break;
                    }
                    case 3: {
                        test_series<mpfr::mpreal>();
                        break;
                    }
                }
                break;
            }
            case choices::c_test_algos: {
                console_effects::clear_console();
                std::cout << "0 - float\n";
                std::cout << "1 - double\n";
                std::cout << "2 - long double\n";
                std::cout << "3 - mpreal\n";
                int type_to_use = console_IO<unsigned long>::input("type:");

                std::cout << "0 - plain\n";
                std::cout << "1 - interval\n";
                std::cout << "2 - complex\n";
                int upper_type_to_use = console_IO<unsigned long>::input("upper_type:");

                console_effects::clear_console();

                switch (upper_type_to_use) {
                    case 0:
                        switch (type_to_use) {
                            case 0: {
                                main_testing_function<float, unsigned short int>();
                                break;
                            }
                            case 1: {
                                main_testing_function<double, unsigned short int>();
                                break;
                            }
                            case 2: {
                                main_testing_function<long double, unsigned short int>();
                                break;
                            }
                            case 3: {
                                main_testing_function<mpfr::mpreal, unsigned short int>();
                                break;
                            }
                        }
                        break;
                    case 1:
                        switch (type_to_use) {
                            case 0: {
                                main_testing_function<intprec::interval<float>, unsigned short int>();
                                break;
                            }
                            case 1: {
                                main_testing_function<intprec::interval<double>, unsigned short int>();
                                break;
                            }
                            case 2: {
                                main_testing_function<intprec::interval<long double>, unsigned short int>();
                                break;
                            }
                            case 3: {
                                main_testing_function<intprec::interval<mpfr::mpreal>, unsigned short int>();
                                break;
                            }
                        }
                        break;
                    case 2:
                        switch (type_to_use) {
                            case 0: {
                                main_testing_function<std::complex<float>, unsigned short int>();
                                break;
                            }
                            case 1: {
                                main_testing_function<std::complex<double>, unsigned short int>();
                                break;
                            }
                            case 2: {
                                main_testing_function<std::complex<long double>, unsigned short int>();
                                break;
                            }
                            case 3: {
                                main_testing_function<std::complex<mpfr::mpreal>, unsigned short int>();
                                break;
                            }
                        }
                        break;
                }
            }
        }

        console_effects::stop_console();
        console_effects::clear_console();
    }

    return 0;
}