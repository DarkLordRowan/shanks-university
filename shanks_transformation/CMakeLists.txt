# CMakeList.txt: проект CMake для shanks-transformation; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (shanks_transformation "main.cpp" "series.h" "shanks_transformation.h" "epsilon_algorithm.h" "chang_whynn_algorithm.h" "epsilon_algorithm_two.h" "levin_algorithm.h" "levin_sidi_algorithm.h" "levin_sidi_M_algorithm.h" "rho_wynn_algorithm.h" "weniger_algorithm.h" "brezinski_theta_algorithm.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET shanks_transformation PROPERTY CXX_STANDARD 20)
endif()
