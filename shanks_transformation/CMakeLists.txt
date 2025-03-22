# CMakeList.txt: проект CMake для shanks-transformation; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (shanks_transformation "main.cpp" "series +.h" "wynn_numerators.h" "shanks_transformation.h" "epsilon_algorithm.h" "chang_whynn_algorithm.h" "epsilon_algorithm_two.h" "levin_algorithm.h" "levin_sidi_S_algorithm.h" "drummond_D_algorithm.h" "levin_sidi_M_algorithm.h" "rho_wynn_algorithm.h" "weniger_algorithm.h" "brezinski_theta_algorithm.h" "epsilon_algorithm_three.h" "remainders.h" "richardson_algorithm.h" "levin_recursion_algorithm.h")
target_compile_features(shanks_transformation PRIVATE cxx_std_20)