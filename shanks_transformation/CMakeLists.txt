cmake_minimum_required(VERSION 3.11)
project(shanks_transformation LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Бинарник (основная программа)
# -------------------------
add_executable(shanks_transformation main.cpp)
target_include_directories(shanks_transformation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# -------------------------
# Pybind11 модуль (Python entrypoint)
# -------------------------
find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(pyshanks py.cpp)
target_include_directories(pyshanks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# -------------------------
# Тесты (GoogleTest) — автоподхват всех *.cpp в ./tests/**
# -------------------------
include(FetchContent)
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