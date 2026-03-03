cmake_minimum_required(VERSION 3.15)
project(vizrd_ffi CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(Eigen3 REQUIRED)
find_library(GMP_LIBRARY gmp REQUIRED)
find_library(MPFR_LIBRARY mpfr REQUIRED)

add_library(shanks_ffi_cpp STATIC
    ffi/src/bridge_core.cpp
    ffi/src/bridge_f64.cpp
    ffi/src/bridge_arb.cpp
    ffi/src/series_registry_impl.cpp
)

# We need cxxbridge to generate the headers
find_program(CXXBRIDGE_CMD cxxbridge REQUIRED)

# Generate bridge headers
set(BRIDGE_HEADER "${CMAKE_CURRENT_BINARY_DIR}/vizrd/src/ffi/bridge.rs.h")
set(CXX_HEADER "${CMAKE_CURRENT_BINARY_DIR}/rust/cxx.h")

add_custom_command(
    OUTPUT ${BRIDGE_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/vizrd/src/ffi"
    COMMAND ${CXXBRIDGE_CMD} "${CMAKE_CURRENT_SOURCE_DIR}/src/ffi/bridge.rs" --header > ${BRIDGE_HEADER}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/ffi/bridge.rs"
)

add_custom_command(
    OUTPUT ${CXX_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/rust"
    COMMAND ${CXXBRIDGE_CMD} --header > ${CXX_HEADER}
)

add_custom_target(generate_cxx_headers DEPENDS ${BRIDGE_HEADER} ${CXX_HEADER})
add_dependencies(shanks_ffi_cpp generate_cxx_headers)

# Include paths
target_include_directories(shanks_ffi_cpp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ffi/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../backend/core/include
    ${CMAKE_CURRENT_BINARY_DIR} # To find vizrd/src/ffi/bridge.rs.h and rust/cxx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/.. # To resolve include "vizrd/ffi/include/bridge.hpp" from cxx output
    ${EIGEN3_INCLUDE_DIRS}
)

target_compile_definitions(shanks_ffi_cpp PUBLIC SHANKS_ENABLE_PROFILING)

# If MinGW, we might need some specific flags
if(MINGW)
    target_compile_options(shanks_ffi_cpp PRIVATE -Wa,-mbig-obj)
endif()
