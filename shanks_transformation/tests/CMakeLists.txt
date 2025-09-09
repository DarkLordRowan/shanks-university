cmake_minimum_required(VERSION 3.20)
project(brezinski_tests CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest
include(FetchContent)
FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_brezinski_theta brezinski_theta_test.cpp)

# Дадим тестам видеть заголовки из соседней папки
# ${CMAKE_CURRENT_LIST_DIR} == tests/
target_include_directories(test_brezinski_theta PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/..        # корень проекта
        ${CMAKE_CURRENT_LIST_DIR}/../methods
)

target_link_libraries(test_brezinski_theta PRIVATE GTest::gtest_main)
