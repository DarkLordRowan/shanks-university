cmake_minimum_required(VERSION 3.11)
project(shanks LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
include(FetchContent)

# -------------------------
# Бинарник (основная программа)
# -------------------------


#include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(shanks 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/interfaces/series_info.cpp
    ${CMAKE_SOURCE_DIR}/src/interfaces/test_funcs_info.cpp
    ${CMAKE_SOURCE_DIR}/src/interfaces/transformations_info.cpp
)

target_link_libraries(shanks_transformation PRIVATE /usr/local/lib/libmpfr.so /usr/local/lib/libgmp.so)

# -------------------------
# Тесты (GoogleTest) — автоподхват всех *.cpp в ./tests/**
# -------------------------
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
add_executable(all_tests ${TEST_SOURCES})
target_include_directories(all_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/methods)
target_link_libraries(all_tests PRIVATE GTest::gtest_main)
gtest_discover_tests(all_tests)
