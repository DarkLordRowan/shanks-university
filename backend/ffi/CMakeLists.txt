cmake_minimum_required(VERSION 3.11)
project(shanks_ffi LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Disable LTO for development speed
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

# Use mold linker if available
find_program(MOLD_PATH mold)
if(MOLD_PATH)
    add_link_options("-fuse-ld=mold")
endif()

# Add module path for custom Find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules")

# Find required packages
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GSL REQUIRED)

# Option for profiling
option(SHANKS_ENABLE_PROFILING "Enable operation counting" ON)
if(SHANKS_ENABLE_PROFILING)
    add_definitions(-DSHANKS_ENABLE_PROFILING)
endif()

# Create shared library
add_library(shanks_ffi SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shanks_ffi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/series_ffi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/accel_ffi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/registry_ffi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/series_registry_impl.cpp
)

target_include_directories(shanks_ffi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/include
)

target_link_libraries(shanks_ffi PUBLIC
    MPFR::MPFR
    GMP::GMP
    Eigen3::Eigen
    GSL::gsl
    GSL::gslcblas
)

# Set output directory
set_target_properties(shanks_ffi PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Export symbols for shared library
# SHANKS_FFI_SHARED_BUILD activates extern template declarations in series.hpp
# so that only series_registry_impl.cpp emits the static-local definitions.
target_compile_definitions(shanks_ffi PRIVATE SHANKS_FFI_SHARED_BUILD)
if(WIN32)
    target_compile_definitions(shanks_ffi PRIVATE SHANKS_FFI_EXPORTS)
endif()

# Create test executable
add_executable(test_ffi ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ffi.cpp)
target_link_libraries(test_ffi PRIVATE shanks_ffi)
set_target_properties(test_ffi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
