cmake_minimum_required(VERSION 3.11)
project(shanks LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

# Use mold linker if available
find_program(MOLD_PATH mold)
if(MOLD_PATH)
    add_link_options("-fuse-ld=mold")
endif()

include(FetchContent)

# Add module path for custom Find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake/modules")

# Find required packages
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GSL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# -------------------------
# Бинарник (основная программа)
# -------------------------

add_executable(shanks
    ${CMAKE_SOURCE_DIR}/main.cpp
)

target_link_libraries(shanks PRIVATE MPFR::MPFR GMP::GMP Eigen3::Eigen GSL::gsl GSL::gslcblas)
