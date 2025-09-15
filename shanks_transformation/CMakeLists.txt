cmake_minimum_required(VERSION 3.11)
project(shanks_transformation LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# -------------------------
# Бинарник (основная программа)
# -------------------------
add_executable(shanks_transformation main.cpp)
target_include_directories(shanks_transformation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# -------------------------
# Pybind11 модуль
# -------------------------
FetchContent_Declare(pybind11 GIT_REPOSITORY https://github.com/pybind/pybind11.git GIT_TAG v2.13.6)
FetchContent_MakeAvailable(pybind11)
pybind11_add_module(pyshanks py.cpp)
target_include_directories(pyshanks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(pyshanks PROPERTIES PREFIX "")

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
