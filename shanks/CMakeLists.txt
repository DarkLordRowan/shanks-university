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

target_link_libraries(shanks PRIVATE /usr/local/lib/libmpfr.so /usr/local/lib/libgmp.so)
