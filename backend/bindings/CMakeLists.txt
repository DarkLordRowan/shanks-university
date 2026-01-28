cmake_minimum_required(VERSION 3.11)
project(pyshanks LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# Use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable LTO for development speed. Enable for production if absolute performance is needed.
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

# Use mold linker if available
find_program(MOLD_PATH mold)
if(MOLD_PATH)
    add_link_options("-fuse-ld=mold")
endif()

include(FetchContent)

# Add module path for custom Find modules
# Assuming backend/bindings/CMakeLists.txt is run, CMAKE_SOURCE_DIR is backend/bindings
# backend/cmake/modules is at ../cmake/modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules")

# Find required packages
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GSL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/include
)

# -------------------------
# Pybind11 модуль
# -------------------------
FetchContent_Declare(pybind11 GIT_REPOSITORY https://github.com/pybind/pybind11.git GIT_TAG v2.13.6)
FetchContent_MakeAvailable(pybind11)
# Option for profiling
option(SHANKS_ENABLE_PROFILING "Enable operation counting" OFF)
if(SHANKS_ENABLE_PROFILING)
    add_definitions(-DSHANKS_ENABLE_PROFILING)
endif()

pybind11_add_module(pyshanks 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pyshanks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bind_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bind_series_real.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bind_series_complex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bind_algos_real.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bind_algos_complex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bind_noise.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bind_filters.cpp
)
target_link_libraries(pyshanks PRIVATE MPFR::MPFR GMP::GMP Eigen3::Eigen GSL::gsl GSL::gslcblas)
set_target_properties(pyshanks PROPERTIES PREFIX "")
