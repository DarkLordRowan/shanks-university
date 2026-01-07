cmake_minimum_required(VERSION 3.11)
project(pyshanks LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
include(FetchContent)

# Add module path for custom Find modules
# Assuming backend/lib/CMakeLists.txt is run, CMAKE_SOURCE_DIR is backend/lib
# backend/cmake/modules is at ../cmake/modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules")

# Find required packages
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

# -------------------------
# Pybind11 модуль
# -------------------------
FetchContent_Declare(pybind11 GIT_REPOSITORY https://github.com/pybind/pybind11.git GIT_TAG v2.13.6)
FetchContent_MakeAvailable(pybind11)
pybind11_add_module(pyshanks 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/names.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pyshanks.cpp
)

target_link_libraries(pyshanks PRIVATE MPFR::MPFR GMP::GMP)
set_target_properties(pyshanks PROPERTIES PREFIX "")
