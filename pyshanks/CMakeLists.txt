cmake_minimum_required(VERSION 3.11)
project(pyshanks LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
include(FetchContent)

# -------------------------
# Pybind11 модуль
# -------------------------
FetchContent_Declare(pybind11 GIT_REPOSITORY https://github.com/pybind/pybind11.git GIT_TAG v2.13.6)
FetchContent_MakeAvailable(pybind11)
pybind11_add_module(pyshanks 
    ${CMAKE_SOURCE_DIR}/src/series_name.cpp
    ${CMAKE_SOURCE_DIR}/src/pyshanks.cpp
)

target_link_libraries(pyshanks PRIVATE /usr/local/lib/libmpfr.so /usr/local/lib/libgmp.so)
set_target_properties(pyshanks PROPERTIES PREFIX "")